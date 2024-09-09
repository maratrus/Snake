#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <unistd.h>
#include <stdint.h>
#include <ctype.h>
#include <time.h>
#include <windows.h>

#define MAX_X 20 // Размер поля
#define MAX_Y 20
#define CONTROLS 5 // Количество кнопок для управления
#define START_DELAY 100000 // Начальная задержка

// Определение направлений
#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3
#define PAUSE 4

typedef struct tail_t {
    int x;
    int y;
} tail_t;

typedef struct food_t {
    int x;
    int y;
} food_t;

typedef struct snake_t {
    int x;
    int y;
    tail_t* tail;
    size_t tsize;
    int direction; // Текущее направление
    int level; // Уровень
    int delay; // Задержка
} snake_t;

// Структура для управления
typedef struct control_buttons {
    char key;       // Клавиша управления
    int direction;  // Направление
} control_buttons;

// Определяем массив управления
control_buttons default_controls[CONTROLS] = {
    {'a', LEFT},   // Влево
    {'d', RIGHT},  // Вправо
    {'w', UP},     // Вверх
    {'s', DOWN},   // Вниз
    {'p', PAUSE}   // Пауза
};

// Функция для установки цвета
void setColor(int textColor, int bgColor) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (WORD)((bgColor << 4) | textColor));
}

// Инициализация змеи
snake_t initSnake(int x, int y, size_t tsize) {
    snake_t snake;
    snake.x = x;
    snake.y = y;
    snake.tsize = tsize;
    snake.tail = (tail_t*)malloc(sizeof(tail_t) * 100);
    for (int i = 0; i < tsize; ++i) {
        snake.tail[i].x = x - (i + 1); // Хвост перед головой
        snake.tail[i].y = y;
    }
    snake.direction = RIGHT; // Начальное движение вправо
    snake.level = 0; // Изначально уровень 0
    snake.delay = START_DELAY; // Начальная задержка
    return snake;
}

food_t generateFood() {
    food_t food;
    do {
        food.x = rand() % MAX_X; // Генерируем координаты еды
        food.y = rand() % MAX_Y;
    } while (food.x == 0 && food.y == 0); // Убедимся, что пища не появляется в начале змейки
    return food;
}

void printSnake(snake_t snake, food_t food) {
    char matrix[MAX_X][MAX_Y];
    for (int i = 0; i < MAX_X; ++i) {
        for (int j = 0; j < MAX_Y; ++j) {
            matrix[i][j] = ' ';
        }
    }

    // Отображаем змейку
    matrix[snake.x][snake.y] = '@';
    for (int i = 0; i < snake.tsize; ++i) {
        matrix[snake.tail[i].x][snake.tail[i].y] = '*';
    }

    // Отображаем еду
    matrix[food.x][food.y] = 'O';

    for (int j = 0; j < MAX_Y; ++j) {
        for (int i = 0; i < MAX_X; ++i) {
            if (matrix[i][j] == '@') {
                setColor(2, 0); // Цвет для змейки (зеленый)
            } else if (matrix[i][j] == 'O') {
                setColor(4, 0); // Цвет для еды (красный)
            } else {
                setColor(7, 0); // Стандартный цвет (белый)
            }
            printf("%c", matrix[i][j]);
        }
        printf("\n");
    }

    setColor(7, 0); // Сброс цвета к стандартному
    printf("Уровень: %d\n", snake.level);
}

int checkCollision(snake_t* snake) {
    // Проверка столкновения с границами
    if (snake->x < 0 || snake->x >= MAX_X || snake->y < 0 || snake->y >= MAX_Y) {
        return 1; // Столкновение с границей
    }
    // Проверка столкновения с самим собой
    for (int i = 0; i < snake->tsize; ++i) {
        if (snake->tail[i].x == snake->x && snake->tail[i].y == snake->y) {
            return 1; // Столкновение с телом
        }
    }
    return 0; // Нет столкновений
}

snake_t move(snake_t snake) {
    // Сохраняем положение хвоста
    for (int i = snake.tsize - 1; i > 0; i--) {
        snake.tail[i] = snake.tail[i - 1];
    }
    snake.tail[0].x = snake.x;
    snake.tail[0].y = snake.y;

    // Обновляем координаты змейки в зависимости от направления
    switch (snake.direction) {
        case LEFT:
            snake.x--;
            if (snake.x < 0) snake.x = MAX_X - 1; // Переход через границу
            break;
        case RIGHT:
            snake.x++;
            if (snake.x >= MAX_X) snake.x = 0; // Переход через границу
            break;
        case UP:
            snake.y--;
            if (snake.y < 0) snake.y = MAX_Y - 1; // Переход через границу
            break;
        case DOWN:
            snake.y++;
            if (snake.y >= MAX_Y) snake.y = 0; // Переход через границу
            break;
    }
    return snake;
}

void increaseLevel(snake_t* snake) {
    snake->level++;
    snake->delay = START_DELAY - (snake->level * 10000); // Уменьшаем задержку
    if (snake->delay < 50000) {
        snake->delay = 50000; // Минимальная задержка 50 мс
    }
}

void printExit(snake_t* snake) {
    printf("Игра окончена! Ваш уровень: %d\n", snake->level);
}

void startMenu() {
    printf("Добро пожаловать в игру 'Змейка'!\n");
    printf("Нажмите любую клавишу, чтобы начать игру...\n");
    _getch(); // Ждем ввода игрока
}

int main() {
    srand(time(NULL)); // Инициализация генератора случайных чисел
    startMenu(); // Запуск стартового меню

    snake_t snake = initSnake(5, 5, 5); // Инициализируем змейку
    food_t food = generateFood(); // Генерируем первую еду

    int paused = 0;
    while (1) {
        if (!paused) {
            system("cls"); // Очистка экрана
            printSnake(snake, food); // Отображаем змейку и еду

            // Проверка столкновения
            if (checkCollision(&snake)) {
                printExit(&snake); // Закрытие игры
                break; // Выход из игры при столкновении
            }

            // Проверка ввода пользователя
            if (_kbhit()) {
                char key = _getch();
                for (int i = 0; i < CONTROLS; i++) {
                    if (key == default_controls[i].key) {
                        if (i != PAUSE) { // PAUSE
                            snake.direction = default_controls[i].direction;
                        } else {
                            paused = !paused; // Переключение паузы
                        }
                    }
                }
            }

            snake = move(snake); // Перемещение змейки

            // Проверка, съела ли змейка еду
            if (snake.x == food.x && snake.y == food.y) {
                increaseLevel(&snake); // Увеличиваем уровень
                food = generateFood(); // Генерируем новую еду
            }

            usleep(snake.delay); // Задержка для визуализации
        } else {
            printf("Игра на паузе. Нажмите 'P', чтобы продолжить.\n");
            usleep(100000); // Задержка во время паузы
        }
    }

    free(snake.tail); // Освобождение памяти для хвоста змейки
    return 0;
}

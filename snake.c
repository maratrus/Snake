#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <unistd.h>
#include <stdint.h>
#include <ctype.h>
#include <time.h>

#define MAX_X 20 // Размер поля
#define MAX_Y 20
#define CONTROLS 4 // Количество кнопок для управления
#define START_DELAY 100000 // Начальная задержка

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

typedef struct control_buttons {
    char key;       // Клавиша управления
    int direction;  // Направление
} control_buttons;

// Определяем массив управления
control_buttons default_controls[CONTROLS] = {
    {'a', 0}, // Влево
    {'d', 1}, // Вправо
    {'w', 2}, // Вверх
    {'s', 3}, // Вниз
    {'p', 4}  // Пауза
};

// Инициализация змеи
snake_t initSnake(int x, int y, size_t tsize) {
    snake_t snake;
    snake.x = x;
    snake.y = y;
    snake.tsize = tsize;
    snake.tail = (tail_t*)malloc(sizeof(tail_t) * 100);
    for (int i = 0; i < tsize; ++i) {
        snake.tail[i].x = x + i + 1;
        snake.tail[i].y = y;
    }
    snake.direction = 1; // Начальное движение вправо
    snake.level = 0; // Изначально уровень 0
    snake.delay = START_DELAY; // Начальная задержка
    return snake;
}

food_t generateFood() {
    food_t food;
    food.x = rand() % MAX_X; // Генерируем координаты еды
    food.y = rand() % MAX_Y;
    return food;
}

void printSnake(snake_t snake, food_t food) {
    char matrix[MAX_X][MAX_Y];
    for (int i = 0; i < MAX_X; ++i) {
        for (int j = 0; j < MAX_Y; ++j) {
            matrix[i][j] = ' ';
        }
    }

    matrix[snake.x][snake.y] = '@';
    for (int i = 0; i < snake.tsize; ++i) {
        matrix[snake.tail[i].x][snake.tail[i].y] = '*';
    }

    matrix[food.x][food.y] = 'O'; // Отображение еды

    for (int j = 0; j < MAX_Y; ++j) {
        for (int i = 0; i < MAX_X; ++i) {
            printf("%c", matrix[i][j]);
        }
        printf("\n");
    }

    printf("Уровень: %d\n", snake.level);
}

int checkDirection(snake_t* snake, int newDirection) {
    // Запрещаем движение в противоположном направлении
    if ((newDirection == 0 && snake->direction == 1) ||
        (newDirection == 1 && snake->direction == 0) ||
        (newDirection == 2 && snake->direction == 3) ||
        (newDirection == 3 && snake->direction == 2)) {
        return 0; // Изменение направления не разрешено
    }
    return 1; // Разрешаем изменение направления
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
        case 0: // Влево
            snake.x--;
            if (snake.x < 0) snake.x = MAX_X - 1; // Переход через границу
            break;
        case 1: // Вправо
            snake.x++;
            if (snake.x >= MAX_X) snake.x = 0; // Переход через границу
            break;
        case 2: // Вверх
            snake.y--;
            if (snake.y < 0) snake.y = MAX_Y - 1; // Переход через границу
            break;
        case 3: // Вниз
            snake.y++;
            if (snake.y >= MAX_Y) snake.y = 0; // Переход через границу
            break;
    }

    return snake;
}

void increaseLevel(snake_t* snake) {
    snake->level++;
    snake->delay = START_DELAY - (snake->level * 5000); // Уменьшаем задержку
    if (snake->delay < 50000) {
        snake->delay = 50000; // Минимальная задержка 50 мс
    }
}

void printExit(snake_t* snake) {
    printf("Игра окончена! Ваш уровень: %d\n", snake->level);
}

void processInput(snake_t* snake, int* paused) {
    if (_kbhit()) {
        char key = _getch();
        key = tolower(key);
        int newDirection = -1;

        for (int i = 0; i < CONTROLS; i++) {
            if (key == default_controls[i].key) {
                newDirection = default_controls[i].direction;
                break;
            }
        }

        if (newDirection == 4) { // Пауза
            *paused = !(*paused);
        } else if (*paused && newDirection != -1) {
            return; // Если игра на паузе, просто возвращаем
        } else if (newDirection != -1 && checkDirection(snake, newDirection)) {
            snake->direction = newDirection; // Изменяем направление только если оно валидно
        }
    }
}

// Основной цикл игры
int main() {
    srand(time(NULL)); // Инициализация генератора случайных чисел
    snake_t snake = initSnake(5, 5, 3);
    food_t food = generateFood(); // Генерируем первую еду
    int paused = 0;

    while (1) {
        if (!paused) {
            printSnake(snake, food);
            processInput(&snake, &paused);
            snake = move(snake); // Перемещаем змейку

            // Проверяем, съела ли змейка еду
            if (snake.x == food.x && snake.y == food.y) {
                snake.tsize++; // Увеличиваем размер змейки
                increaseLevel(&snake); // Увеличиваем уровень
                food = generateFood(); // Генерируем новую еду
            }

            usleep(snake.delay); // Задержка для визуализации
            system("cls"); // Очистка экрана (Windows)
        } else {
            printf("Игра на паузе. Нажмите 'P', чтобы продолжить.\n");
            usleep(100000); // Задержка во время паузы
        }
    }

    printExit(&snake);
    free(snake.tail); // Освобождение памяти
    return 0;
}

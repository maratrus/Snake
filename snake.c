#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <unistd.h>
#include <stdint.h>
#include <ctype.h>

#define MAX_X 15
#define MAX_Y 15
#define CONTROLS 4 

typedef struct tail_t {
    int x;
    int y;
} tail_t;

typedef struct snake_t {
    int x;
    int y;
    struct tail_t* tail;
    size_t tsize;
    int direction; // Добавляем направление
} snake_t;

typedef struct control_buttons {
    char key;       // Клавиша управления
    int direction;  // направления
} control_buttons;

// Определяем массив управления
control_buttons default_controls[CONTROLS] = {
    {'a', 0}, // Лево
    {'d', 1}, // Право
    {'w', 2}, // Наверх
    {'s', 3}  // Вниз
};

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
    snake.direction = 1; // Начнем движение вправо
    return snake;
}

void printSnake(snake_t snake) {
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

    for (int j = 0; j < MAX_Y; ++j) {
        for (int i = 0; i < MAX_X; ++i) {
            printf("%c", matrix[i][j]);
        }
        printf("\n");
    }
}

int checkDirection(snake_t* snake, int newDirection) {
    // Запрещвем перемещения, которые приводят к столкновению с хвостом
    if (newDirection == 0 && snake->direction == 1) return 0;
    if (newDirection == 1 && snake->direction == 0) return 0;
    if (newDirection == 2 && snake->direction == 3) return 0;
    if (newDirection == 3 && snake->direction == 2) return 0; 

    return 1;
}

snake_t moveLeft(snake_t snake) {
    for (int i = snake.tsize - 1; i > 0; i--) {
        snake.tail[i] = snake.tail[i - 1];
    }
    snake.tail[0].x = snake.x;
    snake.tail[0].y = snake.y;

    snake.x = snake.x - 1;
    if (snake.x < 0) {
        snake.x = MAX_X - 1; // Змейка переходит с одного края на другой
    }
    return snake;
}

snake_t moveRight(snake_t snake) {
    for (int i = snake.tsize - 1; i > 0; i--) {
        snake.tail[i] = snake.tail[i - 1];
    }
    snake.tail[0].x = snake.x;
    snake.tail[0].y = snake.y;

    snake.x = snake.x + 1;
    if (snake.x >= MAX_X) {
        snake.x = 0; // Змейка переходит с одного края на другой
    }
    return snake;
}

snake_t moveUp(snake_t snake) {
    for (int i = snake.tsize - 1; i > 0; i--) {
        snake.tail[i] = snake.tail[i - 1];
    }
    snake.tail[0].x = snake.x;
    snake.tail[0].y = snake.y;

    snake.y = snake.y - 1;
    if (snake.y < 0) {

        snake.y = MAX_Y - 1; // Змейка переходит с одного края на другой
    }
    return snake;
}

snake_t moveDown(snake_t snake) {
    for (int i = snake.tsize - 1; i > 0; i--) {
        snake.tail[i] = snake.tail[i - 1];
    }
    snake.tail[0].x = snake.x;
    snake.tail[0].y = snake.y;

    snake.y = snake.y + 1;
    if (snake.y >= MAX_Y) {
        snake.y = 0; // Змейка переходит с одного края на другой
    }
    return snake;
}

void processInput(snake_t* snake) {
    if (_kbhit()) {
        char key = _getch();
        key = tolower(key); // Приводим к нижнему регистру
        int newDirection = -1;

        for (int i = 0; i < CONTROLS; i++) {
            if (key == default_controls[i].key) {
                newDirection = default_controls[i].direction;
                break;
            }
        }

        if (newDirection != -1 && checkDirection(snake, newDirection)) {
            snake->direction = newDirection; // Изменяем направление
        }
    }
}

// Основная функция
int main() {
    snake_t snake = initSnake(10, 5, 2);
    printSnake(snake);

    while (1) {
        processInput(&snake);

        switch (snake.direction) {
            case 0:
                snake = moveLeft(snake);
                break;
            case 1:
                snake = moveRight(snake);
                break;
            case 2:
                snake = moveUp(snake);
                break;
            case 3:
                snake = moveDown(snake);
                break;
        }

        usleep(100000);
        system("cls");
        printSnake(snake);
    }

    free(snake.tail);
    return 0;
}

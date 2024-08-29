#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <unistd.h>
#include <stdint.h> // ���������� ��� ������������� int32_t
#include <ctype.h>  // ��� ������� tolower

#define MAX_X 15
#define MAX_Y 15
#define RIGHT 1
#define LEFT -1
#define UP 2
#define DOWN -2
#define CONTROLS 4 // ���������� ���������� ��������� ����������

typedef struct tail_t {
    int x;
    int y;
} tail_t;

typedef struct snake_t {
    int x;
    int y;
    struct tail_t* tail;
    size_t tsize;
    int direction; // ����� �������� ������� ����������� ������
} snake_t;

typedef struct control_button {
    char key; // �������
    int direction; // �����������
} control_button;

// ��������� ������� checkDirection �� �� ������������� � processInput
int checkDirection(snake_t* snake, int32_t key);

// ����������� ������
control_button default_controls[CONTROLS] = {
    {'w', UP},
    {'s', DOWN},
    {'a', LEFT},
    {'d', RIGHT}
};

// ������������� ������
snake_t initSnake(int x, int y, size_t tsize) {
    snake_t snake;
    snake.x = x;
    snake.y = y;
    snake.tsize = tsize;
    snake.direction = RIGHT; // ��������� �����������
    snake.tail = (tail_t*)malloc(sizeof(tail_t) * 100);
    for (int i = 0; i < tsize; ++i) {
        snake.tail[i].x = x + i + 1;
        snake.tail[i].y = y;
    }
    return snake;
}

// ������ ������
void printSnake(snake_t snake) {
    char matrix[MAX_X][MAX_Y];
    for (int i = 0; i < MAX_X; ++i) {
        for (int j = 0; j < MAX_Y; ++j) {
            matrix[i][j] = ' ';
        }
    }

    matrix[snake.x][snake.y] = '@';
    for (int i = 0; i < snake.tsize; ++i) {
        matrix[snake.tail[i].x][snake.tail[i].y] = '~';
    }

    for (int j = 0; j < MAX_Y; ++j) {
        for (int i = 0; i < MAX_X; ++i) {
            printf("%c", matrix[i][j]);
        }
        printf("\n");
    }
}

// �������� ������������ �����������
int checkDirection(snake_t* snake, int32_t key) {
    switch (snake->direction) {
        case RIGHT:
            if (key == LEFT) {
                return 0; // �����������
            }
            break;
        case LEFT:
            if (key == RIGHT) {
                return 0; // �����������
            }
            break;
        case UP:
            if (key == DOWN) {
                return 0; // �����������
            }
            break;
        case DOWN:
            if (key == UP) {
                return 0; // �����������
            }
            break;
        default:
            return 1; // ���������, ���� ����������� �� ������
    }
    return 1; // ���������
}

// �������� ������ �����
snake_t moveLeft(snake_t snake) {
    for (int i = snake.tsize - 1; i > 0; i--) {
        snake.tail[i] = snake.tail[i - 1];
    }
    snake.tail[0].x = snake.x;
    snake.tail[0].y = snake.y;

    snake.x = snake.x - 1;
    if (snake.x < 0) {
        snake.x = MAX_X - 1;
    }
    return snake;
}

// �������� ������ ������
snake_t moveRight(snake_t snake) {
    for (int i = snake.tsize - 1; i > 0; i--) {
        snake.tail[i] = snake.tail[i - 1];
    }
    snake.tail[0].x = snake.x;
    snake.tail[0].y = snake.y;

    snake.x = snake.x + 1;
    if (snake.x >= MAX_X) {
        snake.x = 0;
    }
    return snake;
}

// �������� ������ �����
snake_t moveUp(snake_t snake) {
    for (int i = snake.tsize - 1; i > 0; i--) {
        snake.tail[i] = snake.tail[i - 1];
    }

    snake.tail[0].x = snake.x;
    snake.tail[0].y = snake.y;

    snake.y = snake.y - 1;
    if (snake.y < 0) {
        snake.y = MAX_Y - 1;
    }
    return snake;
}

// �������� ������ ����
snake_t moveDown(snake_t snake) {
    for (int i = snake.tsize - 1; i > 0; i--) {
        snake.tail[i] = snake.tail[i - 1];
    }
    snake.tail[0].x = snake.x;
    snake.tail[0].y = snake.y;

    snake.y = snake.y + 1;
    if (snake.y >= MAX_Y) {
        snake.y = 0;
    }
    return snake;
}

// ��������� �����
void processInput(snake_t* snake) {
    if (_kbhit()) {
        int key = _getch();
        key = tolower(key); // �������� � ������� ��������
        int newDirection = 0;

        // ��������� ��� ����������� ������
        for (int i = 0; i < CONTROLS; i++) {
            if (key == default_controls[i].key) {
                newDirection = default_controls[i].direction;
                break;
            }
        }

        if (checkDirection(snake, newDirection)) {
            snake->direction = newDirection; // �������� �����������
        }
    }
}

// �������� �������
int main() {
    snake_t snake = initSnake(10, 5, 2);
    printSnake(snake);
    while (1) {
        processInput(&snake); // ��������� �����
        switch (snake.direction) { // �������� � ����������� �� �������� �����������
            case LEFT:
                snake = moveLeft(snake);
                break;
            case RIGHT:
                snake = moveRight(snake);
                break;
            case UP:
                snake = moveUp(snake);
                break;
            case DOWN:
                snake = moveDown(snake);
                break;
        }
        sleep(1);
        system("cls");
        printSnake(snake);
    }
    free(snake.tail);
    return 0;
}

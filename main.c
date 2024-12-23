#define _CRT_SECURE_NO_WARNINGS
#include <conio.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "display.h"

#define MAX_OBSTACLES 2
#define SPACE ' '

#define COLOR_GREEN (FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define COLOR_RED (FOREGROUND_RED | FOREGROUND_INTENSITY)
#define COLOR_YELLOW (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY)
#define COLOR_WHITE (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)

typedef struct {
    int x, y; // ��ֹ��� ��ġ
    int type; // 1: 1�� ��ֹ�, 2: 2�� ��ֹ�
} Obstacle;

int score = 0;
char grade[3] = "F";

char frontBuffer[HEIGHT][WIDTH + 1];
char backBuffer[HEIGHT][WIDTH + 1];

void setColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void clearBuffer(char buffer[HEIGHT][WIDTH + 1]) {
    // ���� �ʱ�ȭ (�������� ä��)
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            buffer[y][x] = ' ';
        }
        buffer[y][WIDTH] = '\0';  // ���ڿ� ���� null ���� �߰�
    }
}

void swapBuffers() {
    memcpy(frontBuffer, backBuffer, sizeof(frontBuffer));
}

void render() {
    COORD coord = { 0, 0 };
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hConsole, coord);

    for (int y = 0; y < HEIGHT; y++) {
        // ���ھ�� ����� ù ��° ��, �����
        if (y == 0) {
            setColor(COLOR_YELLOW);
        }
        // �ٴ��� ������ ��, �ʷϻ�
        else if (y == HEIGHT - 1) {
            setColor(COLOR_GREEN);
        }
        // ������: �÷��̾�� ��ֹ�(���)
        else {
            setColor(COLOR_WHITE);
        }
        printf("%s\n", frontBuffer[y]);
    }
}

void updateGrade() {
    if (score >= 200) strcpy(grade, "A+");
    else if (score >= 150) strcpy(grade, "A");
    else if (score >= 120) strcpy(grade, "B+");
    else if (score >= 100) strcpy(grade, "B");
    else if (score >= 60) strcpy(grade, "C+");
    else if (score >= 50) strcpy(grade, "C");
    else if (score >= 40) strcpy(grade, "D+");
    else if (score >= 30) strcpy(grade, "D");
    else strcpy(grade, "F");
}

void drawGame(int playerY, Obstacle obstacles[], int obstacleCount) {
    clearBuffer(backBuffer);

    // ���ھ�� ��� ǥ��
    setColor(COLOR_YELLOW);
    snprintf(backBuffer[0], WIDTH, "SCORE: %d  GRADE: %s", score, grade);

    // �÷��̾� ��ġ �׸���
    setColor(COLOR_WHITE);
    if (playerY >= 0 && playerY < HEIGHT) {
        backBuffer[playerY][1] = '&'; // �÷��̾� ǥ��
    }

    // ��ֹ� ��ġ �׸���
    setColor(COLOR_WHITE);
    for (int i = 0; i < obstacleCount; i++) {
        if (obstacles[i].y >= 0 && obstacles[i].y < HEIGHT && obstacles[i].x >= 0 && obstacles[i].x < WIDTH) {
            backBuffer[obstacles[i].y][obstacles[i].x] = '��';
        }
    }

    // �ٴ� �׸���
    setColor(COLOR_GREEN);
    for (int x = 0; x < WIDTH; x++) {
        backBuffer[HEIGHT - 1][x] = '_';
    }

    swapBuffers();
    setColor(COLOR_WHITE); // ��� �� �⺻ ���� ����
}

int checkCollision(int playerY, Obstacle obstacles[], int obstacleCount) {
    for (int i = 0; i < obstacleCount; i++) {
        if (obstacles[i].y == playerY && obstacles[i].x == 1) {
            return 1; // �浹 �߻�
        }
    }
    return 0; // �浹 ����
}

void updateObstacles(Obstacle obstacles[], int obstacleCount, int playerY) {
    for (int i = 0; i < obstacleCount; i++) {
        obstacles[i].x--; // ��ֹ� �������� �̵�

        // ��ֹ��� ȭ�� ������ ������ �� ���� �߰�
        if (obstacles[i].x < 0) {
            if (obstacles[i].y != playerY) { // �÷��̾�� �浹���� �ʾ��� ��
                score += (obstacleCount == 1) ? 10 : 15; // ���� �߰�
                updateGrade(); // ��� ������Ʈ
            }

            // ��ֹ� ���ġ
            obstacles[i].x = WIDTH - 2; // ��ֹ� ��ġ �ʱ�ȭ
            obstacles[i].y = HEIGHT - 2; // �ٴ� �ٷ� ��
        }
    }
}

void startGame() {
    int cursorX = START_X; // �ʱ� Ŀ�� X ��ġ
    int cursorY = START_Y; // �ʱ� Ŀ�� Y ��ġ

    while (1) {
        drawStartScreen(cursorX, cursorY);

        // Ű �Է� ó��
        char command = _getch();
        if (command == 'w' && cursorY > 0) cursorY--;           // �������� �̵�
        else if (command == 's' && cursorY < HEIGHT + 1) cursorY++; // �Ʒ������� �̵�
        else if (command == 'a' && cursorX > 0) cursorX--;       // �������� �̵�
        else if (command == 'd' && cursorX < WIDTH - 1) cursorX++; // ���������� �̵�
        else if (command == '\r') {
            if (cursorX == START_X && cursorY == START_Y) {
                break; // Ŀ���� "���� ����"�� ���������� ������ ����
            }
            else if (cursorX == START_X && cursorY == START_Y) { // "ȯ�� ����" ����
                drawSettingsScreen(); // ���� ȭ�� ȣ��
                _getch(); // ���� ȭ�鿡�� �ƹ� Ű�� ������ ���ƿ��� ��
            }
        }
    }

    // ���� ȭ�� �ʱ�ȭ
    system("cls");
    clearBuffer(frontBuffer);
    clearBuffer(backBuffer);
}

void handleJump(int* playerY, Obstacle obstacles[], int obstacleCount) {

    *playerY -= 1; // �÷��̾ ����
    for (int t = 0; t < 10; t++) { // 1�� ���� 10 ������ ó��
        updateObstacles(obstacles, obstacleCount, *playerY); // ��ֹ� �̵�
        drawGame(*playerY, obstacles, obstacleCount); // ȭ�� ���� ������Ʈ
        render(); // ȭ�� ���
        Sleep(100); // ������ ��� (100ms * 10 = 1��)
    }

    *playerY += 1; // �÷��̾ ������
    for (int t = 0; t < 10; t++) { // �������� ���� 10 ������ ó��
        updateObstacles(obstacles, obstacleCount, *playerY); // ��ֹ� �̵�
        drawGame(*playerY, obstacles, obstacleCount); // ȭ�� ���� ������Ʈ
        render(); // ȭ�� ���
        Sleep(100); // ������ ���
    }
}

int main() {
    srand(time(NULL));
    startGame(); // ���� ���� ȭ�� ȣ��

    int playerY = HEIGHT - 2; // �÷��̾� �ʱ� ��ġ (�ٴ� �ٷ� ��)
    int gameRunning = 1;

    while (gameRunning) {
        int obstacleCount = (rand() % MAX_OBSTACLES) + 1; // 1�� �Ǵ� 2�� ��ֹ�
        Obstacle obstacles[MAX_OBSTACLES];
        for (int i = 0; i < obstacleCount; i++) {
            obstacles[i].x = WIDTH - (i + 2) * 5; // ��ֹ� ��ġ (���� ����)
            obstacles[i].y = HEIGHT - 2;         // ��ֹ��� �ٴ� �ٷ� ��
        }

        while (gameRunning) {
            drawGame(playerY, obstacles, obstacleCount);
            updateObstacles(obstacles, obstacleCount, playerY);

            // ��ֹ� ȸ�� ���� Ȯ��
            for (int i = 0; i < obstacleCount; i++) {
                if (obstacles[i].x == 1 && playerY != obstacles[i].y) {
                    score += (obstacleCount == 1) ? 10 : 15; // ���� �߰�
                    updateGrade();
                }
            }

            // �浹 ����
            if (checkCollision(playerY, obstacles, obstacleCount)) {
                printf("Game Over! Your score: %d\n", score);
                gameRunning = 0; // ���� ����
                break;
            }

            // Ű �Է� ó��
            if (_kbhit()) {
                char command = _getch();
                if (command == SPACE) {
                    handleJump(&playerY, obstacles, obstacleCount); // ���� ó��
                }
            }

            render();
            Sleep(80); // ���� ���� �ӵ� ����
        }
        Sleep(2000); // ��ֹ� ���� �ֱ�
    }
    return 0;
}



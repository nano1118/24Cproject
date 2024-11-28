#include <conio.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "display.h"

#define MAX_OBSTACLES 2
#define SPACE ' '

typedef struct {
    int x, y; // ��ֹ��� ��ġ
    int type; // 1: 1�� ��ֹ�, 2: 2�� ��ֹ�
} Obstacle;

int score = 0;
char grade = 'F';

char buffer[HEIGHT][WIDTH + 1];

void clearBuffer() {
    // ���� �ʱ�ȭ (�������� ä��)
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            buffer[y][x] = ' ';
        }
        buffer[y][WIDTH] = '\0';  // ���ڿ� ���� null ���� �߰�
    }
}

void render() {
    system("cls");  // ȭ�� �����
    for (int y = 0; y < HEIGHT; y++) {
        printf("%s\n", buffer[y]);
    }
}

void updateGrade() {
    if (score >= 200) grade = 'A+';
    else if (score >= 150) grade = 'A';
    else if (score >= 120) grade = 'B+';
    else if (score >= 100) grade = 'B';
    else if (score >= 60) grade = 'C+';
    else if (score >= 50) grade = 'C';
    else if (score >= 40) grade = 'D+';
    else if (score >= 30) grade = 'D';
}

void drawGame(int playerY, Obstacle obstacles[], int obstacleCount) {
    clearBuffer();
    snprintf(buffer[0], WIDTH, "Grade: %c", grade);

    // �÷��̾� ��ġ �׸���
    buffer[playerY][1] = '&';

    // ��ֹ� ��ġ �׸���
    for (int i = 0; i < obstacleCount; i++) {
        buffer[obstacles[i].y][obstacles[i].x] = '��';
    }
}

int checkCollision(int playerY, Obstacle obstacles[], int obstacleCount) {
    for (int i = 0; i < obstacleCount; i++) {
        if (obstacles[i].y == playerY && obstacles[i].x == 1) {
            return 1; // �浹 �߻�
        }
    }
    return 0; // �浹 ����
}

void updateObstacles(Obstacle obstacles[], int obstacleCount) {
    for (int i = 0; i < obstacleCount; i++) {
        obstacles[i].x--; // ��ֹ� �������� �̵�
        // ��ֹ��� ȭ�� ������ ������ ���ġ
        if (obstacles[i].x < 0) {
            obstacles[i].x = WIDTH - 2; // ��ֹ� ���ġ
            obstacles[i].y = HEIGHT - 1; // �ٴڿ� ��ġ
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
}

int main() {
    srand(time(NULL));
    startGame(); // ���� ���� ȭ�� ȣ��

    int playerY = HEIGHT - 1; // �÷��̾� �ʱ� ��ġ (�ٴ�)
    int gameRunning = 1;

    while (gameRunning) {
        int obstacleCount = (rand() % MAX_OBSTACLES) + 1; // 1�� �Ǵ� 2�� ��ֹ�
        Obstacle obstacles[MAX_OBSTACLES];
        for (int i = 0; i < obstacleCount; i++) {
            obstacles[i].x = WIDTH - 2; // ��ֹ� ��ġ
            obstacles[i].y = HEIGHT - 1; // ��ֹ��� �ٴڿ� ��ġ
        }
        while (gameRunning) {
            drawGame(playerY, obstacles, obstacleCount);
            updateObstacles(obstacles, obstacleCount);

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
                    playerY--; // �����Ͽ� �� ĭ ���� �ö�
                    Sleep(1000); // 1�� ���� ���� ���� ����
                    playerY++; // �ٽ� ������
                    score += (obstacleCount == 1) ? 10 : 15; // ���� �߰�
                    updateGrade();
                }
            }
            render();
            Sleep(80); // ���� ���� �ӵ� ����
        }
        Sleep(2000); // ��ֹ� ���� �ֱ�
    }
    return 0;
}


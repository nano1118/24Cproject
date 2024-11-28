#include <conio.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "display.h"

#define MAX_OBSTACLES 2
#define SPACE ' '

typedef struct {
    int x, y; // 장애물의 위치
    int type; // 1: 1개 장애물, 2: 2개 장애물
} Obstacle;

int score = 0;
char grade = 'F';

char buffer[HEIGHT][WIDTH + 1];

void clearBuffer() {
    // 버퍼 초기화 (공백으로 채움)
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            buffer[y][x] = ' ';
        }
        buffer[y][WIDTH] = '\0';  // 문자열 끝에 null 문자 추가
    }
}

void render() {
    system("cls");  // 화면 지우기
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

    // 플레이어 위치 그리기
    buffer[playerY][1] = '&';

    // 장애물 위치 그리기
    for (int i = 0; i < obstacleCount; i++) {
        buffer[obstacles[i].y][obstacles[i].x] = '■';
    }
}

int checkCollision(int playerY, Obstacle obstacles[], int obstacleCount) {
    for (int i = 0; i < obstacleCount; i++) {
        if (obstacles[i].y == playerY && obstacles[i].x == 1) {
            return 1; // 충돌 발생
        }
    }
    return 0; // 충돌 없음
}

void updateObstacles(Obstacle obstacles[], int obstacleCount) {
    for (int i = 0; i < obstacleCount; i++) {
        obstacles[i].x--; // 장애물 왼쪽으로 이동
        // 장애물이 화면 밖으로 나가면 재배치
        if (obstacles[i].x < 0) {
            obstacles[i].x = WIDTH - 2; // 장애물 재배치
            obstacles[i].y = HEIGHT - 1; // 바닥에 위치
        }
    }
}

void startGame() {
    int cursorX = START_X; // 초기 커서 X 위치
    int cursorY = START_Y; // 초기 커서 Y 위치

    while (1) {
        drawStartScreen(cursorX, cursorY);

        // 키 입력 처리
        char command = _getch();
        if (command == 'w' && cursorY > 0) cursorY--;           // 위쪽으로 이동
        else if (command == 's' && cursorY < HEIGHT + 1) cursorY++; // 아래쪽으로 이동
        else if (command == 'a' && cursorX > 0) cursorX--;       // 왼쪽으로 이동
        else if (command == 'd' && cursorX < WIDTH - 1) cursorX++; // 오른쪽으로 이동
        else if (command == '\r') {
            if (cursorX == START_X && cursorY == START_Y) {
                break; // 커서가 "게임 시작"에 도달했으면 루프를 종료
            }
            else if (cursorX == START_X && cursorY == START_Y) { // "환경 설정" 선택
                drawSettingsScreen(); // 설정 화면 호출
                _getch(); // 설정 화면에서 아무 키나 누르면 돌아오게 함
            }
        }
    }
}

int main() {
    srand(time(NULL));
    startGame(); // 게임 시작 화면 호출

    int playerY = HEIGHT - 1; // 플레이어 초기 위치 (바닥)
    int gameRunning = 1;

    while (gameRunning) {
        int obstacleCount = (rand() % MAX_OBSTACLES) + 1; // 1개 또는 2개 장애물
        Obstacle obstacles[MAX_OBSTACLES];
        for (int i = 0; i < obstacleCount; i++) {
            obstacles[i].x = WIDTH - 2; // 장애물 위치
            obstacles[i].y = HEIGHT - 1; // 장애물은 바닥에 위치
        }
        while (gameRunning) {
            drawGame(playerY, obstacles, obstacleCount);
            updateObstacles(obstacles, obstacleCount);

            // 충돌 감지
            if (checkCollision(playerY, obstacles, obstacleCount)) {
                printf("Game Over! Your score: %d\n", score);
                gameRunning = 0; // 게임 종료
                break;
            }
            // 키 입력 처리
            if (_kbhit()) {
                char command = _getch();
                if (command == SPACE) {
                    playerY--; // 점프하여 한 칸 위로 올라감
                    Sleep(1000); // 1초 동안 점프 상태 유지
                    playerY++; // 다시 내려옴
                    score += (obstacleCount == 1) ? 10 : 15; // 점수 추가
                    updateGrade();
                }
            }
            render();
            Sleep(80); // 게임 루프 속도 조절
        }
        Sleep(2000); // 장애물 생성 주기
    }
    return 0;
}


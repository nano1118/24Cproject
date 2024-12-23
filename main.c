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
    int x, y; // 장애물의 위치
    int type; // 1: 1개 장애물, 2: 2개 장애물
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
    // 버퍼 초기화 (공백으로 채움)
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            buffer[y][x] = ' ';
        }
        buffer[y][WIDTH] = '\0';  // 문자열 끝에 null 문자 추가
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
        // 스코어와 등급은 첫 번째 줄, 노란색
        if (y == 0) {
            setColor(COLOR_YELLOW);
        }
        // 바닥은 마지막 줄, 초록색
        else if (y == HEIGHT - 1) {
            setColor(COLOR_GREEN);
        }
        // 나머지: 플레이어와 장애물(흰색)
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

    // 스코어와 등급 표시
    setColor(COLOR_YELLOW);
    snprintf(backBuffer[0], WIDTH, "SCORE: %d  GRADE: %s", score, grade);

    // 플레이어 위치 그리기
    setColor(COLOR_WHITE);
    if (playerY >= 0 && playerY < HEIGHT) {
        backBuffer[playerY][1] = '&'; // 플레이어 표시
    }

    // 장애물 위치 그리기
    setColor(COLOR_WHITE);
    for (int i = 0; i < obstacleCount; i++) {
        if (obstacles[i].y >= 0 && obstacles[i].y < HEIGHT && obstacles[i].x >= 0 && obstacles[i].x < WIDTH) {
            backBuffer[obstacles[i].y][obstacles[i].x] = '■';
        }
    }

    // 바닥 그리기
    setColor(COLOR_GREEN);
    for (int x = 0; x < WIDTH; x++) {
        backBuffer[HEIGHT - 1][x] = '_';
    }

    swapBuffers();
    setColor(COLOR_WHITE); // 출력 후 기본 색상 복원
}

int checkCollision(int playerY, Obstacle obstacles[], int obstacleCount) {
    for (int i = 0; i < obstacleCount; i++) {
        if (obstacles[i].y == playerY && obstacles[i].x == 1) {
            return 1; // 충돌 발생
        }
    }
    return 0; // 충돌 없음
}

void updateObstacles(Obstacle obstacles[], int obstacleCount, int playerY) {
    for (int i = 0; i < obstacleCount; i++) {
        obstacles[i].x--; // 장애물 왼쪽으로 이동

        // 장애물이 화면 밖으로 나갔을 때 점수 추가
        if (obstacles[i].x < 0) {
            if (obstacles[i].y != playerY) { // 플레이어와 충돌하지 않았을 때
                score += (obstacleCount == 1) ? 10 : 15; // 점수 추가
                updateGrade(); // 등급 업데이트
            }

            // 장애물 재배치
            obstacles[i].x = WIDTH - 2; // 장애물 위치 초기화
            obstacles[i].y = HEIGHT - 2; // 바닥 바로 위
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

    // 게임 화면 초기화
    system("cls");
    clearBuffer(frontBuffer);
    clearBuffer(backBuffer);
}

void handleJump(int* playerY, Obstacle obstacles[], int obstacleCount) {

    *playerY -= 1; // 플레이어가 점프
    for (int t = 0; t < 10; t++) { // 1초 동안 10 프레임 처리
        updateObstacles(obstacles, obstacleCount, *playerY); // 장애물 이동
        drawGame(*playerY, obstacles, obstacleCount); // 화면 상태 업데이트
        render(); // 화면 출력
        Sleep(100); // 프레임 대기 (100ms * 10 = 1초)
    }

    *playerY += 1; // 플레이어가 내려옴
    for (int t = 0; t < 10; t++) { // 내려오는 동안 10 프레임 처리
        updateObstacles(obstacles, obstacleCount, *playerY); // 장애물 이동
        drawGame(*playerY, obstacles, obstacleCount); // 화면 상태 업데이트
        render(); // 화면 출력
        Sleep(100); // 프레임 대기
    }
}

int main() {
    srand(time(NULL));
    startGame(); // 게임 시작 화면 호출

    int playerY = HEIGHT - 2; // 플레이어 초기 위치 (바닥 바로 위)
    int gameRunning = 1;

    while (gameRunning) {
        int obstacleCount = (rand() % MAX_OBSTACLES) + 1; // 1개 또는 2개 장애물
        Obstacle obstacles[MAX_OBSTACLES];
        for (int i = 0; i < obstacleCount; i++) {
            obstacles[i].x = WIDTH - (i + 2) * 5; // 장애물 위치 (간격 포함)
            obstacles[i].y = HEIGHT - 2;         // 장애물은 바닥 바로 위
        }

        while (gameRunning) {
            drawGame(playerY, obstacles, obstacleCount);
            updateObstacles(obstacles, obstacleCount, playerY);

            // 장애물 회피 여부 확인
            for (int i = 0; i < obstacleCount; i++) {
                if (obstacles[i].x == 1 && playerY != obstacles[i].y) {
                    score += (obstacleCount == 1) ? 10 : 15; // 점수 추가
                    updateGrade();
                }
            }

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
                    handleJump(&playerY, obstacles, obstacleCount); // 점프 처리
                }
            }

            render();
            Sleep(80); // 게임 루프 속도 조절
        }
        Sleep(2000); // 장애물 생성 주기
    }
    return 0;
}



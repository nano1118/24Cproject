#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "display.h"

void clearScreen()
{
    system("cls"); // Windows에서 화면 지우기
}

void setTextColor(int color)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void drawStartScreen(int cursorX, int cursorY)
{
    clearScreen();

    int titleX = (WIDTH / 2) - 1;
    printf("\n\n\n");

    // 게임 영역 그리기
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            if (y == 1 && x == titleX)
            {
                printf("A+런");
                x += 2; // "A+런"이 차지하는 너비만큼 이동
            }
            else if (y == START_Y && x == START_X-13)
            {
                // 커서 위치에 @ 표시
                if (cursorX == START_X)
                {
                    printf("@ ");
                    setTextColor(12); // 빨간색
                    printf("게임 시작");
                    setTextColor(7); // 기본 색상으로 복원
                }
                printf("     "); // 두 옵션 간 간격 추가
                // 커서가 "환경 설정"에 있을 때
                if (cursorX > START_X)
                {
                    printf("@ ");
                    setTextColor(12); // 빨간색
                    printf("환경 설정");
                    setTextColor(7); // 기본 색상으로 복원
                }
                else
                {
                    printf("  환경 설정");
                }

                x += 16; // "게임 시작"과 "환경 설정"의 너비만큼 이동
            }
            else
            {
                printf(" ");
            }
        }
        printf("\n");
    }
}

void drawSettingsScreen()
{
    clearScreen();
    printf("\n\n환경 설정 화면\n");
    printf("1. Option 1\n");
    printf("2. Option 2\n");
    printf("3. Back\n");
}

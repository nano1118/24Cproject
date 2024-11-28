#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "display.h"

void clearScreen()
{
    system("cls"); // Windows���� ȭ�� �����
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

    // ���� ���� �׸���
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            if (y == 1 && x == titleX)
            {
                printf("A+��");
                x += 2; // "A+��"�� �����ϴ� �ʺ�ŭ �̵�
            }
            else if (y == START_Y && x == START_X-13)
            {
                // Ŀ�� ��ġ�� @ ǥ��
                if (cursorX == START_X)
                {
                    printf("@ ");
                    setTextColor(12); // ������
                    printf("���� ����");
                    setTextColor(7); // �⺻ �������� ����
                }
                printf("     "); // �� �ɼ� �� ���� �߰�
                // Ŀ���� "ȯ�� ����"�� ���� ��
                if (cursorX > START_X)
                {
                    printf("@ ");
                    setTextColor(12); // ������
                    printf("ȯ�� ����");
                    setTextColor(7); // �⺻ �������� ����
                }
                else
                {
                    printf("  ȯ�� ����");
                }

                x += 16; // "���� ����"�� "ȯ�� ����"�� �ʺ�ŭ �̵�
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
    printf("\n\nȯ�� ���� ȭ��\n");
    printf("1. Option 1\n");
    printf("2. Option 2\n");
    printf("3. Back\n");
}

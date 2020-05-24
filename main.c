#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include "game.h"

#define SIZE 4

// Global data
int state[SIZE][SIZE];
int currentScore;
int scoreRecord;

int saveRecord();
int readRecord();

int encodeRecord();
int decodeRecord();

void updateScreen();

void rotateBoardClockwise(int board[SIZE][SIZE], int times);
int slideLeft(int board[SIZE][SIZE]);
int slideRight(int board[SIZE][SIZE]);
int slideUp(int board[SIZE][SIZE]);
int slideDown(int board[SIZE][SIZE]);

BOOL GameInit(OUT PENV oldEnv)
{
    // hide cursor
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &oldEnv->cursorInfo);
    CONSOLE_CURSOR_INFO info = {1, 0}; // set cursor invisible
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);

    // TODO: add "read score record from file" function
    // Read record from file, and maybe decrypt it before read record
    scoreRecord = 100; // assuming the highest score record
    currentScore = 0;

    return SetConsoleTitle(L"进击的2048");
}

BOOL ConsoleRecovery(IN PENV oldEnv)
{
    // recover the environment
    return SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &oldEnv->cursorInfo);
}

BOOL StartGame()
{
    BOOL willExit = FALSE;
    int key, key_extra;
    while (!willExit)
    {
        key = getch();
        switch (key)
        {
        case 'w':
        case 'W':
            slideUp();
        case 's':
        case 'S':
            slideDown();
        case 'A':
        case 'a':
            slideLeft();
        case 'D':
        case 'd':
            slideRight();
        case 0xE0:
            switch (key_extra = getch())
            {
            case 72:
                printf("UP\n");
                break;
            case 80:
                printf("DOWN\n");
                break;
            case 75:
                printf("LEFT\n");
                break;
            case 77:
                printf("RIGHT\n");
                break;
            default:
                break;
            }
        default:
            break;
        }
    }
}

void rotateBoardClockwise(int board[SIZE][SIZE], int times)
{
    // rotate square matrix, (not only 4 x 4 square)
    if (0 == times || 4 == times)
    {
        return;
    }
    while (times--)
    {
        int step = 0;
        int *tempArr = (int *)malloc(sizeof(int) * (SIZE - step - 1));
        while (step < (SIZE / 2))
        {
            int i;
            int j = 0;
            for (i = SIZE - 1 - step; i > step; i--)
                tempArr[j++] = board[i][SIZE - 1 - step];

            for (i = SIZE - 1 - step; i > step; i--)
                board[i][SIZE - 1 - step] = board[step][i];
            for (i = SIZE - 1 - step; i > step; i--)
                board[step][i] = board[SIZE - 1 - i][step];
            for (i = step; i < SIZE - 1 - step; i++)
                board[i][step] = board[SIZE - 1 - step][i];

            j = 0;
            for (i = step; i < SIZE - 1 - step; i++)
            {
                board[SIZE - 1 - step][i] = tempArr[j++];
            }
            step++;
        }
    }
}

int slideUp(int board[SIZE][SIZE])
{
    int *nonZeroNums = (int *)malloc(sizeof(int) * SIZE);
    int numNotZero;
    int i;
    int val;
    for (int row, col = 0; col < SIZE; col++)
    {
        row = 0;
        numNotZero = 0;
        while (row < SIZE)
        {
            if (0 == board[row++][col])
            {
                continue;
            }
            // find 1st non-zero number
            nonZeroNums[numNotZero++] = board[row - 1][col];

            while (row < SIZE)
            {
                if (0 == (val = board[row++][col]))
                {
                    continue;
                }
                // find 2nd non-zero number
                if (board[row - 1][col] == nonZeroNums[numNotZero - 1])
                {
                    nonZeroNums[numNotZero - 1] <<= 1;
                    break;
                }
                nonZeroNums[numNotZero++] = val;
                // row;
            }
        }

        // put all non-zero values in a column of the board
        for (i = 0; i < numNotZero; i++)
        {
            board[i][col] = nonZeroNums[i];
        }

        for (; i < SIZE; i++)
        {
            board[i][col] = 0;
        }
    }
    free(nonZeroNums);
}

int selectionIn(const char **options, int numOfOptions)
{
    int selection = 0;
    int key;
    int key_extra;
    const char *prefix = "*";
    do
    {
        system('cls');
        for (int i; i < numOfOptions; i++)
        {
            printf("\n");
            printf("\t%s %s", i == selection ? prefix : " ", options[i]);
        }
        key = getch();
        switch (key)
        {
        case 'w':
        case 'W':
            selection = (numOfOptions + selection - 1) % numOfOptions;
            continue;
        case 's':
        case 'S':
            selection = (selection + 1) % numOfOptions;
            continue;
        case 0xE0:
            // ↑, ↓ keys generates a key and a key_extra
            switch (key_extra = getch())
            {
            case 72:
                selection = (numOfOptions + selection - 1) % numOfOptions;
                break;
            case 80:
                selection = (selection + 1) % numOfOptions;
                break;
            default:
                break;
            }
            continue;
        default:
            continue;
        }
    } while (key != VK_RETURN); // press enter to confirm selection
    return selection;
}

BOOL mainMenu()
{
    BOOL willExit = FALSE;
    const char *options[] = {"开始游戏", "查看排名", "退出游戏"};
    switch (selectionIn(options, 3))
    {
    case 0:
        willExit = gameStart();
        saveRecord();
    case 1:
        lookUpRank();
    case 2:
        willExit = TRUE;
    }
    return willExit;
}

int main()
{
    PENV oldEnv = (PENV)malloc(sizeof(ENV));
    int **board;
    if (!GameInit(board, oldEnv))
    {
        return EXIT_FAILURE;
    }

    BOOL willExit;
    do
    {
        willExit = mainMenu();
    } while (!willExit);

    return ConsoleRecovery(oldEnv);
}
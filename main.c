#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <conio.h>
#include "game.h"

// NOTE: maybe it's better to set "PMATRIX pBoard" as global variable;
#define SIZE 4

typedef int ** MATRIX;
typedef MATRIX*  PMATRIX;


// Global data
int currentScore;
int scoreRecord;
int restZeros;

int saveRecord();
int readRecord();

int encodeRecord();
int decodeRecord();


void rotateBoardClockwise(PMATRIX pBoard, int times);
int slideLeft(PMATRIX pBoard);
int slideRight(PMATRIX pBoard);
int slideUp(PMATRIX pBoard);
int slideDown(PMATRIX pBoard);
void addNewValToBoard(PMATRIX pBoard);
void updateScreen(PMATRIX pBoard);
BOOL GameInit(OUT PMATRIX *pBoard, OUT PENV *pOldEnv);
BOOL StartGame(PMATRIX pBoard);

void lookUpRank();

void rotateBoardClockwise(PMATRIX pBoard, int times)
{
    MATRIX board = *pBoard;
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

int slideUp(PMATRIX pBoard)
{
    MATRIX board = *pBoard;
    int *columnNonZeroNums = (int *)malloc(sizeof(int) * SIZE);
    int columnNumNotZero;
    restZeros = 0;
    int i;
    int val;
    for (int row, col = 0; col < SIZE; col++)
    {
        row = 0;
        columnNumNotZero = 0;
        while (row < SIZE)
        {
            if (0 == board[row++][col])
            {
                continue;
            }
            // 1st non-zero number fpund
            columnNonZeroNums[columnNumNotZero++] = board[row - 1][col];

            // having made sure that nonZeroNums is not empty
            // so the code below can handle all 3 cases:
            // "0, equal, not equal"
            while (row < SIZE)
            {
                if (0 == (val = board[row++][col]))
                {
                    continue;
                }
                if (board[row - 1][col] == columnNonZeroNums[columnNumNotZero - 1])
                {
                    columnNonZeroNums[columnNumNotZero - 1] <<= 1;
                    break;
                }
                columnNonZeroNums[columnNumNotZero++] = val;
            }
        }

        // TODO: separate out a new function
        // put all non-zero values in a column of the board
        for (i = 0; i < columnNumNotZero; i++)
        {
            board[i][col] = columnNonZeroNums[i];
        }

        for (; i < SIZE; i++)
        {
            board[i][col] = 0;
        }

        restZeros += SIZE - columnNumNotZero;
    }
    free(columnNonZeroNums);
    return restZeros;
}

int slideDown(PMATRIX pBoard)
{
    rotateBoardClockwise(pBoard, 2);
    slideUp(pBoard);
    rotateBoardClockwise(pBoard, 2);
    return restZeros;
}

int slideLeft(PMATRIX pBoard)
{
    rotateBoardClockwise(pBoard, 1);
    slideUp(pBoard);
    rotateBoardClockwise(pBoard, 3);
    return restZeros;
}

int slideRight(PMATRIX pBoard)
{
    rotateBoardClockwise(pBoard, 3);
    slideUp(pBoard);
    rotateBoardClockwise(pBoard, 1);
    return restZeros;
}

void addNewValToBoard(PMATRIX pBoard)
{
    MATRIX board = *pBoard;
    int pos = rand() % restZeros;
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            if (!board[i][j])
            {
                pos--;
                if (!pos)
                {
                    int times = rand() % 2 ? 1 : 2;
                    board[i][j] = 2 * times;
                    break;
                }
            }
        }
        if (!pos)
        {
            break;
        }
    }
}

int selectionIn(const char **options, int numOfOptions)
{
    int selection = 0;
    int key;
    int key_extra;
    const char *prefix = "*";
    do
    {
        system("cls");
        for (int i = 0; i < numOfOptions; i++)
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

BOOL StartGame(PMATRIX pBoard)
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
            slideUp(pBoard);
            break;
        case 's':
        case 'S':
            slideDown(pBoard);
            break;
        case 'A':
        case 'a':
            slideLeft(pBoard);
            break;
        case 'D':
        case 'd':
            slideRight(pBoard);
            break;
        case 0xE0:
            switch (key_extra = getch())
            {
            case 72:
                slideUp(pBoard);
                break;
            case 80:
                slideDown(pBoard);
                break;
            case 75:
                slideLeft(pBoard);
                break;
            case 77:
                slideRight(pBoard);
                break;
            default:
                break;
            }
        default:
            break;
        }
        updateScreen(pBoard);
        addNewValToBoard(pBoard);
        updateScreen(pBoard);
    }
}

BOOL mainMenu(PMATRIX pBoard)
{
    BOOL willExit = FALSE;
    const char *options[] = {"开始游戏", "查看排名", "退出游戏"};
    switch (selectionIn(options, 3))
    {
    case 0:
        willExit = StartGame(pBoard);
        // saveRecord();
        break;
    case 1:
        // lookUpRank();
        break;
    case 2:
        willExit = TRUE;
    }
    return willExit;
}

BOOL GameInit(OUT PMATRIX *ppBoard, OUT PENV *pOldEnv)
{
    
    MATRIX board = (MATRIX)malloc(sizeof(int *) * SIZE);
    for (int i = 0; i < SIZE; i++)
    {
        board[i] = (int *)malloc(sizeof(int) * SIZE);
        memset(board[i], 0, sizeof(int) * SIZE);
    }
    *ppBoard = &board;

    srand((unsigned)time(NULL));
    restZeros = 16;
    addNewValToBoard(&board);

    // hide cursor after save origin settings
    PENV oldEnv = *pOldEnv;
    oldEnv = (PENV)malloc(sizeof(ENV));
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &oldEnv->cursorInfo);
    CONSOLE_CURSOR_INFO info = {1, 0}; // set cursor invisible
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);

    // TODO: add "read score record from file" function
    // Read record from file, and maybe decrypt it before read record
    scoreRecord = 100; // assuming the highest score record
    currentScore = 0;

    return SetConsoleTitleW(L"进击的2048");
}

BOOL ConsoleRecovery(IN PENV oldEnv)
{
    // recover the environment
    return SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &oldEnv->cursorInfo);
}

int main()
{
    PENV oldEnv;
    PMATRIX pBoard;
    if (!GameInit(&pBoard, &oldEnv))
        return EXIT_FAILURE;

    BOOL willExit;
    do
    {
        willExit = mainMenu(pBoard);
    } while (!willExit);

    return ConsoleRecovery(oldEnv);
}
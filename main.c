#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <conio.h>
#include "game.h"

// NOTE: maybe it's better to set "PMATRIX board" as global variable;

typedef int **MATRIX;
typedef MATRIX *PMATRIX;

// Global data
int boardSize = 4;
int currentScore;
int scoreRecord;
int restZeros;

int saveRecord();
int readRecord();

int encodeRecord();
int decodeRecord();

void rotateBoardClockwise(MATRIX board, int times);
int slideLeft(MATRIX board);
int slideRight(MATRIX board);
int slideUp(MATRIX board);
int slideDown(MATRIX board);
void addNewValToBoard(MATRIX board);

void getColor(int val, char *buffer, int length);
void resetColor();
void displayBoard(MATRIX Board);

BOOL GameInit(OUT PMATRIX *pBoard, OUT PENV *pOldEnv);
BOOL StartGame(MATRIX board);

void lookUpRank();

void rotateBoardClockwise(MATRIX board, int times)
{
    // rotate square matrix, (not only 4 x 4 square)
    if (0 == times || 4 == times)
    {
        return;
    }
    while (times--)
    {
        int step = 0;
        int *tempArr = (int *)malloc(sizeof(int) * (boardSize - step - 1));
        while (step < (boardSize / 2))
        {
            int i;
            int j = 0;
            for (i = boardSize - 1 - step; i > step; i--)
                tempArr[j++] = board[i][boardSize - 1 - step];

            for (i = boardSize - 1 - step; i > step; i--)
                board[i][boardSize - 1 - step] = board[step][i];
            for (i = boardSize - 1 - step; i > step; i--)
                board[step][i] = board[boardSize - 1 - i][step];
            for (i = step; i < boardSize - 1 - step; i++)
                board[i][step] = board[boardSize - 1 - step][i];

            j = 0;
            for (i = step; i < boardSize - 1 - step; i++)
            {
                board[boardSize - 1 - step][i] = tempArr[j++];
            }
            step++;
        }
    }
}

int slideUp(MATRIX board)
{
    int *columnNonZeroNums = (int *)malloc(sizeof(int) * boardSize);
    int columnNumNotZero;
    restZeros = 0;
    int i;
    int val;
    for (int row, col = 0; col < boardSize; col++)
    {
        row = 0;
        columnNumNotZero = 0;
        while (row < boardSize)
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
            while (row < boardSize)
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

        for (; i < boardSize; i++)
        {
            board[i][col] = 0;
        }

        restZeros += boardSize - columnNumNotZero;
    }
    free(columnNonZeroNums);
    return restZeros;
}

int slideDown(MATRIX board)
{
    rotateBoardClockwise(board, 2);
    slideUp(board);
    rotateBoardClockwise(board, 2);
    return restZeros;
}

int slideLeft(MATRIX board)
{
    rotateBoardClockwise(board, 1);
    slideUp(board);
    rotateBoardClockwise(board, 3);
    return restZeros;
}

int slideRight(MATRIX board)
{
    rotateBoardClockwise(board, 3);
    slideUp(board);
    rotateBoardClockwise(board, 1);
    return restZeros;
}

void addNewValToBoard(MATRIX board)
{
    int pos = rand() % restZeros;
    for (int i = 0; i < boardSize; i++)
    {
        for (int j = 0; j < boardSize; j++)
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

void displayBoard(MATRIX board)
{
    system("cls");

    // DEBUG
    for (int i = 0; i < boardSize; i++)
    {
        for (int j = 0; j < boardSize; j++)
        {
            printf("%2d ", board[i][j]);
        }
        printf("\n");
    }

    printf(" - 2048 -\n");
    printf("Highest score: %-d pts\nCurrent Score: %-d pts\n\n",
           scoreRecord, currentScore);
    char color[32];
    int row, col;
    for (row = 0; row < boardSize; row++)
    {
        for (col = 0; col < boardSize; col++)
        {
            getColor(board[row][col], color, 32);
            printf("%s", color);
            printf("%7s", "");
            resetColor();
        }
        printf("\n");
        for (col = 0; col < boardSize; col++)
        {
            getColor(board[row][col], color, 32);
            printf("%s", color);
            if (board[row][col])
            {
                char valStr[8];
                sprintf_s(valStr, 8, "%d", board[row][col]);
                int t = 7 - strlen(valStr);
                printf("%*s%s%*s", t - t / 2, "", valStr, t / 2, "");
            }
            else
            {
                printf("       ");
            }
            resetColor();
        }
        printf("\n");
        for (col = 0; col < boardSize; col++)
        {
            getColor(board[row][col], color, 32);
            printf("%s", color);
            printf("%7s", "");
            resetColor();
        }
        printf("\n");
    }
    printf("\nControl: WSAD or ←,↑,→,↓ to slide blocks");
    printf("\n         Enter Q to return to home menu");
}

void getColor(int val, char *buffer, int length)
{
    UINT8 color = 0;
    UINT8 colorPairs[] = {255, 8, 255, 1, 255, 2, 255, 3, 255, 4, 255, 5,
                          255, 6, 255, 7, 9, 0, 10, 0, 11, 0, 12, 0, 13,
                          0, 14, 0, 255, 0, 255, 0}; // (f color, b color) pairs
    if (val)
    {
        color = 0;
        while ((val & 1) ^ 1)
        {
            val >>= 1;
            color++;
        }
    }
    sprintf_s(buffer, length, "\033[38;5;%d;48;5;%dm",
              colorPairs[color], colorPairs[color + 1]);
}

void resetColor()
{
    printf("\033[m");
}

BOOL StartGame(MATRIX board)
{

    displayBoard(board);
    BOOL willExit = FALSE;
    int key, key_extra;
    while (!willExit)
    {
        key = _getch();
        switch (key)
        {
        case 'w':
        case 'W':
            slideUp(board);
            break;
        case 's':
        case 'S':
            slideDown(board);
            break;
        case 'A':
        case 'a':
            slideLeft(board);
            break;
        case 'D':
        case 'd':
            slideRight(board);
            break;
        case 0xE0:
            switch (key_extra = _getch())
            {
            case 72:
                slideUp(board);
                break;
            case 80:
                slideDown(board);
                break;
            case 75:
                slideLeft(board);
                break;
            case 77:
                slideRight(board);
                break;
            default:
                break;
            }
        default:
            break;
        }
        displayBoard(board);
        addNewValToBoard(board);
        displayBoard(board);
    }
    return TRUE;
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
            printf("\n\n");
            printf("\t%s %s\n", i == selection ? prefix : " ", options[i]);
        }
        key = _getch();
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
            switch (key_extra = _getch())
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

BOOL mainMenu(MATRIX board)
{
    BOOL willExit = FALSE;
    const char *options[] = {"开始游戏", "查看排名", "退出游戏"};
    switch (selectionIn(options, 3))
    {
    case 0:
        willExit = StartGame(board);
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

    MATRIX board = (MATRIX)malloc(sizeof(int *) * boardSize);
    for (int i = 0; i < boardSize; i++)
    {
        board[i] = (int *)malloc(sizeof(int) * boardSize);
        memset(board[i], 0, sizeof(int) * boardSize);
    }
    *ppBoard = &board;

    srand((unsigned)time(NULL));
    restZeros = 16;
    addNewValToBoard(board);

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

    // DEBUG
    for (int i = 0; i < boardSize; i++)
    {
        for (int j = 0; j < boardSize; j++)
        {
            printf("%2d ", board[i][j]);
        }
        printf("\n");
    }

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
    MATRIX board = *pBoard;

    BOOL willExit;
    do
    {
        willExit = mainMenu(board);
    } while (!willExit);

    return ConsoleRecovery(oldEnv);
}
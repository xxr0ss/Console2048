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

BOOL rotateBoardClockwise(int board[SIZE][SIZE]);
BOOL slideLeft(int board[SIZE][SIZE]);
BOOL slideRight(int board[SIZE][SIZE]);
BOOL slideUp(int board[SIZE][SIZE]);
BOOL slideDown(int board[SIZE][SIZE]);

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

BOOL rotateBoardClockwise(int board[SIZE][SIZE])
{
}

BOOL slideUp(int board[SIZE][SIZE])
{
    int* notZero = (int*)malloc(sizeof(int) * SIZE);
    int numNotZero;
    for (int col = 0; col < SIZE; col++)
    {
        // Add two block of same number
        for (int row = 0; row < SIZE - 1; row++){
            if (board[col][row+1] == board[col][row]){
                board[col][row] <<= 1;
                board[col][row + 1] == 0;
                row += 2;
            }
        }
        // save number of numbers which != 0 in a column
        numNotZero = 0;
        for(int row = 1; row < SIZE; row++){
            if (board[col][row]){
                notZero[numNotZero++] = board[col][row];
            }
        }
        // put all none zero numbers in a column of the board
        for(int i = 0; i < numNotZero; i++){
            board[col][i] = notZero[i];
        }
    }
}

BOOL slideLeft(int board[SIZE][SIZE])
{
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
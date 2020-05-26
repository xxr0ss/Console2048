#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <conio.h>
#include "game.h"
#include "gameMenu.h"

// NOTE: maybe it's better to set "PMATRIX board" as global variable;

int boardSize = 4;
int currentScore;
int scoreRecord;
int restZeros;

/* Data control */
int saveRecord();
int readRecord();

/* Funtions act on the board */
MATRIX createEmptyBoard();
void freeBoard(MATRIX board);
void rotateBoardClockwise(MATRIX board, int times);
int slideLeft(MATRIX board);
int slideRight(MATRIX board);
int slideUp(MATRIX board);
int slideDown(MATRIX board);
void clearBoard(MATRIX board);
void addNewValToBoard(MATRIX board);

/* Display related */
void getColor(int val, char *buffer, int length);
void resetColor();
void displayBoard(MATRIX Board);
int calCurrentScore(MATRIX Board);

/* Game environment related */
BOOL GameInit(OUT PMATRIX *pBoard, OUT PENV *pOldEnv);
BOOL runGame(MATRIX board, BOOL newGame);
void lookUpRank();



MATRIX createEmptyBoard()
{
    MATRIX board = (MATRIX)calloc(boardSize, sizeof(int *));
    board[0] = (int *)calloc(boardSize * boardSize, sizeof(int));
    for (int i = 0; i < boardSize; i++)
    {
        board[i] = board[0] + boardSize * i;
    }
    return board;
}

void freeBoard(MATRIX board)
{
    if (board[0] != NULL)
        free(board[0]);
    if (board != NULL)
        free(board);
}

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
    int *board1D = board[0];
    for (int i = 0; i < boardSize * boardSize; i++)
    {
        if (!board1D[i])
        {
            if (!pos)
            {
                int times = (rand() % 3) ? 1 : 2; // possibility of 2: 2/3
                board1D[i] = 2 * times;
                restZeros -= 1;
                break;
            }
            pos--;
        }
    }
}

void clearBoard(MATRIX board)
{
    for (int i = 0; i < boardSize; i++)
        memset(board[i], 0, sizeof(int) * boardSize);
    restZeros = boardSize * boardSize;
    // TODO: save current scure
    currentScore = 0;
}

int calCurrentScore(MATRIX board)
{
    currentScore = 0;
    for (int i = 0; i < boardSize; i++)
    {
        for (int j = 0; j < boardSize; j++)
        {
            // score calculation rules: add up all values that is greater than 4
            if (board[i][j] > 4)
            {
                currentScore += board[i][j];
            }
        }
    }
    return currentScore;
}

void displayBoard(MATRIX board)
{
    system("cls");

    currentScore = calCurrentScore(board);
    scoreRecord = currentScore > scoreRecord ? currentScore : scoreRecord;

    int paddinglen;
    paddinglen = (boardSize * 7) / 2 - 5;
    paddinglen = paddinglen > 0 ? paddinglen : 0;
    printf("\n\n%*s> 2048 <\n", paddinglen, "");
    printf("Highest score: %-d pts\nCurrent score: %-d pts\n\n",
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
                int len = strlen(valStr);
                // printf("%*s%s%*s", 7 - (7 - len) / 2 - len, "", valStr, (7 - len) / 2, "");
                // 这样感觉更顺眼
                printf("%*s%s%*s", (7 - len) / 2, "", valStr, 7 - (7 - len) / 2 - len, "");
            }
            else
            {
                printf("%3s.%3s", "", "");
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
    printf("\nControl: WSAD or ←,↑,→,↓");
    printf("\n         Q to return");
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
            color += 2;
        }
    }
    sprintf_s(buffer, length, "\033[38;5;%d;48;5;%dm",
              colorPairs[color], colorPairs[color + 1]);
}

void resetColor()
{
    printf("\033[m");
}

BOOL runGame(MATRIX board, BOOL newGame)
{
    BOOL newValNeeded = TRUE;
    int key, key_extra;
    if (newGame)
        addNewValToBoard(board);
    displayBoard(board);
    while (1)
    {
        newValNeeded = TRUE;
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
        case 'Q':
        case 'q':
            return FALSE;
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
                newValNeeded = FALSE;
            }
            break;
        default:
            newValNeeded = FALSE;
            fflush(stdin);
            break;
        }
        if (newValNeeded)
        {
            addNewValToBoard(board);
        }
        if (restZeros < 1){
            break;
        }
        displayBoard(board);
    }

    // system("cls");
    int paddinglen = (boardSize * 7) / 2 - 6;
    paddinglen = paddinglen > 0 ? paddinglen : 0;
    printf("\n\n%*s- GAME OVER -\n\n", paddinglen, "");
    printf("your score: %-d\nPRESS ANY KEY\n", currentScore);
    key = _getch();
    // When player holds a direction key until gameover, sleep for 0.7s
    // makes it more natural
    Sleep(700);
    fflush(stdin);
    return TRUE;
}

int menuSelection(const char **options, int numOfOptions)
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

BOOL homeMenu(MATRIX board)
{
    BOOL alreadyStarted = FALSE;
    BOOL gameOver = FALSE;
    while (1)
    {
        if (alreadyStarted && !gameOver)
        {
            switch (menuSelection(allOptions, 4))
            {
            case 0:
                clearBoard(board);
                gameOver = runGame(board, TRUE);
                break;
            case 1:
                runGame(board, FALSE);
                break;
            case 2:
                // lookupRank();
            case 3:
                return TRUE;
            }
        }
        else
        {
            switch (menuSelection(lessOptions, 3))
            {
            case 0:
                clearBoard(board);
                gameOver = runGame(board, TRUE);
                break;
            case 1:
                // TODO: lookupRank();
                break;
            case 2:
                return TRUE;
            default:
                break;
            }
            alreadyStarted = TRUE;
        }
    }
    return TRUE;
}

BOOL GameInit(OUT PMATRIX pBoard, OUT PENV *pOldEnv)
{
    *pBoard = createEmptyBoard();
    MATRIX testBoard = createEmptyBoard();
    for (int i = 0; i < 4; i++)
    {
        (*pBoard)[0][i] = i + 1;
    }
    memcpy_s(testBoard[0], boardSize * boardSize, (*pBoard)[0], boardSize * boardSize);

    srand((unsigned)time(NULL));
    restZeros = boardSize * boardSize;

    // hide cursor after save origin settings
    PENV oldEnv = (PENV)malloc(sizeof(ENV));
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &oldEnv->cursorInfo);
    *pOldEnv = oldEnv;

    CONSOLE_CURSOR_INFO info = {1, 0}; // set cursor invisible
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);

    // TODO: add "read score record from file" function
    // Read record from file, and maybe decrypt it before read record
    scoreRecord = 0; // assuming the highest score record
    currentScore = 0;

    return SetConsoleTitleW(L"进击的2048");
}

BOOL restoreEnv(IN PENV oldEnv)
{
    // recover the environment
    return SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &oldEnv->cursorInfo);
}

int main()
{
    MATRIX board;
    PENV oldEnv;

    if (!GameInit(&board, &oldEnv))
        return EXIT_FAILURE;

    homeMenu(board);

    restoreEnv(oldEnv);
    system("cls");
    printf("\n\n\tSee you next time!\n\n");
    exit(0);
    return 0;
}
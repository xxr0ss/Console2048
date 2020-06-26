#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <conio.h>
#include "game.h"


int boardSize = 4;
int currentScore;
int scoreRecord;
int restZeros;

CONFIG defaultConfig = {
    4 // boardSize
};

// DEBUG
int testCase[] = { 2, 8, 64, 128, 
					8, 4, 32, 6,
					4, 8, 4, 2,
					2, 4, 2, 8 };
void useTestCase(MATRIX board) {
	int size = boardSize * boardSize * sizeof(int);
	memcpy_s(board[0], size, testCase, size);
}

int saveSettings(CONFIG config) {
    FILE* settingsFile = fopen(filename, "wb");
	if (settingsFile == NULL) {
		return 0; // cannot save
	}
	// check configuration values
	if (config.boardSize > 8)
		config.boardSize = 8;
	else if (config.boardSize < 2)
		config.boardSize = 2; 

	fwrite((void*)&config, sizeof(CONFIG), 1, settingsFile);
	fclose(settingsFile);
	return 1;
}

int readSettings(OUT PCONFIG pConfig_recv){
    FILE * settingsFile = fopen(filename, "rb");
	if (!settingsFile) {
		settingsFile = fopen(filename, "wb");
		fwrite((void*)&defaultConfig, sizeof(CONFIG), 1, settingsFile);
		fclose(settingsFile);
		pConfig_recv->boardSize = defaultConfig.boardSize;
		return 0;
	}
	fread(pConfig_recv, sizeof(CONFIG), 1, settingsFile);
	fclose(settingsFile);
	return 1;
}

MATRIX createEmptyBoard()
{
	MATRIX board = (MATRIX)calloc(boardSize, sizeof(int*));
	board[0] = (int*)calloc(boardSize * boardSize, sizeof(int));
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
	if (times % 4 == 0)
	{
		return;
	}
	while (times--)
	{
		int step = 0;
		int* tempArr = (int*)malloc(sizeof(int) * (boardSize - step - 1));
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
	int* columnNonZeroNums = (int*)malloc(sizeof(int) * boardSize);
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


BOOL addNewValToBoard(MATRIX board)
{
	BOOL result = FALSE;
	if (restZeros == 0) {
		return result;
	}
	int pos = rand() % restZeros;
	int* board1D = board[0];
	for (int i = 0; i < boardSize * boardSize; i++)
	{
		if (!board1D[i])
		{
			if (!pos)
			{
				int times = (rand() % 3) ? 1 : 2; // possibility of 2: 2/3
				board1D[i] = 2 * times;
				restZeros -= 1;
				result = TRUE;
				break;
			}
			pos--;
		}
	}
	return result;
}

void clearBoard(MATRIX board)
{
	for (int i = 0; i < boardSize; i++)
		memset(board[i], 0, sizeof(int) * boardSize);
	restZeros = boardSize * boardSize;
	currentScore = 0;
}


BOOL canSlide(const MATRIX board)
{
	BOOL result = FALSE;
	int originRestZeros = restZeros;
	MATRIX testBoard = createEmptyBoard();
	int rotateTimes = 0;
	int boardMemSize = (boardSize * boardSize) * sizeof(int);
	do
	{
		memcpy_s(testBoard[0], boardMemSize,
			board[0], boardMemSize);
		rotateBoardClockwise(testBoard, rotateTimes);
		slideUp(testBoard);
		rotateBoardClockwise(testBoard, 4 - rotateTimes);
		if (memcmp(testBoard[0], board[0], boardMemSize))
		{
			// when got one direction that the board will have different
			// restZeros, player can slide;
			result = TRUE;
			break;
		}
	} while (rotateTimes++ < 3);

	freeBoard(testBoard);
	restZeros = originRestZeros;
	return result;
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


void getColor(int val, char* buffer, int length)
{
	UINT8 color = 0;
	UINT8 colorPairs[] = { 255, 8, 255, 1, 255, 2, 255, 3, 255, 4, 255, 5,
						  255, 6, 255, 7, 9, 0, 10, 0, 11, 0, 12, 0, 13,
						  0, 14, 0, 255, 0, 255, 0 }; // (f color, b color) pairs
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

void displayBoard(MATRIX board)
{
	system("cls");
	// DEBUG
	// printf("[*] canSlide: %d\n", canSlide(board));

	currentScore = calCurrentScore(board);
	scoreRecord = currentScore > scoreRecord ? currentScore : scoreRecord;

	int paddinglen;
	paddinglen = (boardSize * 7) / 2 - 5;
	paddinglen = paddinglen > 0 ? paddinglen : 0;
	printf("\n%*s> 2048 <\n", paddinglen, "");
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

BOOL runGame(MATRIX board, BOOL newGame)
{
	BOOL isDirectionKey = TRUE;
	int key = 0, key_extra;
	if (newGame)
		addNewValToBoard(board);
	displayBoard(board);
	while (1)
	{
		isDirectionKey = TRUE;
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
				isDirectionKey = FALSE;
			}
			break;
		default:
			isDirectionKey = FALSE;
			fflush(stdin);
			break;
		}
		
		if (isDirectionKey)
		{
			displayBoard(board);
			Sleep(100);
			if (!addNewValToBoard(board)) {
				if (!canSlide(board)) {
					break;
				}
			}
			displayBoard(board);
		}
	}

    gameOver();
	return TRUE;
}

void gameOver(){
    // system("cls");
	int paddinglen = (boardSize * 7) / 2 - 6;
	paddinglen = paddinglen > 0 ? paddinglen : 0;
	printf("\n\n%*s- GAME OVER -\n\n", paddinglen, "");
	printf("your score: %-d\nPRESS ANY KEY\n", currentScore);
	int key = _getch();
	// When player holds a direction key until gameover, sleep for 0.7s
	// makes it more natural
	Sleep(700);
	fflush(stdin);
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
				settingsMenu();
				break;
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
				//useTestCase(board);
				gameOver = runGame(board, TRUE);
				break;
			case 1:
				settingsMenu();
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


int menuSelection(const char** options, int numOfOptions)
{
	int selection = 0;
	int key;
	int key_extra;
	const char* selection_char = ">";
	do
	{
		system("cls");
		for (int i = 0; i < numOfOptions; i++)
		{
			printf("\n\n");
			printf("\t%s %s\n", i == selection ? selection_char : " ", options[i]);
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

int settingsMenu() {
	CONFIG config;
	int key, key_extra;
	int newSize = boardSize;
	BOOL confirm = FALSE;
	while (!confirm) {
		system("cls");
		printf("\n\n");
		printf("\t            %s\n", newSize < 8 ? "△":" ");
		printf("\t桌面大小   %2d\n", newSize);
		printf("\t            %s\n", newSize > 2? "▽" : " ");
		key = _getch();
		switch (key)
		{
		case 'w':
		case 'W':
			if (newSize < 8) {
				newSize++;
			}
			break;
		case 's':
		case 'S':
			if (newSize > 2) {
				newSize--;
			}
			break;
		case 0xE0:
			// ↑, ↓ keys generates a key and a key_extra
			switch (key_extra = _getch())
			{
			case 72:
				if (newSize < 8) {
					newSize++;
				}
				break;
			case 80:
				if (newSize > 2) {
					newSize--;
				}
				break;
			}
			break;
		case 0x0D:
			confirm = TRUE;
		}
	}
	if (newSize == boardSize) {
		printf("设置未更改!");
	}
	else {
		config.boardSize = newSize;
		if(saveSettings(config) == 0)
            printf("错误：设置无法保存！");
        else
            printf("更改将在下次游戏时生效， 正在返回主界面");
	}
	Sleep(1000);
	return newSize == boardSize;
}

BOOL initialize(OUT PMATRIX pBoard, OUT PENV* pOldEnv)
{
	// hide cursor after save origin configuration
	PENV oldEnv = (PENV)malloc(sizeof(ENV));
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &oldEnv->cursorInfo);
	*pOldEnv = oldEnv;

    CONFIG config;
    if(readSettings(&config) == 0){
        printf("无配置文件，已使用默认配置。");
    }
    boardSize = config.boardSize;
	*pBoard = createEmptyBoard();

	srand((unsigned)time(NULL));
	restZeros = boardSize * boardSize;


	CONSOLE_CURSOR_INFO info = { 1, 0 }; // set cursor invisible
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);

	scoreRecord = 0;
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

	if (!initialize(&board, &oldEnv))
		return EXIT_FAILURE;

	homeMenu(board);

	restoreEnv(oldEnv);
	system("cls");
	printf("\n\n\tSee you next time!\n\n");
	exit(0);
	return 0;
}
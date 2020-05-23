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

BOOL slideLeft(int board[SIZE][SIZE]);
BOOL slideRight(int board[SIZE][SIZE]);
BOOL slideUp(int board[SIZE][SIZE]);
BOOL slideDown(int board[SIZE][SIZE]);
void rotateBoard(int board[SIZE][SIZE]);

BOOL GameInit(OUT PENV oldEnv) {
	// hide cursor
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &oldEnv->cursorInfo);
	CONSOLE_CURSOR_INFO info = { 1, 0 }; // set cursor invisible
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);

	// Read record from file, and maybe decrypt it before read record
	scoreRecord = 100; // assuming the highest score record
	currentScore = 0;


	return SetConsoleTitle(L"进击的2048");
}

BOOL ConsoleRecovery(IN PENV oldEnv) {
	// recover the environment
	return SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &oldEnv->cursorInfo);
}

BOOL StartGame() {
	BOOL willExit = FALSE;
	int key, key_extra;
	while (!willExit) {
		key = getch();
		switch (key) {
		case 'w':
		case 'W':
			up();
		case 's':
		case 'S':
			down();
		case 'A':
		case 'a':
			left();
		case 'D':
		case 'd':
			right();
		case 0xE0:
			switch (key_extra = getch()) {
			case 72:  printf("UP\n"); break;
			case 80:  printf("DOWN\n"); break;
			case 75:  printf("LEFT\n"); break;
			case 77:  printf("RIGHT\n"); break;
			default:
				break;
			}
		case VK_ESCAPE:
			willExit
		default:
			continue;
		}
	}
}

int selectionIn(const char** options, int numOfOptions) {
	int selection = 0;
	int key;
	int key_extra;
	const char* prefix = "*";
	do {
		system('cls');
		for (int i; i < numOfOptions; i++) {
			printf("\n");
			printf("\t%s %s", i == selection ? prefix : " ", options[i]);
		}
		key = getch();
		switch (key) {
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
			switch (key_extra = getch()) {
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

BOOL mainMenu() {
	BOOL willExit = FALSE;
	const char* options[] = { "开始游戏", "查看排名", "退出游戏" };
	switch (selectionIn(options, 3)) {
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

int main() {
	PENV oldEnv = (PENV)malloc(sizeof(ENV));
	int** board;
	if (!GameInit(board, oldEnv)) {
		return EXIT_FAILURE;
	}
	BOOL willExit;
	do {
		willExit = mainMenu();
	} while (!willExit);
	return ConsoleRecovery(oldEnv);
}
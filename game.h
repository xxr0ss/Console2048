#include <Windows.h>
#include "gameMenu.h"


typedef int** MATRIX;
typedef MATRIX* PMATRIX;

// Origin environment
typedef struct _CONFIG {
	int boardSize;
}CONFIG, *PCONFIG;

typedef struct _ENV{
	CONSOLE_CURSOR_INFO cursorInfo;
} ENV, *PENV;


/* Data control */
const char* filename = ".\\settings";
int saveSettings(CONFIG config);
int readSettings(OUT PCONFIG pConfig_recv);

/* Funtions act on the board */
MATRIX createEmptyBoard();
void freeBoard(MATRIX board);
void rotateBoardClockwise(MATRIX board, int times);
int slideLeft(MATRIX board);
int slideRight(MATRIX board);
int slideUp(MATRIX board);
int slideDown(MATRIX board);
void clearBoard(MATRIX board);
BOOL canSlide(MATRIX board);
BOOL addNewValToBoard(MATRIX board);

/* Display related */
// Colors
#define RED_BG "\033[1;37;41m"
#define GRAY_BG "\033[1;30;47m"
#define SET_DEFAULT "\033[0m"
void getColor(int val, char* buffer, int length);
void resetColor();
void displayBoard(MATRIX Board);
int settingsMenu();

/* Game environment related */
BOOL gameInit(OUT PMATRIX pBoard, OUT PENV* pOldEnv);
BOOL runGame(MATRIX board, BOOL newGame);
void gameOver();
int calCurrentScore(MATRIX Board);
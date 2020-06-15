#include <Windows.h>

// Colors
#define RED_BG "\033[1;37;41m"
#define GRAY_BG "\033[1;30;47m"
#define SET_DEFAULT "\033[0m"


// Origin environment
typedef struct _ENV{
	CONSOLE_CURSOR_INFO cursorInfo;
} ENV, *PENV;

typedef struct _SETTINGS {
	int boardSize;
}SETTINGS, *PSETTINGS;

typedef int **MATRIX;
typedef MATRIX *PMATRIX;
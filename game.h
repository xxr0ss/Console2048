#include <Windows.h>

// Colors
#define RED_BG "\033[1;37;41m"
#define GRAY_BG "\033[1;30;47m"
#define SET_DEFAULT "\033[0m"

// main menu signals defines
#define START_NEW           0
#define ALREADY_STARTED     1
#define WANT_TO_EXIT        2
/*signal:
    0: start new game
    1: already started new game
    2: choose to exit in mainMenu
*/


// Origin environment
typedef struct _ENV{
	CONSOLE_CURSOR_INFO cursorInfo;
} ENV, *PENV;

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define clear() printf("\033[H\033[J")
#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))
#define VERSION "0.93A"


struct termios originalTerminal;
int ESCgamePaused = 0;

void enableRAWMode();
void disableRAWMode();
void gameSetup();
void drawBoard(int numRows, int numCols, char* titleBoxText);
void *inputread();
void setupThreads();
void *tick();
void *boardThread();

struct winsize w;

int direction = 1;
int curx = 0;
int cury = 0;
int dX = 0;
int dY = 0;
int board[300][500]; // Max size
int snakeLength = 15;

int main() {
	gameSetup();
	return 0;
}

void gameSetup() {
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	char* title;
	asprintf(&title, "Snaek - v%s - [%dx%d]",VERSION,w.ws_col,w.ws_row);
	drawBoard(w.ws_row,w.ws_col,title);
	setupThreads();
}

void setupThreads() {
	pthread_t id_tick, id_inputread;
	
	pthread_create(&id_tick, NULL, tick, NULL);
	pthread_create(&id_inputread, NULL, inputread, NULL);

	pthread_join(id_tick, NULL);
	pthread_join(id_inputread, NULL);
}

// Game board is programmed to take up the height of the terminal and half of the width, centered in the middle of the terminal
//
void drawBoard(int numRows, int numCols, char* titleBoxText) {
	clear();
	printf("╔══╣ %s ╠", titleBoxText);
	for (int i=0; i<(numCols/2)-6-strlen(titleBoxText); i++) printf("═");
	printf("╗\n");
	for (int i=0; i<numRows-2; i++) {
		printf("║");
		for(int i=0; i<(numCols/2); i++) {
			printf(" ");
		}
		printf("║\n");
	}
	printf("╚");
	for (int i=0; i<(numCols/2); i++) printf("═");
	printf("╝");
	for (int i=0; i<numRows; i++) {
		for (int j=0; j<(numCols/2); j++) {

		}
	}
}
void *inputread() {
	
	enableRAWMode();
	char ch;

	while ((ch = getchar()) != 27) {
		if ((ch == 119) && (direction!=3)) {
			direction=1;
			dX=0;
			dY=-1;
		}
		if ((ch == 97) && (direction !=2)) {
			direction=4;
			dX=-1;
			dY=0;
		}
		if ((ch == 100) && (direction !=4)) {
			direction=2;
			dX=1;
			dY=0;
		}
		if ((ch == 115 ) && (direction !=1)) {
			direction=3;
			dX=0;
			dY=1;
		}
	}
	ESCgamePaused = 1;
	
}

void *tick() {
	int curx = w.ws_col/4;
	int cury = w.ws_row/2;
	while (!ESCgamePaused) {
		if ((direction == 1)||(direction == 3)) usleep(80000);
		else usleep(50000);
		gotoxy(curx, cury);
		printf("5\n");
		board[cury][curx] = snakeLength + 1;
		curx += dX;
		cury += dY;
		gotoxy(120,16);
		printf("%d,%d\n",curx,cury);
		gotoxy(curx, cury);

		for (int i=0; i<=w.ws_col/2+1; i++) {
			for (int j=0; j<w.ws_row; j++) {
				int num = board[j][i];
				if (num > 0) board[j][i] --;
				if (num > 1) {
					if (num == snakeLength+1) {
						gotoxy(i,j);
						printf("█\n");
						gotoxy(curx,cury);
					
					}
					else if (num > 2*(snakeLength/(float)3)) {
						gotoxy(i,j);
						printf("▓\n");
						gotoxy(curx,cury);
					
					}
					else if (num > (snakeLength/3)) {
						gotoxy(i,j);
						printf("▒\n");
						gotoxy(curx,cury);
					
					}
					else {
						gotoxy(i,j);
						printf("░\n");
						gotoxy(curx,cury);
					
					}
				} 
				if (num == 1) {
					gotoxy(i,j);
					printf(" \n");
					gotoxy(curx,cury);
				}
			}
		}
		if ((curx<2)||(cury<2)||(curx>w.ws_col/2+1)||(cury>w.ws_row-1)) ESCgamePaused = 1;
	}
}

void placeApple() {

}
/// This function enables RAW mode for terminal.
void enableRAWMode() {
  struct termios raw;
  tcgetattr(STDIN_FILENO, &raw); // Save the state of the terminal to struct raw
                                 // STDIN_FILENO is from <stdlib.h>
                                 // tcgetattr() from <termios.h>
  tcgetattr(STDIN_FILENO, &originalTerminal);
  atexit(&disableRAWMode); // Revert to canonical mode when exiting the program
                           // atext() from <stdlib.h>
  raw.c_lflag &=
      ~(ECHO | ICANON); // Turn off canonical mode
                        // Turn off ECHO mode so that keyboard is not
                        // printing to terminal
                        // ICANON and ECHO is bitflag. ~ is binary NOT operator

  tcsetattr(STDIN_FILENO, TCSAFLUSH,
            &raw); // Set the terminal to be in raw mode
                   // tcsetattr() from <termios.h>
}

void disableRAWMode() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH,
            &originalTerminal); // Set terminal to original state
}

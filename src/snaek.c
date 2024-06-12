#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <pthread.h>
#include <unistd.h>
#define clear() printf("\033[H\033[J")
#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))


struct termios originalTerminal;
int ESCgamePaused = 0;

void enableRAWMode();
void disableRAWMode();
void gameSetup();
void drawBoard();
void *inputread();
void *tick();

int main() {
	gameSetup();
	gotoxy(2,1);
	//scanf("%d");
}

void gameSetup() {
	pthread_t id_tick, id_inputread;
	
	pthread_create(&id_tick, NULL, tick, NULL);
	pthread_create(&id_inputread, NULL, inputread, NULL);

	pthread_join(id_tick, NULL);
	pthread_join(id_inputread, NULL);
	
}

// Note, the entire game board is technically 102x33,
// but the game space is played on a 100x32 
void drawBoard() {
	clear();
	printf("╔══╣ Snaek Game by Aspectious ╠");
	for (int i=0; i<70; i++) printf("═");
	printf("╗\n");
	for (int i=0; i<32; i++) {
		printf("║");
		for(int i=0; i<100; i++) {
			printf(" ");
		}
		printf("║\n");
	}
	printf("╚");
	for (int i=0; i<100; i++) printf("═");
	printf("╝");
}

int direction;
int curx = 50;
int cury = 16;

void *inputread() {
	enableRAWMode();
	char ch;

	while ((ch = getchar()) != 27) {
		if (ch == 119) direction = 1;
		else if (ch == 97 ) direction = 4;
		else if (ch == 100 ) direction = 2;
		else if (ch == 115 ) direction = 3;
		else direction = direction;
	}
	ESCgamePaused = 1;
	
}

void *tick() {
	drawBoard();
	while (!ESCgamePaused) {
		sleep(1);
		switch (direction) {
			case 1:
				cury--;
				break;
			case 2:
				curx++;
				break;
			case 3:
				cury++;
				break;
			case 4:
				curx--;
				break;
			default:
				break;
		}
		if (curx<2) curx=2;
		if (cury<2) cury=2;
		if (curx>100) curx=100;
		if (cury>31) cury=31;
		gotoxy(curx, cury);
	}
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

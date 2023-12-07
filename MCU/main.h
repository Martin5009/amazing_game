#include <stdio.h>
#include <stdlib.h>
#include "STM32L432KC.h"
#include "DE-DP14211.h"
#include "media.h"
#include <math.h>

#define XSTART 1
#define YSTART 5
#define TIMVAL 12000
#define STARTMSPERFRAME 100
#define ROWS 16
#define COLS 32
#define SPD 0.3

char initstate[16][32];
char state[16][32];

// Objects
struct Player {
  float x;
  float y;
};

struct Goal {
  int x;
  int y;
};

// Functions
void delay(int cycles);

void drawPlayer(struct Player p, char board[16][32], char initboard[16][32]);

int movePlayer(struct Player *p, char board[16][32], char initboard[16][32], char x, char y);

void tickTimer(char board[16][32], char initboard[16][32], int tim, int timval);

void drawBoard(char board[16][32], char oldboard[16][32]);

void copyBoard(char board[16][32], char newboard[16][32]);

int checkGoal(struct Player p, struct Goal g[4], int n);

void drawFrame(char (*boards[8])[16][32], char (*initboards[8])[16][32], int frame);

int main(void);


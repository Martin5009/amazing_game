#include <stdio.h>
#include <stdlib.h>
#include "STM32L432KC.h"
#include "DE-DP14211.h"
#include <math.h>

#define XSTART 0
#define YSTART 1
#define TIMVAL 5000
#define STARTMSPERFRAME 40

char initstate[16][32];
char state[16][32];

// Objects
struct Player {
  int x;
  int y;
};

struct Goal {
  int x;
  int y;
};

// Functions
void delay(int cycles);

void drawPlayer(struct Player p, char board[16][32], char initboard[16][32]);

void movePlayer(struct Player *p, char board[16][32], char initboard[16][32], int l, int d, int u, int r);

void tickTimer(char board[16][32], char initboard[16][32], int tim, int timval);

void drawBoard(char board[16][32], char oldboard[16][32]);

void copyBoard(char board[16][32], char newboard[16][32]);

int checkGoal(struct Player p, struct Goal g[4], int n);

void startCount(TIM_TypeDef * TIMx, uint32_t ms);

int checkCount(TIM_TypeDef * TIMx);

void drawFrame(char (*boards[8])[16][32], char (*initboards[8])[16][32], int frame);

int main(void);

// Matrix Presets

char sans[16][32] = {{0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b01, 0b01, 0b01, 0b01, 0b01, 0b01, 0b01, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b00, 0b00, 0b01, 0b01, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b01, 0b01, 0b00, 0b00, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b00, 0b00, 0b01, 0b11, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b11, 0b01, 0b00, 0b00, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b00, 0b00, 0b01, 0b01, 0b01, 0b00, 0b00, 0b01, 0b00, 0b00, 0b01, 0b01, 0b01, 0b00, 0b00, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b01, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b00, 0b00, 0b00, 0b01, 0b01, 0b01, 0b01, 0b01, 0b01, 0b01, 0b01, 0b01, 0b00, 0b00, 0b00, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b00, 0b00, 0b00, 0b01, 0b00, 0b01, 0b00, 0b01, 0b00, 0b01, 0b00, 0b01, 0b00, 0b00, 0b00, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b01, 0b00, 0b00, 0b01, 0b01, 0b01, 0b01, 0b01, 0b01, 0b01, 0b00, 0b00, 0b01, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b01, 0b01, 0b01, 0b01, 0b01, 0b01, 0b01, 0b01, 0b01, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00}};

char happy[16][32] = {{0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                      {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                      {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                      {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                      {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                      {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                      {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                      {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                      {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                      {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                      {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                      {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                      {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                      {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                      {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                      {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}};

char sad[16][32] = {{0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                    {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                    {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                    {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                    {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                    {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                    {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                    {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                    {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                    {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                    {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                    {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                    {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                    {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                    {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0},
                    {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}};

char testmaze[16][32] = {{0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1}, 
                        {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1}, 
                        {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1}, 
                        {0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b1}, 
                        {0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1}, 
                        {0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1}, 
                        {0b1, 0b0, 0b0, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1, 0b1, 0b1, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1}, 
                        {0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1}, 
                        {0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1}, 
                        {0b1, 0b0, 0b0, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1}, 
                        {0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1}, 
                        {0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1}, 
                        {0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b1}, 
                        {0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0}, 
                        {0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0}, 
                        {0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1}};


char start_count1[16][32] = {{0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b11, 0b0, 0b11, 0b0, 0b1, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b11, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b11, 0b1, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}};

char start_count2[16][32] = {{0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b11, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b11, 0b11, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b11, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b11, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}};

char start_count3[16][32] = {{0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b11, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b1, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b1, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b1, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b11, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b11, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b11, 0b11, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b0, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}};


char empty[16][32] = {{0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00},
                    {0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00, 0b00}};

char start_ani1[16][32] = {{0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b11, 0b0, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b1, 0b0, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0}, {0b0, 0b11, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b1, 0b0, 0b1, 0b11, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0}, {0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b11, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0}, {0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b1, 0b0, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0}, {0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b1, 0b0}, {0b0, 0b0, 0b0, 0b11, 0b11, 0b11, 0b0, 0b0, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b11, 0b0, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b11, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0, 0b11, 0b11, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}};

char start_ani2[16][32] = {{0b11, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b11, 0b0, 0b0, 0b11, 0b11, 0b0, 0b11, 0b11, 0b0, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b1, 0b0, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0}, {0b0, 0b11, 0b11, 0b0, 0b0, 0b11, 0b0, 0b0, 0b1, 0b0, 0b1, 0b11, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0}, {0b11, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0}, {0b11, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b11, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0}, {0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b1, 0b0, 0b1, 0b1, 0b1, 0b1, 0b11, 0b0, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0}, {0b11, 0b11, 0b0, 0b11, 0b11, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b11, 0b0, 0b11, 0b11, 0b0, 0b0, 0b1, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b0, 0b0, 0b11, 0b11, 0b0, 0b11, 0b11, 0b0, 0b0, 0b0, 0b0, 0b1, 0b1, 0b0, 0b1, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b1, 0b1, 0b0, 0b1, 0b1}, {0b0, 0b0, 0b0, 0b11, 0b11, 0b11, 0b0, 0b11, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b11, 0b0, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b11, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b11, 0b1, 0b1, 0b0, 0b1, 0b1, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0, 0b11, 0b11, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0}, {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0}};

char start_ani3[16][32] = {{0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                            {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1, 0b1, 0b0, 0b0, 0b1, 0b0, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0}, 
                            {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b1, 0b11, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0}, 
                            {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0}, 
                            {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b11, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b1, 0b0, 0b1, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0}, 
                            {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b0, 0b1, 0b0, 0b0, 0b1, 0b1, 0b0, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0}, 
                            {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                            {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                            {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                            {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                            {0b0, 0b0, 0b0, 0b11, 0b11, 0b11, 0b0, 0b0, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                            {0b0, 0b0, 0b11, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                            {0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b11, 0b0, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                            {0b0, 0b0, 0b11, 0b0, 0b11, 0b1, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b11, 0b0, 0b11, 0b0, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                            {0b0, 0b0, 0b0, 0b0, 0b11, 0b0, 0b0, 0b0, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b11, 0b11, 0b11, 0b11, 0b0, 0b0, 0b0, 0b11, 0b11, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}, 
                            {0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0, 0b0}};

// Matrix Animations

char (*start_ani[3])[16][32] = {&start_ani2, &start_ani1, &start_ani3};
char (*start_ani_init[3])[16][32] = {&start_ani3, &start_ani2, &start_ani1};
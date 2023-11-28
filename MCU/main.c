/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------

File    : main.c
Purpose : Application start

*/

#include "main.h"

/*********************************************************************
*
*       main()
*
*  Function description
*   Application entry point.
*/

/*
Simple delay function
*/
void delay(int cycles) {
   while (cycles-- > 0) {
      volatile int x=200;
      while (x-- > 0)
         __asm("nop");
   }
}


/*
Draws a 32x16 image, represented by a 2D char array, on the DP14311 LED matrix.
Each element in the array encodes one pixel on the matrix according to the
following standard:
  0b00 = off
  0b01 = green
  0b11 = red
This function takes two char arrays.
  board: the image to be drawn
  oldboard: the image currently displayed on the matrix
To avoid having to redraw the board completely every time, the board and oldboard 
arrays are compared against each other to determine which pixels need to be updated.
To completely redraw the board anyway, pass in an array of zeroes into oldboard, 
representing a completely empty image.
*/
void drawBoard(char board[16][32], char oldboard[16][32]) {
  int x;
  int y;
  int n;
  char eng = 0;
  char engold = 0;
  char enr = 0;
  char enrold = 0;
  char px = 0;
  char pxold = 0;
  char rg = 0;
  char rgold = 0;
  char en = 0;
  char enold = 0;
  
  for (x=0; x<32; x++) {
    for (y=0; y<4; y++) {
      eng = 0;
      enr = 0;
      engold = 0;
      enrold = 0;

      for (n=0; n<4; n++) {
        px = board[4*y+n][x];
        rg = px & 0b00000010;
        en = (px & 0b00000001);
        enr |= (rg && en) << (7-n);
        eng |= (!rg && en) << (7-n);

        pxold = oldboard[4*y+n][x];
        rgold = pxold & 0b00000010;
        enold = (pxold & 0b00000001);
        enrold |= (rgold && enold) << (7-n);
        engold |= (!rgold && enold) << (7-n);
      }
      enr = enr >> 4;
      eng = eng >> 4;
      enrold = enrold >> 4;
      engold = engold >> 4;

      if ((enr != enrold) || (eng != engold)) {
        writeDP14211(x, y, 0, eng);
        writeDP14211(x, y, 1, enr);
      }
    }
  }
}

/*
Draws the player character on the gameboard.
  p: an instance of a Player struct
  board: the 2D char array representing the current gameboard
  initboard: the 2D char array representing the initial gameboard, without the player
*/
void drawPlayer(struct Player p, char board[16][32], char initboard[16][32]) {
  drawBoard(initboard, board);
  copyBoard(initboard, board);
  board[p.y][p.x] = 0b11;
  drawBoard(board, initboard);
}

/*
Checks for user input, then moves the player character accordingly.
  *p: a pointer to an instance of a Player struct
  board: the 2D char array representing the current gameboard
  initboard: the 2D char array representing the initial gameboard, without the player
*/
//TODO: modify to work with Nunchuk
void movePlayer(struct Player *p, char board[16][32], char initboard[16][32], int l, int d, int u, int r) {
  int x = p->x;
  int y = p->y;
  if (l && x>0 && board[y][x-1]==0) {
    p->x = x - 1;
    drawPlayer(*p, board, initboard);
  }
  else if (d && y<15 && board[y+1][x]==0) {
    p->y = y + 1;
    drawPlayer(*p, board, initboard);
  }
  else if (u && y>0 && board[y-1][x]==0) {
    p->y = y - 1;
    drawPlayer(*p, board, initboard);
  }
  else if (r && x<31 && board[y][x+1]==0) {
    p->x = x + 1;
    drawPlayer(*p, board, initboard);
  }
}

/*

*/
int tickTimer(char board[16][32], char initboard[16][32], int tim, int timval) {
  int n;
  float test;
  int newtim;
  int col;
  int row;
  char oldboard[16][32];
  
  copyBoard(board, oldboard);

  newtim = tim - 1;
  n = floor((double)tim/(double)timval * 32);
  
  for (col=0 ; col<32 ; col++) {
    board[0][col] = 0b1;
  }

  for (col=0 ; col<n ; col++) {
    board[0][col] = 0b11;
  }
  
  drawBoard(board, oldboard);

  return newtim;
}

/*
Creates a copy of a 2D char array.
  board: the array to be copied
  newboard: the destination of the copy
*/
void copyBoard(char board[16][32], char newboard[16][32]) {
  // Copy board
  int row;
  int col;

  for (row=0 ; row<16 ; row++) {
    for (col=0 ; col<32 ; col++) {
      newboard[row][col] = board[row][col];
    }
  }
}

/*
Returns True if player is at any of the goal points.
  p: instance of a Player struct
  g: array of up to four Goal instances
  n: the first n Goal instances in the array will be checked 
*/
int checkGoal(struct Player p, struct Goal g[4], int n) {
  int cnt = 0;
  int i;
  for (i=0 ; i<n ; i++) {
    cnt = cnt + (p.x == g[i].x && p.y == g[i].y);
  }
  return cnt;
}

int main(void) {
  configureFlash();
  configureClock();
  gpioEnable(GPIO_PORT_A);
  gpioEnable(GPIO_PORT_B);
  gpioEnable(GPIO_PORT_C);
  
  togglePin(PA12); // PA12 as DONE
  pinMode(PA12, GPIO_INPUT);

  // Enable clock output on MCO pin
  RCC->CFGR = _VAL2FLD(RCC_CFGR_MCOSEL, 0b0001);
  togglePin(PA8);
  GPIOA->AFR[1] |= _VAL2FLD(GPIO_AFRH_AFSEL8, 0b0000);
  pinMode(PA8, GPIO_ALT);
  
   // Enable SPI1 peripheral
  RCC->APB2ENR |= (RCC_APB2ENR_SPI1EN);
  initSPI(0b011, 0, 0);
  
  // Configure SPI1 SDO, SDI, CE, and SCK pins
  togglePin(PA5); // PA5 as SCK
  GPIOA->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL5, 0b0101);
  pinMode(PA5, GPIO_ALT);

  togglePin(PB5); // PB5 as SDO
  GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL5, 0b0101);
  pinMode(PB5, GPIO_ALT);

  togglePin(PB4); // PB4 as SDI
  GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL4, 0b0101);
  pinMode(PB4, GPIO_ALT);

  togglePin(PB0); // PB0 as CE
  pinMode(PB0, GPIO_OUTPUT);
  digitalWrite(PB0, 0);

  // Configure user input device pins
  
  togglePin(PA6); //left
  pinMode(PA6, GPIO_INPUT);

  togglePin(PB4); //down
  pinMode(PB4, GPIO_INPUT);

  togglePin(PB1); //up
  pinMode(PB1, GPIO_INPUT);

  togglePin(PA9); //right
  pinMode(PA9, GPIO_INPUT);
  
  // Initialize matrix
  initDP14211();
  clearDP14211();
  
  // Initialize game state
  //  0 = start
  //  1 = play
  //  2 = win
  //  3 = lose
  int state = 0;

  // Create game board
  char initboard[16][32];
  char timboard[16][32];
  char board[16][32];
  
  // Create player object
  struct Player p;

  // Create goal objects
  struct Goal g1 = {31, 14};
  struct Goal g2 = {31, 13};
  struct Goal g[2] = {g1, g2};

  // User input storage
  int lft;
  int dwn;
  int up;
  int rght;

  int cnt = 0;
  int tim;

  // Game Loop
  while (1) {
    // Capture user input
    //TODO: modify for Nunchuk
    lft = !digitalRead(PA6);
    dwn = !digitalRead(PB4);
    up = !digitalRead(PB1);
    rght = !digitalRead(PA9);
    
    // State transition logic

    //TODO: draw start screen & modify for Nunchuk
    if (state == 0) {
      if (cnt == 0) {
        clearDP14211();
        drawBoard(start_ani1, empty);
        cnt++;
      }

      drawBoard(start_ani2, start_ani1);
      delay(200);
      drawBoard(start_ani3, start_ani2);
      delay(200);
      drawBoard(start_ani1, start_ani3);
      delay(200);
      
      if (rght) {
        state = 1;
        
        // Place player at start
        p.x = XSTART;
        p.y = YSTART;

        // Initialize timer
        tim = TIMVAL;

        // Draw maze
        clearDP14211();

        // Draw the counter
        drawBoard(start_count3, empty);
        delay(1000);
        drawBoard(start_count2, start_count3);
        delay(1000);
        drawBoard(start_count1, start_count2);
      
        clearDP14211();

        copyBoard(testmaze, initboard);   // copy desired board to initboard variable
        copyBoard(initboard, board);  // copy initboard to board variable
        copyBoard(board, timboard);
        drawBoard(board, empty);      // draw board
        cnt = 0;

      }
    }
    
    //TODO: modify for Nunchuk
    if (state == 1) {

      if (cnt == 1000) {
        tim = tickTimer(timboard, initboard, tim, TIMVAL);
        cnt = 0;
      }
      movePlayer(&p, board, timboard, lft, dwn, up, rght);
      
      cnt++;
      if (checkGoal(p, g, 2)) {
        state = 2;
        cnt = 0;
      }
      if (tim == 0) {
        state = 3;
        cnt = 0;
      }
    }

    //TODO: draw win screen & modify for Nunchuk
    if (state == 2) {
      if (cnt == 0) {
        clearDP14211();
        drawBoard(happy, empty);
        cnt++;
      }

      if (lft) {
        state = 0;
        cnt = 0;
      }
    }

    //TODO: draw lose screen & modify for Nunchuk
    if (state == 3) {
      if (cnt == 0) {
        clearDP14211();
        drawBoard(sad, empty);
        cnt++;
      }

      if (lft) {
        state = 0;
        cnt = 0;
      }
    }
  }
}

/*************************** End of file ****************************/

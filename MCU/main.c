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
//TODO: modify to work with Nunchuk inputs
void movePlayer(struct Player *p, char board[16][32], char initboard[16][32]) {
  int x = p->x;
  int y = p->y;
  if (!digitalRead(PA6) && x>0 && board[y][x-1]==0) {
    p->x = x - 1;
    drawPlayer(*p, board, initboard);
  }
  else if (!digitalRead(PB4) && y<15 && board[y+1][x]==0) {
    p->y = y + 1;
    drawPlayer(*p, board, initboard);
  }
  else if (!digitalRead(PB1) && y>0 && board[y-1][x]==0) {
    p->y = y - 1;
    drawPlayer(*p, board, initboard);
  }
  else if (!digitalRead(PA9) && x<31 && board[y][x+1]==0) {
    p->x = x + 1;
    drawPlayer(*p, board, initboard);
  }
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
  
  // Initialize matrix and draw maze
  initDP14211();
  clearDP14211();
  
  // Initialize game state
  //  0 = start
  //  1 = play
  //  2 = win
  //  3 = lose
  int state = 1;

  // Create game board
  char initboard[16][32];
  char board[16][32];
  copyBoard(sans, initboard);   // copy desired board to initboard variable
  copyBoard(initboard, board);  // copy initboard to board variable
  drawBoard(board, empty);      // draw board
  
  // Create player object
  struct Player p = {0, 0};
  drawPlayer(p, board, initboard);

  // Create goal object
  struct Goal g = {31, 15};

  // Game Loop
  while (1) {
    // State transition logic
    if (state == 0) {
      //TODO: draw start screen & check for user input
    }
    if (state == 1) {
      movePlayer(&p, board, initboard);

      if ((p.x == g.x) && (p.y == g.y)) {
      state = 2;
    }
    }
    if (state == 2) {
      drawBoard(state1, board);
      //TODO: draw win screen & check for user input
    }
    if (state == 3) {
      //TODO: draw lose screen & check for user input
    }
  }
}

/*************************** End of file ****************************/

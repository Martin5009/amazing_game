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
  board[(int) p.y][(int) p.x] = 0b11;
  drawBoard(board, initboard);
}

/*
Checks for user input, then moves the player character accordingly.
  *p: a pointer to an instance of a Player struct
  board: the 2D char array representing the current gameboard
  initboard: the 2D char array representing the initial gameboard, without the player
*/

int movePlayer(struct Player *p, char board[16][32], char initboard[16][32], char x_in, char y_in) {
  float x = p->x;
  float y = p->y;
  int x_in_int = (int) x_in;
  int y_in_int = (int) y_in;

  if (y_in_int < 10 && x>0 && board[(int) y][(int) x-1]==0) {
    p->x = x - SPD;
    drawPlayer(*p, board, initboard);
    return 1;
  }
  else if (x_in_int > 245 && y<15 && board[(int) y+1][(int) x]==0) {
    p->y = y + SPD;
    drawPlayer(*p, board, initboard);
    return 1;
  }
  else if (x_in_int < 10 && y>0 && board[(int) y-1][(int) x]==0) {
    p->y = y - SPD;
    drawPlayer(*p, board, initboard);
    return 1;
  }
  else if (y_in_int > 245 && x<31 && board[(int) y][(int) x+1]==0) {
    p->x = x + SPD;
    drawPlayer(*p, board, initboard);
    return 1;
  }
  else return 0;
}

/*
Updates the timer bar in the board.
*/
void tickTimer(char board[16][32], char initboard[16][32], int tim, int timval) {
  int n;
  float test;
  int newtim;
  int col;
  int row;
  char oldboard[16][32];
  
  copyBoard(board, oldboard);
  n = floor((double)tim/(double)timval * 32);
  
  for (col=0 ; col<32 ; col++) {
    board[0][col] = 0b1;
    board[15][col] = 0b1;
  }

  for (col=0 ; col<n ; col++) {
    board[0][col] = 0b11;
    board[15][col] = 0b11;
  }
  
  drawBoard(board, oldboard);
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
    cnt = cnt + ((int) p.x == g[i].x && (int) p.y == g[i].y);
  }
  return cnt;
}


void drawFrame(char (*boards[8])[16][32], char (*initboards[8])[16][32], int frame) {
  drawBoard(boards[frame], initboards[frame]);
}

void playNote(int notes[][2], int n) {
  setFreq(TIM16, notes[n][0]);
}

void playSong(int notes[][2]) {
  int i = 0;
  while (1) {
  if (notes[i][1] == 0) {
    break;
  }
  else {
    setFreq(TIM16, notes[i][0]);
    delay_millis(TIM1, notes[i][1]);
  }
  i++;
  }
  setFreq(TIM16, 0);
}

int main(void) {
  configureFlash();
  configureClock();
  gpioEnable(GPIO_PORT_A);
  gpioEnable(GPIO_PORT_B);
  gpioEnable(GPIO_PORT_C);
  
  togglePin(PA12); // PA12 as DONE
  pinMode(PA12, GPIO_INPUT);
  
  // Configure Timers
  togglePin(6); // Toggle pin PA6
  GPIOA->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL6, 14); // Choose PA6 as CH1 of TIM16
  pinMode(6, GPIO_ALT); // Enable PA6
  RCC->CFGR |= _VAL2FLD(RCC_CFGR_PPRE2, 0b000);
  RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;
  RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;
  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
  initTIM(TIM15); // Initialize game timer
  initTIM(TIM16); // Initialize frequency timer
  initTIM(TIM1);  // Initialize delay timer

  // Enable clock output on MCO pin
  RCC->CFGR |= _VAL2FLD(RCC_CFGR_MCOSEL, 0b0001);
  togglePin(PA8);
  GPIOA->AFR[1] |= _VAL2FLD(GPIO_AFRH_AFSEL8, 0b0000);
  pinMode(PA8, GPIO_ALT);
  
  // Enable SPI1 peripheral
  RCC->APB2ENR |= (RCC_APB2ENR_SPI1EN);
  initSPI(0b011, 0, 0);
  
  // Configure SPI1 SDO, SDI, CE, and SCK pins
  togglePin(PB3); // PA5 as SCK
  GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL5, 0b0101);
  pinMode(PB3, GPIO_ALT);

  togglePin(PB5); // PB5 as SDO
  GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL5, 0b0101);
  pinMode(PB5, GPIO_ALT);

  togglePin(PB4); // PB4 as SDI
  GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL4, 0b0101);
  pinMode(PB4, GPIO_ALT);

  togglePin(PB0); // PB0 as CE
  pinMode(PB0, GPIO_OUTPUT);
  digitalWrite(PB0, 0);
  
  // Initialize matrix
  initDP14211();
  clearDP14211();
  
  // Initialize game state
  //  0 = start
  //  1 = play
  //  2 = win
  //  3 = lose
  //  4 = record
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
  int frame;
  int note;
  int i;
  int z;
  int c;
  int best = 0;
  int score;
  
  // Initialize I2C; run 5 times if it randomly crashes
  char *wiidata;
  char *wiiint;

  initI2C();
  initNunchukFirst();
  initNunchukSecond();
  initNunchukThird();
  wiiint = readData();
  
  //playSong(notes0);

  // Game Loop
  while (1) {
    // Capture user input
    initNunchukPrim();
    wiidata = readData();
    z = wiidata[5] & 1;
    c = (wiidata[5] >> 1) & 1;
    
    // State transition logic

    if (state == 0) {
      if (cnt == 0) {
        clearDP14211();
        drawBoard(start_ani1, empty);
        cnt++;
        frame = 0;
        startCount(TIM15, STARTMSPERFRAME); // Start animation timer
      }
      
      // Loop animation
      if (checkCount(TIM15)) {
        drawFrame(start_ani, start_ani_init, frame);
        if (frame >= 2) frame = 0;
        else frame++;
        startCount(TIM15, STARTMSPERFRAME);
      }
      
      // Start game when user input detected

      if (!z) {
        state = 1;

        // Place player at start
        p.x = XSTART;
        p.y = YSTART;

        // Initialize timer
        tim = TIMVAL;

        // Start countdown
        clearDP14211();
        drawBoard(start_count3, empty);
        playSong(countsound);
        delay(4000);
        drawBoard(start_count2, start_count3);
        playSong(countsound);
        delay(4000);
        drawBoard(start_count1, start_count2);
        playSong(countsound);
        delay(4000);

        clearDP14211();
        
        // Draw maze
        copyBoard(testmaze, initboard);   // copy desired board to initboard variable
        copyBoard(initboard, board);  // copy initboard to board variable
        copyBoard(board, timboard);
        drawBoard(board, empty);      // draw board
        cnt = 0;

        playSong(gosound);

      }
    }
    
    if (state == 1) {
      if (cnt == 0) {
        startCount(TIM15, TIMVAL);
        cnt++;
        frame = 0;
        note = 0;
        //startCount(TIM1, 10); // Start song timer
      }
      
      tickTimer(timboard, initboard, TIM15->CNT, TIM15->ARR);
      
      // Loop song
      if (movePlayer(&p, board, timboard, wiidata[0], wiidata[1])) {
        playNote(moveloop, note);
        //startCount(TIM1, notes0[note][1]);
        if (note >= 3) note = 0;
        else note++;
      }
      else TIM16->CR1 &= 0;

      // Check win condition
      if (checkGoal(p, g, 2)) {
        cnt = 0;
        TIM16->CR1 &= 0;
        score = (int) TIM15->ARR - (int) TIM15->CNT;
        if (score > best) {
          state = 4;
          best = score;
        }
        else state = 2;
      }
      
      // Check if time left
      if (checkCount(TIM15)) {
        state = 3;
        cnt = 0;
        TIM16->CR1 &= 0;
      }
    }

    //TODO: draw win screen
    if (state == 2) {
      if (cnt == 0) {
        clearDP14211();
        drawBoard(happy, empty);
        cnt++;
        frame = 0;
        playSong(winsound);
      }

      if (!c) {
        state = 0;
        cnt = 0;
        playSong(userin);
      }
    }

    //TODO: draw lose screen
    if (state == 3) {
      if (cnt == 0) {
        clearDP14211();
        drawBoard(sad, empty);
        cnt++;
        frame = 0;
        playSong(losesound);
      }

      if (!c) {
        state = 0;
        cnt = 0;
        playSong(userin);
      }
    }

    //TODO: draw record screen
    if (state == 4) {
      if (cnt == 0) {
        clearDP14211();
        drawBoard(record_ani1, empty);
        cnt++;
        frame = 0;
        startCount(TIM15, STARTMSPERFRAME*7); // Start animation timer
        playSong(recordsound);
      }

      // Loop animation
      if (checkCount(TIM15)) {
        drawFrame(record_ani, record_ani_init, frame);
        if (frame >= 1) frame = 0;
        else frame++;
        startCount(TIM15, STARTMSPERFRAME*7);
      }

      if (!c) {
        state = 0;
        cnt = 0;
        playSong(userin);
      }
    }
  }
}

/*************************** End of file ****************************/

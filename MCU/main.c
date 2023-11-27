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

void delay(int cycles) {
   while (cycles-- > 0) {
      volatile int x=200;
      while (x-- > 0)
         __asm("nop");
   }
}

void drawState(char state[16][32], char oldstate[16][32]) {
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
        px = state[4*y+n][x];
        rg = px & 0b00000010;
        en = (px & 0b00000001);
        enr |= (rg && en) << (7-n);
        eng |= (!rg && en) << (7-n);

        pxold = oldstate[4*y+n][x];
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

void drawPlayer(struct Player p, char state[16][32], char initstate[16][32]) {
  drawState(initstate, state);
  copyState(initstate, state);
  state[p.y][p.x] = 0b11;
  drawState(state, initstate);
}

void movePlayer(struct Player *p, char state[16][32], char initstate[16][32]) {
  int x = p->x;
  int y = p->y;
  if (!digitalRead(PA6) && x>0 && state[y][x-1]==0) {
    p->x = x - 1;
    drawPlayer(*p, state, initstate);
  }
  else if (!digitalRead(PB4) && y<15 && state[y+1][x]==0) {
    p->y = y + 1;
    drawPlayer(*p, state, initstate);
  }
  else if (!digitalRead(PB1) && y>0 && state[y-1][x]==0) {
    p->y = y - 1;
    drawPlayer(*p, state, initstate);
  }
  else if (!digitalRead(PA9) && x<31 && state[y][x+1]==0) {
    p->x = x + 1;
    drawPlayer(*p, state, initstate);
  }
}

void copyState(char state[16][32], char newstate[16][32]) {
  // Copy state
  int row;
  int col;
  
  for (row=0 ; row<16 ; row++) {
    for (col=0 ; col<32 ; col++) {
      newstate[row][col] = state[row][col];
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
  
  // Create game state
  char initstate[16][32];
  char state[16][32];
  char teststate[16][32];
  copyState(sans, initstate);
  copyState(initstate, state);
  drawState(state, empty);
  
  // Create player object
  struct Player p = {XSTART, YSTART};
  drawPlayer(p, state, initstate);

  // Game Loop
  while (1) {
    movePlayer(&p, state, initstate);
    copyState(empty, teststate);
  }
}

/*************************** End of file ****************************/

/*
File: main.c
Authors: Lawrence Nelson and Cecilia Li
Email: llnelson@g.hmc.edu, celi@g.hmc.edu

This code sets up SPI commuication between our development
board's MCU and FPGA. It also interfaces with an Adafruit Wii
Nunchuk and its breakout board using I2C.
*/

#include "STM32L432KC.h"

int main(void) {
  
  // Each char corresponds to the LEDs of one column unit on the LED display
  // The first byte of each char corresponds to the state of the red LED
  // The second byte of each char corresponds to the state of the green LED
  // Char = R3, R2, R1, R0, G3, G2, G1, G0
  char startScreenMap [16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // First Row Grouping
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};  // Second Row Grouping
  char gameMap[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  
  char *wiidata;
  char *wiiint;
  int gameRun = 0;
  int i;

  // Configure flash latency and set clock to run at 84 MHz
  configureFlash();
  configureClock();

  // "clock divide" = master clock frequency / desired baud rate
  // the phase for the SPI clock is 1 and the polarity is 0
  initSPI(1, 0, 0);
  
  // Initialize I2C; run 5 times if it randomly crashes
  initI2C();
  //initTIM(TIM16);
  initNunchukFirst();
  initNunchukSecond();
  initNunchukThird();
  wiiint = readData();
  initNunchukPrim();

  //x = 0;

  while(1){
    //initNunchukFirst();
    //initNunchukSecond();
    initNunchukPrim();
    wiidata = readData();
  
    for(i=0;i<6;i++){
      printf("%d\n", wiidata[i]);
    };

    printf("_____________\n");
    //delay_millis(TIM16, 10);
  }

  return 1;
}
//////////////////////////////////////////////////////////////////////////////
/*

  // Send Start Screen to FPGA

  while(!gameRun){
    // Read data from Wii Nunchuk and wait for C button input to start game
    wiidata = readData();
    if ((wiidata[5] & (1 << 1)) == 0x02) gameRun = 1;
  };

  // Send Maze Map to FPGA
  sendMap(startScreenMap);

  while(gameRun){
    // Read data from Wii Nunchuk
    wiidata = readData();

    // Update Game Map
    updateMap(wiidata,gameMap);

    // Send new Game Map to FPGA
    sendMap(gameMap);
  };

  // Load and done pins
  //pinMode(PA5, GPIO_OUTPUT);  // LOAD
  //pinMode(PA6, GPIO_INPUT);   // DONE
}

void updateMap(char * wiidata, char * Map){


}


void sendMap(char * Map){
  
  int ledData;
  int i;
  int j;

  for(i = 0; i < 2; i++) {          // scales to 4
    for(j = 0; i < 8; j++){         // scales to 32
      ledData &= ~(0xFF);
      ledData |= (i << 0);          // setting X based on location in gameMap
      ledData |= (j << 5);          // setting Y based on location in gameMap
      ledData |= (Map[j] << 7);     // setting Red and Green LED bits
      spiSendReceive(ledData);
    }
  }
}
*/
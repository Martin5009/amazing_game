// Martin Susanto
// msusanto@hmc.edu
// 12 November 2023
// DE-DP14211 16x32 LED Matrix Driver

#include "DE-DP14211.h"

/*
Initializes the matrix by enabling the system clock. 
Run this at least once before calling any other function.
*/
void initDP14211(void) {
   
  // Disable system 
  digitalWrite(PB0, 1); //Pull CE high
  uint16_t cfig = 0b1000000000000000;
  spiSendReceive(cfig);
  digitalWrite(PB0, 0); //Pull CE low
  checkDone(); //Wait until FPGA is ready to receive

  // Enable system oscillator
  digitalWrite(PB0, 1); //Pull CE high
  cfig = 0b1000000010000000;
  spiSendReceive(cfig);
  digitalWrite(PB0, 0); //Pull CE low
  checkDone(); //Wait until FPGA is ready to receive
  
  // Enable LEDs
  digitalWrite(PB0, 1); //Pull CE high
  cfig = 0b1000000110000000;
  spiSendReceive(cfig);
  digitalWrite(PB0, 0); //Pull CE low
  checkDone(); //Wait until FPGA is ready to receive

  // 16/16 PWM duty cycle
  digitalWrite(PB0, 1); //Pull CE high
  cfig = 0b1101011110000000;
  spiSendReceive(cfig);
  digitalWrite(PB0, 0); //Pull CE low
  checkDone(); //Wait until FPGA is ready to receive
}


/*
Sends a request to the matrix to control a column of four LEDs.
Refer to the communication standard for details on how x, y, g, and r are defined, 
as well as how the LED columns are defined.
  x: the x position of the LED column, where x is the left edge of the display
  y: the y position of the LED column, where y is the top edge of the display
  rg: if set to 1, write message controls red LEDs; otherwise, message controls green LEDs
  en: 4-bit number where each bit toggles one of the four LEDs in the column
*/
void writeDP14211(uint16_t x, uint16_t y, uint16_t rg, uint16_t en) {

  uint16_t send = 0;

  uint16_t x_mask = x;
  uint16_t y_mask = y << 5;
  uint16_t en_mask = en << 7;
  uint16_t rg_mask = rg << 11;

  send |= (x_mask | y_mask | en_mask | rg_mask);
  
  // Write to display 
  digitalWrite(PB0, 1); //Pull CE high
  spiSendReceive(send); 
  digitalWrite(PB0, 0); //Pull CE low
  checkDone(); //Wait until FPGA is ready to receive
}

/*
Clears the entire matrix.
*/
void clearDP14211(void) {
  int x;
  int y;

  for (x=0; x<32; x++) {
    for (y=0; y<4; y++) {
      writeDP14211(x,y,0b0000,0b0000);
      writeDP14211(x,y,0b0001,0b0000);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper Functions
////////////////////////////////////////////////////////////////////////////////////////////////////

/*
Waits in a loop until matrix has finished shifting in data
*/
void checkDone(void) {
  int done = 0;
  while (!done) {
    done = digitalRead(PA12);
  }
}
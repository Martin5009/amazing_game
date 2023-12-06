# amazing_game
Source code for a **timed maze runner game** implemented on a STM32432KC Microcontroller, using an iCE40 UP5K FPGA to drive a DE-DP14112 LED Matrix. User inputs are handled by a 3rd-party Wii Nunchuk, which communicates with the MCU over I2C.

## Structure
* [FPGA](https://github.com/Martin5009/amazing_game/tree/master/FPGA) contains SystemVerilog source code for the LED matrix driver
* [MCU](https://github.com/Martin5009/amazing_game/tree/master/MCU) contains C source code for the game logic, Wii Nunchuk driver, and matrix driver
* [sketchmatrix](https://github.com/Martin5009/amazing_game/tree/master/sketchmatrix) contains a Python script to generate bitmaps for arbitrary images to be drawn on the matrix


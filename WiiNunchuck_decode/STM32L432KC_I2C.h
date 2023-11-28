/*
File: STM32L432KC_I2C.h
Authors: Lawrence Nelson and Cecilia Li
Email: llnelson@g.hmc.edu, celi@g.hmc.edu

Header file for STM32L432KC_I2C.c
*/

#include <stdint.h>
#include <stm32l432xx.h>

#define I2C_SCL PB6
#define I2C_SDA PB7

///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////
void initI2C(void);

void initNunchukFirst(void);
void initNunchukSecond(void);
void initNunchukThird(void);
void initNunchukPrim(void);

char * readData(void);

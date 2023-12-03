// STM32F401RE_TIM.h
// Header for TIM functions

#ifndef STM32L4_TIM_H
#define STM32L4_TIM_H

#include <stdint.h> // Include stdint header
#include <stm32l432xx.h>
#include "STM32L432KC_GPIO.h"
#include <math.h>

#define DEL_PSC 5000
#define FREQ_PSC 50
#define AHB_PSC 1
#define APB2_PSC 1
#define CLK_F 16*pow(10,6)

///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////

void initTIM(TIM_TypeDef * TIMx);
void delay_millis(TIM_TypeDef * TIMx, uint32_t ms);
void setFreq(TIM_TypeDef * TIMx, float f);
void startCount(TIM_TypeDef * TIMx, uint32_t ms);
int checkCount(TIM_TypeDef * TIMx);

#endif
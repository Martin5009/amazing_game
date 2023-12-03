// STM32F401RE_TIM.c
// TIM functions

#include "STM32L432KC_TIM.h"
#include "STM32L432KC_RCC.h"

void initTIM(TIM_TypeDef * TIMx){
  TIMx->PSC = DEL_PSC;
  // Generate an update event to update prescaler value
  TIMx->EGR |= 1;
  // Enable counter
  TIMx->CR1 |= 1; // Set CEN = 1
}

void delay_millis(TIM_TypeDef * TIMx, uint32_t ms) {
// Delay for specified number of milliseconds ms
TIMx->CR1 &= ~(1 << 0);
TIMx->ARR &= ~(0b1111111111111111 << 0);

TIMx->PSC &= ~(0b1111111111111111 << 0);
TIMx->PSC |= (DEL_PSC << 0);

uint16_t arr_val = round(CLK_F*ms*pow(10,-3)/(AHB_PSC*APB2_PSC*(DEL_PSC+1)) - 1);
TIMx->ARR |= arr_val;

TIMx->EGR |= (1 << 0);
TIMx->SR &= ~(1 << 0);
TIMx->CR1 |= (1 << 0);

while (((TIMx->SR >> 0) & 1) != 1) {
}
TIMx->CR1 &= ~(1 << 0);
TIMx->SR &= ~(1 << 0);
}

void startCount(TIM_TypeDef * TIMx, uint32_t ms){
  uint16_t arr_val = round(CLK_F*ms*pow(10,-3)/(AHB_PSC*APB2_PSC*(DEL_PSC+1)) - 1);
  TIMx->CR1 &= 0;
  TIMx->ARR = arr_val;// Set timer max count
  TIMx->EGR |= 1;     // Force update
  TIMx->SR &= ~(0x1); // Clear UIF
  TIMx->CNT = 0;      // Reset count
  TIMx->CR1 |= 1;
}

int checkCount(TIM_TypeDef * TIMx){
  return (TIMx->SR & 1); // Wait for UIF to go high
}

void setFreq(TIM_TypeDef * TIMx, float f) {
// Set TIMx to a specified output frequency f
TIMx->CR1 &= ~(1 << 0); // Disable counter

if (f != 0) {

TIMx->CCMR1 &= ~(0b111 << 4); // Configure timer for PWM mode
TIMx->CCMR1 |= (0b110 << 4); // Set OC1M bits to 110
TIMx->CCMR1 |= (1 << 3); // OCPE

TIMx->PSC &= ~(0b1111111111111111 << 0);
TIMx->PSC |= (FREQ_PSC << 0);

TIMx->ARR &= ~(0b111111111111111 << 0); // Initialize auto-reload value

uint16_t arr_val = round(CLK_F/(AHB_PSC*APB2_PSC*(FREQ_PSC+1)*f) - 1); // Calculate and set new auto-reload value
TIMx->ARR = arr_val;

TIMx->CCR1 &= 0; // Initialize CCR1
TIMx->CCR1 |= (TIMx->ARR) >> 1; // Set duty cycle to 50%

TIMx->CCER |= (1 << 0); // CC1E
TIMx->BDTR |= (1 << 15); // Set MOE to 1

TIMx->EGR |= (1 << 0); // Initialize registers
TIMx->CR1 |= (1 << 0); // Enable timer

}
}
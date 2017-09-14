#ifndef LED_h
#define LED_h
// #include "stm32f10x.h"
// #include "stm32f10x_gpio.h"

#define LEDOn GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET)
#define LEDOff GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET)

void LED_init();
void Blink(int times);
#endif

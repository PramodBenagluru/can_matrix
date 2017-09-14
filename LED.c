#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "LED.h"
#include "Delay.h"

void LED_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	GPIO_InitTypeDef GPIOC_Init_Struct;	
	GPIOC_Init_Struct.GPIO_Pin = GPIO_Pin_13;
	GPIOC_Init_Struct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOC_Init_Struct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,  & GPIOC_Init_Struct); //Initialize GPIO LED
	LEDOff;
}

void Blink(int times)
{
	int loop_no=0;
			for (loop_no = 0;loop_no< times; loop_no++) {
			LEDOn;
			Delayi(1);
			LEDOff;
			Delayi(1);
		}
}

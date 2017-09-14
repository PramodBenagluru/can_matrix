#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_can.h"
#include "Delay.h"
#include "LED.h"
#include "Relay_Config.h"

void initializeRelayPort(void)
{
	GPIO_InitTypeDef GPIOB_Init_Struct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//Enable clock for gpiob
	GPIOB_Init_Struct.GPIO_Pin = GPIO_Pin_All;
	GPIOB_Init_Struct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOB_Init_Struct.GPIO_Speed = GPIO_Speed_50MHz;//initialize relay control pins
	GPIO_Init(GPIOB,  & GPIOB_Init_Struct);
	All_Relay_Off;
}
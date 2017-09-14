#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_can.h"
#include "General.h"
#include "Delay.h"
#include "LED.h"
#include "CAN_Config.h"
#include "Relay_Config.h"


void SetAllClocksTo8Mhz(void)
{
	RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	SystemCoreClockUpdate();
	RCC_PCLK1Config(RCC_HCLK_Div1);
	RCC_PCLK2Config(RCC_HCLK_Div1);
}

void SysTick_Handler()
{
    if (Relay1_Timer != 0)
    {
    	Relay1_Timer--;
    }
    else if((On_Relay_Timer_Flag & 0x80)!=0)
    {
    	GPIO_WriteBit(GPIOB, GPIO_Pin_9, Bit_RESET);
    	On_Relay_Timer_Flag&=0x70;	//Clear flag
    	if(Response_Mess==ENABLE)
    	{
		TxMessage.Data[0] = On_Relay_Timer_Flag|0x0A;
		CAN_Transmit(CAN1, &TxMessage);
    	}
	}
    if (Relay2_Timer != 0)
    {
    	Relay2_Timer--;
    }
    else if((On_Relay_Timer_Flag & 0x40)!=0)
    {
    	GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_RESET);
    	On_Relay_Timer_Flag&=0xB0;
    	if(Response_Mess==ENABLE)
    	{
		TxMessage.Data[0] = On_Relay_Timer_Flag|0x0A;
		CAN_Transmit(CAN1, &TxMessage);
    	}
	}
    if (Relay3_Timer != 0)
    {
    	Relay3_Timer--;
    }
    else if((On_Relay_Timer_Flag & 0x20)!=0)
    {
    	GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_RESET);
    	On_Relay_Timer_Flag&=0xD0;
    	if(Response_Mess==ENABLE)
    	{
		TxMessage.Data[0] = On_Relay_Timer_Flag|0x0A;
		CAN_Transmit(CAN1, &TxMessage);
    	}
	}
    if (Relay4_Timer != 0)
    {
    	Relay4_Timer--;
    }
    else if((On_Relay_Timer_Flag & 0x10)!=0)
    {
    	GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_RESET);
    	On_Relay_Timer_Flag&=0xE0;
    	if(Response_Mess==ENABLE)
    	{
		TxMessage.Data[0] = On_Relay_Timer_Flag|0x0A;
		CAN_Transmit(CAN1, &TxMessage);
    	}
	}
	if(On_Relay_Timer_Flag==0x00)
	{
		LEDOff;
		Stop_Timer();
	}
}

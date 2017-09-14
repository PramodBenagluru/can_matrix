#include "Delay.h"
#include "LED.h"

volatile uint16_t Global_Timer=0;
volatile uint16_t Relay1_Timer=0;
volatile uint16_t Relay2_Timer=0;
volatile uint16_t Relay3_Timer=0;
volatile uint16_t Relay4_Timer=0;
uint8_t On_Relay_Timer_Flag=0x00;

void Delay(uint16_t millis) { //in ms //Max 65.5 seconds @ 8MHz
	Global_Timer=millis;
	while(Global_Timer);
}

void Delayi(int ticks) {
	int i = 0,j = 0;
	for (j = 0; j < ticks; j++) {
		for (i = 0; i < 100000; i++);
		for (i = 0; i < 100000; i++);
		for (i = 0; i < 100000; i++);
	}
}

void Stop_Timer(void)
{
	  SysTick->CTRL  = (uint32_t)0;
}

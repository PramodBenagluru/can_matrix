#ifndef Delay_h
#define Delay_h
#include "stm32f10x.h"

#define Reload_1ms_8M 8000



extern volatile uint16_t Global_Timer;
extern volatile uint16_t Relay1_Timer;
extern volatile uint16_t Relay2_Timer;
extern volatile uint16_t Relay3_Timer;
extern volatile uint16_t Relay4_Timer;
extern uint8_t On_Relay_Timer_Flag;

void Delay(uint16_t millis);
void Delayi(int ticks);
void Blink(int times);
void Stop_Timer(void);
#endif

#ifndef General_h
#define General_h
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_can.h"
#include "Delay.h"
#include "LED.h"

#define SysTickStart SysTick_Config(Reload_1ms_8M)
#define CAN_Tx_Pin GPIO_Pin_12
#define CAN_Rx_Pin GPIO_Pin_11

typedef enum {Success,Failure} Action_Status;
void SetAllClocksTo8Mhz(void);
#endif

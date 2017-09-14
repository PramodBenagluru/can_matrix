#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_can.h"
#include "Delay.h"
#include "LED.h"
#include "CAN_Config.h"
#include "General.h"

FunctionalState Response_Mess=ENABLE;

/* Response message */
CanTxMsg TxMessage={
		.StdId = TxId,
		.RTR = CAN_RTR_DATA,
		.IDE = CAN_ID_STD,
		.DLC = 1,
		.Data[0] = 0xFF
};

/* Receive message data structure Intialized*/
CanRxMsg RxMessage={
		.StdId = 0x7FF,
		.RTR = CAN_RTR_DATA,
		.IDE = CAN_ID_STD,
		.DLC = 1,
		.Data[0] = 0x00
};

/* Pin A0 and A1 select CAN baud rate */
void Baud_Select_Pins_Config(void)
{
	GPIO_InitTypeDef GPIOA_Init_Struct;
	GPIOA_Init_Struct.GPIO_Pin = (GPIO_Pin_0|GPIO_Pin_1);
	GPIOA_Init_Struct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA,  & GPIOA_Init_Struct); //Initialize can rx;
}

/*CAN Rx and Tx pin config*/
void CAN_Pins_Config(void)
{
	GPIO_InitTypeDef GPIOA_Init_Struct;
	RCC_APB2PeriphClockCmd((RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO),ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE); //Enabling the clock for CAN peripheral.
	GPIOA_Init_Struct.GPIO_Pin = GPIO_Pin_12;
	GPIOA_Init_Struct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIOA_Init_Struct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,  & GPIOA_Init_Struct); //Initialize can Tx;

	GPIOA_Init_Struct.GPIO_Pin = GPIO_Pin_11;
	GPIOA_Init_Struct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA,  & GPIOA_Init_Struct); //Initialize can rx;
}

/* CAN peripheral initialization.
 * One of 4 baud rates is selected according to pins A0 and A1. BS is variable storing read values of pin A0 and A1
 * LED is flashed BS+1 times to indicate initialization success with selected baud rate.*/
void CAN_Peripheral_Init(uint8_t BS)
{
	uint16_t BPS_in_K=0;
	switch(BS)
	{
		case 0: BPS_in_K=1000;
		break;
		case 1: BPS_in_K=125;
		break;
		case 2: BPS_in_K=250;
		break;
		case 3: BPS_in_K=500;
		break;
		default:BPS_in_K=500;
	}
	CAN_InitTypeDef CAN_InitStructure;
	CAN_StructInit(&CAN_InitStructure);
	CAN_InitStructure.CAN_TTCM = DISABLE;	//TIME TRIGGERED CAN DISABLE
	CAN_InitStructure.CAN_ABOM = ENABLE;	//AUTOMATIC BUS-OFF MANAGEMENT ENABLED
	CAN_InitStructure.CAN_AWUM = ENABLE;	//AUTOMATIC WAKE UP MODE ENABLED
	CAN_InitStructure.CAN_NART = DISABLE;	//AUTOMATIC RE-TRANSMISSION ON ARBITRATION LOSS ENABLED
	CAN_InitStructure.CAN_RFLM = DISABLE;  //overflow management
	CAN_InitStructure.CAN_TXFP = DISABLE; //Tx prio according to id.
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal; //Silent mode disable, loopback disable.
	// CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack; 
	if(BPS_in_K<=500)
	{
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;	//Phase1+Phase2+Synch=16 quanta,tq=1/8M
	CAN_InitStructure.CAN_BS1 = CAN_BS1_8tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_7tq;	
	CAN_InitStructure.CAN_Prescaler = (uint16_t)(500/BPS_in_K);
	} else if(BPS_in_K==1000)
	{
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;	//Phase1+Phase2+Synch=8 quanta,tq=1/8M
	CAN_InitStructure.CAN_BS1 = CAN_BS1_4tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;	
	CAN_InitStructure.CAN_Prescaler = 1; //.
	}
	uint8_t Init_Stat = CAN_Init(CAN1,  & CAN_InitStructure);
		if (Init_Stat == CAN_InitStatus_Success) {		
		Blink(BS+1);
	} 		
}

/*One filter used in 16 bit id mask mode. Only 601 and 600 accepted. Any of them can be used since id check is not performed*/
Action_Status CAN_Filter_Initialize(uint16_t Accepted_Id)
{
if(Accepted_Id>0x7FF)
{
	return Failure;
}
	/* Filter initialization parameters for 600 and 601 ids*/
CAN_FilterInitTypeDef CAN_FilterInitStructure;
CAN_FilterInitStructure.CAN_FilterNumber = 0;
CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;

CAN_FilterInitStructure.CAN_FilterIdHigh = (uint16_t)(Accepted_Id<<5);	//For 600	//Programmable
CAN_FilterInitStructure.CAN_FilterIdLow = 0xFFE0;	//Always accepts 7FF.

CAN_FilterInitStructure.CAN_FilterMaskIdHigh =0xFFFF ; //All bits must match
CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0xFFFF;
CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
CAN_FilterInit(&CAN_FilterInitStructure);
return Success;
}

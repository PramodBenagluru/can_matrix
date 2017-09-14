/*Global timer=F8
 No timer =8F*/


#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_can.h"
#include "stm32f10x_flash.h"
#include "Delay.h"
#include "LED.h"
#include "CAN_Config.h"
#include "Relay_Config.h"
#include "General.h"
#include "MyFlash.h"

Action_Status Filter_Init_Stat=Failure;

int main(void) {
/*Clock and peripherals initialization*/

	uint32_t Id_Accepted=0x600;
	uint32_t Temp_Tx_Id=0x604;
	uint16_t Reprog_Count=0;
	uint16_t Temp_relay=0;
	uint16_t Present_State=0;
	uint16_t Temp_Time=0;
/*  Current_Address=page_start_address;
	Current_prog_counter=0; */
	SetAllClocksTo8Mhz();
	LED_init();		
	CAN_Pins_Config();
	Baud_Select_Pins_Config();
	initializeRelayPort();
	CAN_DeInit(CAN1);
	Delayi(1);
	uint8_t BS=((GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))|((GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)<<1)));
	CAN_Peripheral_Init(BS);
	uint32_t Read_Word=Read_Configured_Ids();
	if(Read_Word==0xFFFFFFFF)
	{
		RxMessage.StdId = 0x600;
		TxMessage.StdId = 0x604;
		WriteIdWord(0x600,0x604);
	}
	else
	{
		RxMessage.StdId = EVALREQUESTID(Read_Word);
		TxMessage.StdId = EVALRESPONSEID(Read_Word);
	}
	CAN_Filter_Initialize(RxMessage.StdId);	
	TxMessage.Data[0] = 0xFF;
	CAN_Transmit(CAN1, &TxMessage);
	while(1)
	{
START:	if(FIFO_Not_Empty!=0)	//Poll for message arrival.
		{
			CAN_Receive(CAN1, CAN_FIFO0, & RxMessage);
			TxMessage.Data[0] = 0x93;
			if((RxMessage.DLC>=4))	//Accept only if DLC is greater than or equal to 4
			{
				if(On_Relay_Timer_Flag==0x00) {	//Config requests accepted only if no timer is active.
				switch(RxMessage.Data[3])
				{
				case 0x8B:{
				Id_Accepted=(RxMessage.Data[1]*256)+RxMessage.Data[2];
				if(Id_Accepted<0x7FF)
				{
				RxMessage.StdId=Id_Accepted;
				Filter_Init_Stat=CAN_Filter_Initialize(Id_Accepted);
				if(Filter_Init_Stat==Success)
				{
					TxMessage.Data[0] = RxMessage.Data[3]<<1;
				}			
				}
				}
				break;
				case 0x32:{
				Temp_Tx_Id = (RxMessage.Data[1]*256)+RxMessage.Data[2];	//Ignore Byte 0.
				if(Temp_Tx_Id<0x7FF) {
					TxMessage.StdId=Temp_Tx_Id;
					TxMessage.Data[0]=RxMessage.Data[3]<<1;
				}			
				}
				break;
				case 0x00:{
							 if((RxMessage.Data[0]==0x50)&&(RxMessage.Data[1]==0x4A)&&(RxMessage.Data[2]==0x42))
							 {
							 All_Relay_Off;
							 LEDOff;
							 Stop_Timer();
							 CAN_Filter_Initialize(0x600);	//Reset to default.
							 Reprog_Count=WriteIdWord(0x600,0x604);
							 TxMessage.StdId=0x604;
							 TxMessage.DLC=2;
							 TxMessage.Data[1] = (uint8_t)(Reprog_Count & 0x00FF);
							 TxMessage.Data[0] = (uint8_t)((Reprog_Count & 0xFF00)>>8);
							 Response_Mess=ENABLE;
							 CAN_Transmit(CAN1, &TxMessage);
							 TxMessage.DLC=1;
							 goto START;
							 }
							}
				break;
				case 0xB2:
				{
				Id_Accepted = EVALREQUESTIDfromCAN(RxMessage.Data[0],RxMessage.Data[1],RxMessage.Data[2]);
				Temp_Tx_Id	= EVALRESPONSEIDfromCAN(RxMessage.Data[0],RxMessage.Data[1],RxMessage.Data[2]);
				if((Temp_Tx_Id<0x7FF)&&(Id_Accepted<0x7FF))
				{
				TxMessage.StdId = Temp_Tx_Id;
				Filter_Init_Stat=CAN_Filter_Initialize(Id_Accepted);
				Reprog_Count=WriteIdWord(Id_Accepted,Temp_Tx_Id);
				TxMessage.DLC=2;
				TxMessage.Data[1] = (uint8_t)(Reprog_Count & 0x00FF);
				TxMessage.Data[0] = (uint8_t)((Reprog_Count & 0xFF00)>>8);
				CAN_Transmit(CAN1, &TxMessage);
				TxMessage.DLC=1;
				goto START;
				}
				}
				break;
				case 0xAB:{
					Response_Mess=!Response_Mess;
					TxMessage.Data[0] = RxMessage.Data[3]<<1;
				}
				break;
				default:break;				
				}
				}
			switch(RxMessage.Data[3])
				{
				case 0x8F: {
					if((RxMessage.Data[0] & 0xF0 & On_Relay_Timer_Flag)==0)	// if a timer for relay is running, that bit must be sent as 0. if sent as1, will get error, if sent as 0, ignored. This is to avoid user thinking that relay is set permanantly to on while automatic turn off is enabled.
					{
					Temp_relay=(uint16_t)((RxMessage.Data[0] & 0xF0/* & (~On_Relay_Timer_Flag)*/)|On_Relay_Timer_Flag);	//set/reset relays for which auto turn off not enabled. leaves relays set for auto turn off in the same state.
					GPIO_Write(GPIOB,(Temp_relay<<2));
					TxMessage.Data[0]=0x8F<<1;
					}
					}
				break;
				case 0xF8:	//Same time set for specified relays for auto turn off. Max timeout is 65s specified by byte 2 and byte 3
				{// if a timer for relay is running, that bit must be sent as 0. if sent as1, will get error, if sent as 0, ignored. This is to avoid user thinking that timer is set to on while while timer was already running.
				if(((RxMessage.Data[1]!=0x00)||(RxMessage.Data[2]!=0x00))&&((RxMessage.Data[0] & 0xF0)!=0)&&((RxMessage.Data[0] & 0xF0 & On_Relay_Timer_Flag)==0))
					{
					Present_State=(GPIO_ReadOutputData(GPIOB))>>2;
					Temp_relay=((((uint16_t)(RxMessage.Data[0] | On_Relay_Timer_Flag))|(Present_State)) & 0x00F0);	//set/reset relays for which auto turn off not enabled. leaves relays set for auto turn off in the same state.
					GPIO_Write(GPIOB,(Temp_relay<<2));
					SysTickStart;
					On_Relay_Timer_Flag|=(RxMessage.Data[0] & 0xF0);
					Temp_Time=(RxMessage.Data[1] * 256) + RxMessage.Data[2];
					if((RxMessage.Data[0] & 0x80)!=0)
					{
						Relay1_Timer=Temp_Time;
					}
					if((RxMessage.Data[0] & 0x40)!=0)
					{
						Relay2_Timer=Temp_Time;
					}
					if((RxMessage.Data[0] & 0x20)!=0)
					{
						Relay3_Timer=Temp_Time;
					}
					if((RxMessage.Data[0] & 0x10)!=0)
					{
						Relay4_Timer=Temp_Time;
					}
					LEDOn;
					TxMessage.Data[0] = RxMessage.Data[3]<<1;	//Relays on and timer started.
					}
				}
				break;
				case 0xF4:	//For different timings for individual realys.
				{	
					if(((RxMessage.Data[1]!=0x00)||(RxMessage.Data[2]!=0x00))&&((RxMessage.Data[0] & 0xF0)!=0)&&((RxMessage.Data[0] & 0xF0 & On_Relay_Timer_Flag)==0)&&(RxMessage.DLC>=6))
					{					
						if((RxMessage.Data[0] & 0x10)!=0)
						{
							Temp_Time=(uint16_t)(RxMessage.Data[5]);
							if(Temp_Time==0)
							{
								break;
							}
							Relay1_Timer=Temp_Time;
						}
						if((RxMessage.Data[0] & 0x20)!=0)
						{
							Temp_Time=(uint16_t)(RxMessage.Data[4]);
							if(Temp_Time==0)
							{
								break;
							}
							Relay2_Timer=Temp_Time;
						}
						if((RxMessage.Data[0] & 0x40)!=0)
						{
							Temp_Time=(uint16_t)(RxMessage.Data[2]);
							if(Temp_Time==0)
							{
								break;
							}
							Relay3_Timer=Temp_Time;
						}
						if((RxMessage.Data[0] & 0x80)!=0)
						{
							Temp_Time=(uint16_t)(RxMessage.Data[1]);
							if(Temp_Time==0)
							{
								break;
							}
							Relay4_Timer=Temp_Time;
						}
						Present_State=(GPIO_ReadOutputData(GPIOB))>>2;
						Temp_relay=((((uint16_t)(RxMessage.Data[0] | On_Relay_Timer_Flag))|(Present_State)) & 0x00F0);	//set/reset relays for which auto turn off not enabled. leaves relays set for auto turn off in the same state.
						GPIO_Write(GPIOB,(Temp_relay<<2));
						SysTickStart;
						On_Relay_Timer_Flag|=(RxMessage.Data[0] & 0xF0);
						LEDOn;
						TxMessage.Data[0] = RxMessage.Data[3]<<1;	//Relays on and timer started.
						}
					}
				break;
				case 0x01:
				{
					On_Relay_Timer_Flag=0x00;
					All_Relay_Off;
					LEDOff;
					Stop_Timer();	
					Relay1_Timer=0;
					Relay2_Timer=0;
					Relay3_Timer=0;
					Relay4_Timer=0;
					TxMessage.Data[0] = 0x01<<1;	//Timer aborted.
			    }						
				break;
				default:break;
	           }
	}
			if((Response_Mess==ENABLE)||(TxMessage.Data[0]==0x93))
			{
	CAN_Transmit(CAN1, &TxMessage);
			}
	}
}
}

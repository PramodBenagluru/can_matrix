#ifndef CAN_Config_h
#define CAN_Config_h
#include "General.h"
#define RxId 0x601
#define TxId 0x604

#define EVALREQUESTIDfromCAN(Byte0,Byte1,Byte2) ((uint32_t)Byte2|(((uint32_t)Byte1<<8)&0x00000700))
#define EVALRESPONSEIDfromCAN(Byte0,Byte1,Byte2) (((((uint32_t)Byte1)&0x000000F8)>>3)|((((uint32_t)Byte0)&0x0000003F)<<5))

#define FIFO_Not_Empty (CAN1->RF0R & CAN_RF0R_FMP0)	// No of message pending bits

extern FunctionalState Response_Mess;
extern CanTxMsg TxMessage;
extern CanRxMsg RxMessage;

void CAN_Peripheral_Init(uint8_t BS);
void Baud_Select_Pins_Config(void);
void CAN_Pins_Config(void);
Action_Status CAN_Filter_Initialize(uint16_t Accepted_Id);
#endif

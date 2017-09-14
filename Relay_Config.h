#ifndef Relay_Config_h
#define Relay_Config_h

#define Select_Relays_On GPIO_Write(GPIOB,(((uint16_t)RxMessage.Data[0])<<2)) //Pins 6789 are used.
#define All_Relay_Off GPIO_Write(GPIOB,0x00)
void initializeRelayPort(void);
#endif

//Relay 1 is Bit 8.

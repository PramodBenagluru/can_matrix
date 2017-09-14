#ifndef MyFlash_h
#define MyFlash_h
#include "stm32f10x.h"
#define page_start_address ((uint32_t)0x0800F800)	//Application ends at page 5. Last but one page being used
#define page_End_address ((uint32_t)0x0800FBFB)
#define word_alignment ((uint32_t)4)
#define EVALREQUESTID(ProgWord) ((uint32_t)ProgWord & 0x000007FF)
#define EVALRESPONSEID(ProgWord) (((uint32_t)ProgWord & 0x003FF800)>>11)
#define ReProg_Counter(ProgWord) (((uint32_t)ProgWord & 0xFFC00000)>>22)

extern uint32_t Current_Address;
extern uint32_t Current_prog_counter;
uint16_t WriteIdWord(uint32_t Req_Id,uint32_t Res_Id);
uint32_t Read_Configured_Ids();

#endif




//8B for changing request id until reset.
//32 for changing reponse id until reset.
//B2 for changing response and request ids until reprogram.
//F8 Relay Control Timer On.
//8F Relay control Timer Off .Ignore Bytes 2 and 3.
//00 Full Reset. Key 50 4A 42 in Byte 1,2 and 3.
//01 Relay,Timer, LED Off.
//


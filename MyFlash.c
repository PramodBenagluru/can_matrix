#include "MyFlash.h"
#include "stm32f10x_flash.h"

uint32_t Current_Address=page_start_address;
uint32_t Current_prog_counter=0;




uint16_t WriteIdWord(uint32_t Req_Id,uint32_t Res_Id)
{
uint32_t CProg_Count_To_Write=0;
if(Current_prog_counter>=1023)
	{
	CProg_Count_To_Write=0;
}
else
{
	CProg_Count_To_Write=Current_prog_counter+1;
}
uint32_t Word_To_Write=(uint32_t)(((CProg_Count_To_Write)<<22)|(Req_Id)|(Res_Id<<11));
FLASH_Unlock();
FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
if((Current_Address==page_start_address)&&(Current_prog_counter==0))
{
	FLASH_ProgramWord(page_start_address,Word_To_Write);
	goto FIRSTTIME;
}
else if(Current_Address<page_End_address)
{
Current_Address+=word_alignment;
}
else
{
Current_Address=page_start_address;
FLASH_ErasePage(page_start_address);
}
FLASH_ProgramWord(Current_Address,Word_To_Write);
FIRSTTIME:FLASH_Lock();
Current_prog_counter=CProg_Count_To_Write;
return ((uint16_t)CProg_Count_To_Write);
}

//Should be called before Writeid to update global variables current address and prog counter.
uint32_t Read_Configured_Ids()
{
	uint32_t loc=0;	
	uint32_t Curr_Data=0;
	for (loc=page_start_address;loc<=page_End_address;loc+=word_alignment)
	{
		if((*(__IO uint32_t*) loc) == 0xFFFFFFFF) //
		{
			if(loc==page_start_address)
			{
				return 0xFFFFFFFF;
			}
			else
			{
			Current_Address=loc-word_alignment;	//Previous data is the most recent configured ids, and page is emty after that.
			Curr_Data = (*(__IO uint32_t*) Current_Address);
			Current_prog_counter=ReProg_Counter(Curr_Data);
			return Curr_Data;
			}
		}
		else if(loc==page_End_address)
		{
			Current_Address=page_End_address;
			Curr_Data = (*(__IO uint32_t*) Current_Address);
			Current_prog_counter=ReProg_Counter(Curr_Data);
			return Curr_Data;
		}
	}
}

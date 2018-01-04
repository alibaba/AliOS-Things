/***************************************************************
 *Copyright (C), 2017, Shanghai Eastsoft Microelectronics Co., Ltd.
 ***************************************************************/
#include "lib_aes.h"

void AES_Init(AES_InitStruType * AES_InitStruct)
{


	AES_Reset();

	AES->CON.ENCRYPT = AES_InitStruct->MODE;
	return;
}

void AES_WriteKey(uint32_t *AES_KEY)
{
	uint32_t *address = AES_KEY;
	
	AES->KEY3.AES_KEY3 = *address;
	address ++;
	AES->KEY2.AES_KEY2 = *address;
	address ++;
	AES->KEY1.AES_KEY1 = *address;
	address ++;
	AES->KEY0.AES_KEY0 = *address;
}

void AES_ReadKey(uint32_t * AES_KEY)
{
	uint32_t *address = AES_KEY;
	
	*address = AES->KEY3.AES_KEY3;
	address ++;
	*address = AES->KEY2.AES_KEY2;
	address ++;
	*address = AES->KEY1.AES_KEY1;
	address ++;
	*address = AES->KEY0.AES_KEY0;
	return;
}

void AES_WriteData(uint32_t *AES_DATA)
{
	uint32_t *address = AES_DATA;
	
	AES->DATA3.AES_DATA3 = *address;
	address ++;
	AES->DATA2.AES_DATA2 = *address;
	address ++;
	AES->DATA1.AES_DATA1 = *address;
	address ++;
	AES->DATA0.AES_DATA0 = *address;
}

void AES_ReadData(uint32_t * AES_DATA)
{
	uint32_t *address = AES_DATA;
	
	*address = AES->DATA3.AES_DATA3;
	address ++;
	*address = AES->DATA2.AES_DATA2;
	address ++;
	*address = AES->DATA1.AES_DATA1;
	address ++;
	*address = AES->DATA0.AES_DATA0;
	return;
}

void AES_ITConfig(AES_TYPE_IT AES_IE, TYPE_FUNCEN NewState)
{
	uint32_t Word = AES->CON.Word&0xffffff8e;
	
	if(AES->CON.GO_DONE == 1)
	{
		return;
	}
	
	if (NewState == ENABLE)
		Word |= (uint32_t)AES_IE;
	else
		
		Word &= ~(uint32_t)AES_IE;

	AES->CON.Word = Word;
	return;

}

FlagStatus AES_GetFlagStatus(AES_TYPE_IF IFName)
{

	if(AES->CON.Word & IFName) {
		return SET;
	}
	return RESET;
}

void AES_ClearITPendingBit(AES_TYPE_IF IFName)
{
	if(AES->CON.GO_DONE == SET)
	{
		return;
	}

	AES->CON.IF = SET;

	return;
}

AES_TYPE_DONE AES_GetDoneStatus(void)
{
	if(AES->CON.GO_DONE == 1)
	{
		return AES_DONE_NO;
	}

	return AES_DONE_YES;
}

void AES_Reset(void)
{
	AES->DATA0.Word = 0x00000000;
	AES->DATA1.Word = 0x00000000;
	AES->DATA2.Word = 0x00000000;
	AES->DATA3.Word = 0x00000000;

	AES->KEY0.Word = 0x00000000;
	AES->KEY1.Word = 0x00000000;
	AES->KEY2.Word = 0x00000000;
	AES->KEY3.Word = 0x00000000;

	AES->CON.Word = 0x00000000;
}



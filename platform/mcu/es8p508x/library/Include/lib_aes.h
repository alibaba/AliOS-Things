/***************************************************************
 *Copyright (C), 2017, Shanghai Eastsoft Microelectronics Co., Ltd
 ***************************************************************/
#ifndef __LIBAES_H__
#define __LIBAES_H__

#include "ES8P508x.h"
#include "type.h"

typedef enum {
	AES_MODE_DECRYPT = 0,	
	AES_MODE_ENCRYPT = 1,	
}AES_TYPE_MODE;

typedef enum {
	AES_DONE_YES = 0,	
	AES_DONE_NO = 1,	
}AES_TYPE_DONE;

typedef struct {
	uint32_t DATA[4];
}AES_DATA_TypeDef;

typedef enum {
	AES_IF_IF = 0x80,
}AES_TYPE_IF;

typedef enum {
	AES_IT_IT = 0x40,
}AES_TYPE_IT;

typedef struct {
	uint32_t KEY[4];
}AES_KEY_TypeDef;

typedef struct {
	AES_TYPE_MODE MODE;	
}AES_InitStruType;

#define AES_Enable()	(AES->CON.GO_DONE = 1)
#define AES_Disable()	(AES->CON.GO_DONE = 0)

void AES_Init(AES_InitStruType * AES_InitStruct);
void AES_WriteKey(uint32_t *AES_KEY);
void AES_ReadKey(uint32_t * AES_KEY);

void AES_WriteData(uint32_t *AES_DATA);
void AES_ReadData(uint32_t * AES_DATA);

void AES_ITConfig(AES_TYPE_IT AES_IE, TYPE_FUNCEN NewState);
FlagStatus AES_GetFlagStatus(AES_TYPE_IF Flag);
void AES_ClearITPendingBit(AES_TYPE_IF Flag);

AES_TYPE_DONE AES_GetDoneStatus(void);
void AES_Reset(void);

#endif


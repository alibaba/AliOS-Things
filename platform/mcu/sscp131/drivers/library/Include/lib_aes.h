/*********************************************************
*Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
**********************************************************/

#ifndef __LIB_AES_H__
#define __LIB_AES_H__

#include "P131.h"
#include "type.h"

typedef enum {
    AES_DATA0  = 0,
    AES_DATA1  = 1,
    AES_DATA2  = 2,
    AES_DATA3  = 3,
} AES_DATAx;

typedef enum {
    AES_KEY0  = 0,
    AES_KEY1  = 1,
    AES_KEY2  = 2,
    AES_KEY3  = 3,
} AES_KEYx;

#define AES_Encryption()            (AES->CON.ENCRYPT = 1)

#define AES_Deciphering()           (AES->CON.ENCRYPT = 0)

#define AES_Start()                 (AES->CON.GODONE = 1)
#define AES_Close()                 (AES->CON.GODONE = 0)

#define AES_ClearITPendingBit()     (AES->CON.IF = 1)

void AES_WriteDataWord(AES_DATAx datax, uint32_t dat);
ErrorStatus AES_WriteData4Word(uint32_t *dat, uint8_t len);
uint32_t AES_ReadDataWord(AES_DATAx datax);
ErrorStatus AES_ReadData4Word(uint32_t *dat, uint8_t len);

void AES_WriteKeyWord(AES_KEYx keyx, uint32_t key);
ErrorStatus AES_WriteKey4Word(uint32_t *key, uint8_t len);
uint32_t AES_ReadKeyWord(AES_KEYx keyx);
ErrorStatus AES_ReadKey4Word(uint32_t *key, uint8_t len);

ITStatus AES_GetITStatus(void);


#endif

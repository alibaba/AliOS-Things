/*********************************************************
*Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
**********************************************************/

#include "lib_aes.h"

void AES_ITConfig(TYPE_FUNCEN NewState)
{
    if (NewState == ENABLE)
        AES->CON.IE = 1;
    else
        AES->CON.IE = 0;
}

ITStatus AES_GetITStatus(void)
{
    if(AES->CON.IF)
        return SET;
    else 
        return RESET;
}

void AES_DAMCmd(TYPE_FUNCEN NewState)
{
    if (NewState == ENABLE)        
        AES->CON.DMA_EN = 1;  
    else
       AES->CON.DMA_EN = 0;   
}

void AES_WriteDataWord(AES_DATAx datax, uint32_t dat)
{
    switch(datax)
    {
        case AES_DATA0:
            AES->DATA0.DATA0 = dat;
            break;
        case AES_DATA1:
            AES->DATA1.DATA1 = dat;
            break;
        case AES_DATA2:
            AES->DATA2.DATA2 = dat;
            break;
        case AES_DATA3:
            AES->DATA3.DATA3 = dat;
            break;
    }
}

ErrorStatus AES_WriteData4Word(uint32_t *dat, uint8_t len)
{
    if(len > 4)
        return ERROR;
    if(len >= 1)
        AES->DATA0.DATA0 = dat[0];
    if(len >= 2)
        AES->DATA1.DATA1 = dat[1];
    if(len >= 3)
        AES->DATA2.DATA2 = dat[2];
    if(len >= 4)
        AES->DATA3.DATA3 = dat[3];
    return SUCCESS;
}

uint32_t AES_ReadDataWord(AES_DATAx datax)
{
    switch(datax)
    {
        case AES_DATA0:
            return AES->DATA0.DATA0;
        case AES_DATA1:
            return AES->DATA1.DATA1;
        case AES_DATA2:
            return AES->DATA2.DATA2;
        case AES_DATA3:
            return AES->DATA3.DATA3;
        default:
            return ERROR;
    }
}

ErrorStatus AES_ReadData4Word(uint32_t *dat, uint8_t len)
{
    if(len > 4)
        return ERROR;
    if(len >= 1)
        dat[0] = AES->DATA0.DATA0;
    if(len >= 2)
        dat[1] = AES->DATA1.DATA1;
    if(len >= 3)
        dat[2] = AES->DATA2.DATA2;
    if(len >= 4)
        dat[3] = AES->DATA3.DATA3;
    return SUCCESS;
}

void AES_WriteKeyWord(AES_KEYx keyx, uint32_t key)
{
    switch(keyx)
    {
        case AES_KEY0:
            AES->KEY0.KEY0 = key;
            break;
        case AES_KEY1:
            AES->KEY1.KEY1 = key;
            break;
        case AES_KEY2:
            AES->KEY2.KEY2 = key;
            break;
        case AES_KEY3:
            AES->KEY3.KEY3 = key;
            break;
    }
}

ErrorStatus AES_WriteKey4Word(uint32_t *key, uint8_t len)
{
    if(len > 4)
        return ERROR;
    if(len >= 1)
        AES->KEY0.KEY0 = key[0];
    if(len >= 2)
        AES->KEY1.KEY1 = key[1];
    if(len >= 3)
        AES->KEY2.KEY2 = key[2];
    if(len >= 4)
        AES->KEY3.KEY3 = key[3];
    return SUCCESS;
}

uint32_t AES_ReadKeyWord(AES_KEYx keyx)
{
    switch(keyx)
    {
        case AES_KEY0:
            return AES->DATA0.DATA0;
        case AES_KEY1:
            return AES->DATA1.DATA1;
        case AES_KEY2:
            return AES->DATA2.DATA2;
        case AES_KEY3:
            return AES->DATA3.DATA3;
        default:
            return ERROR;
    }
}

ErrorStatus AES_ReadKey4Word(uint32_t *key, uint8_t len)
{
    if(len > 4)
        return ERROR;
    if(len >= 1)
        key[0] = AES->KEY0.KEY0;
    if(len >= 2)
        key[1] = AES->KEY1.KEY1;
    if(len >= 3)
        key[2] = AES->KEY2.KEY2;
    if(len >= 4)
        key[3] = AES->KEY3.KEY3;
    return SUCCESS; 
}

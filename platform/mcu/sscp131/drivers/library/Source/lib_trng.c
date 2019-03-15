/*********************************************************
*Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
**********************************************************/
#include "lib_trng.h"

void TRNG_Init(TRNG_InitStruType *TRNG_InitStruct)
{
    TRNG->CON0.TEST_EN = 0;
    TRNG->CON0.SEED = TRNG_InitStruct->TRNG_Seed;
    TRNG->CON0.DATA_MODE = TRNG_InitStruct->TRNG_DataOutput;
    TRNG->CON1.START_TIME = TRNG_InitStruct->TRNG_DelayTime;
}

uint32_t TRNG_ReadData(void)
{
    return TRNG->DATA.TRNG_DATA;
}

ErrorStatus TRNG_DataCheck(void)
{
    if(TRNG->CON0.RNG_VALID)
        return SUCCESS;
    else
        return ERROR;
}

ITStatus TRNG_GetITStatus(void)
{
    if(TRNG->CON0.RNG_IF)
        return SET;
    else 
        return RESET;
}




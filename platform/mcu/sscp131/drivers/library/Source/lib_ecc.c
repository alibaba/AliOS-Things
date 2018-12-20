/*********************************************************
*Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
**********************************************************/

#include "lib_ecc.h"

ErrorStatus ECC_ErrorStart(void)
{
    if(ECC->STA.ERR)
        return ERROR;
    else
        return SUCCESS;
}

FlagStatus  ECC_OperationStop(void)
{
    if(ECC->STA.DONE)
        return SET;
    else
        return RESET;
}

FlagStatus  ECC_WorkKeep(void)
{
    if(ECC->STA.WK)
        return RESET;
    else 
        return SET;
}


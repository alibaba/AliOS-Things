/*********************************************************
*Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
**********************************************************/

#ifndef __LIB_ECC_H__
#define __LIB_ECC_H__

#include "P131.h"
#include "type.h"
                           
typedef enum
{
    TRNG_ComMod_ZACAU = 0x0,  //ZACAU
    TRNG_ComMod_DBLU  = 0x1,  //DBLU
    TRNG_ComMod_ZADDC = 0x2,  //ZADDC
    TRNG_ComMod_ZADDU = 0x3,  //ZADDU
} TRNG_Type_ComMod;
                           
typedef enum
{
    TRNG_Mode_H         = 0x0,
    TRNG_Mode_ModMul    = 0x1,
    TRNG_Mode_ModAddi   = 0x2,
    TRNG_Mode_ModSub    = 0x3,
    TRNG_Mode_ModInver  = 0x4,
    TRNG_Mode_MontOut   = 0x5,
    TRNG_Mode_MontIn    = 0x6,
} TRNG_Type_Mode;
                           
typedef enum
{
    TRNG_Comb_Single        = 0x0,
    TRNG_Comb_Combination   = 0x1,
} TRNG_Type_Comb;


#define ESS_Start()         (ECC->CON.RUN = 1)
#define ESS_Close()         (ECC->CON.RUN = 0)

#define ECC_DMAEnable()     (ECC->DMACTR.DMA_EN = 1)
#define ECC_DMADisable()    (ECC->DMACTR.DMA_EN = 0)

#define ESS_Reset_Enable()  (ECC->CON.RST = 1)
#define ESS_Reset_Disable() (ECC->CON.RST = 0)

ErrorStatus ECC_ErrorStart(void);
FlagStatus  ECC_OperationStop(void);
FlagStatus  ECC_WorkKeep(void);

#endif

/*********************************************************
*Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
**********************************************************/

#ifndef __LIB_TRND_H__
#define __LIB_TRNG_H__

#include "P131.h"
#include "type.h"
                     
typedef enum
{
    TRNG_Seed_0 = 0x0,
    TRNG_Seed_1 = 0x1,
} TRNG_Type_Seed;
                           
typedef enum
{
    TRNG_DataOutput_Serial   = 0x0,
    TRNG_DataOutput_Parallel = 0x1,
} TRNG_Type_DataOutput;

typedef struct
{
    TRNG_Type_Seed          TRNG_Seed;
    TRNG_Type_DataOutput    TRNG_DataOutput;
    uint32_t                TRNG_DelayTime;
} TRNG_InitStruType;

#define TRNG_Enable()       (TRNG->CON0.PD = 0)
#define TRNG_Disable()      (TRNG->CON0.PD = 1)

#define TRNG_DMAEnable()    (TRNG->CON0.DMA_EN = 1)
#define TRNG_DMADisable()   (TRNG->CON0.DMA_EN = 0)

#define TRNG_IT_Enable()       (TRNG->CON0.RNG_IE = 1)
#define TRNG_IT_Disable()      (TRNG->CON0.RNG_IE = 0)

#define TRNG_ClearITPendingBit()    (TRNG->CON0.RNG_IF = 1)

void TRNG_Init(TRNG_InitStruType *TRNG_InitStruct);
uint32_t TRNG_ReadData(void);
ErrorStatus TRNG_DataCheck(void);
ITStatus TRNG_GetITStatus(void);

#endif

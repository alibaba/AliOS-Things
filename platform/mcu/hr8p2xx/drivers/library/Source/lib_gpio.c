/***************************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd
 ***************************************************************/

#include "lib_gpio.h"

void GPIO_Init(GPIO_TYPE GPIOx,GPIO_TYPE_PIN PINx, GPIO_InitStruType* GPIO_InitStruct)
{
    if(GPIOx == GPIOA)
    {
        if(PINx/8 == 0)
        {
            GPIO->PAFUN0.Word &=~ ((uint32_t)0x3 << (PINx*4));
            GPIO->PAFUN0.Word |= ((uint32_t)GPIO_InitStruct->GPIO_Func << (PINx*4));

        }
        else if(PINx/8 == 1)
        {
            GPIO->PAFUN1.Word &=~ ((uint32_t)0x3 << (PINx%8*4));
            GPIO->PAFUN1.Word |= ((uint32_t)GPIO_InitStruct->GPIO_Func << (PINx%8*4));
        }
        else if(PINx/8 == 2)
        {
            GPIO->PAFUN2.Word &=~ ((uint32_t)0x3 << (PINx%8*4));
            GPIO->PAFUN2.Word |= ((uint32_t)GPIO_InitStruct->GPIO_Func << (PINx%8*4));
        }
        else 
        {
            GPIO->PAFUN3.Word &=~ ((uint32_t)0x3 << (PINx%8*4));
            GPIO->PAFUN3.Word |= ((uint32_t)GPIO_InitStruct->GPIO_Func << (PINx%8*4));
        }


        GPIO->PADIR.Word &=~((uint32_t)0x1 << PINx);
        GPIO->PADIR.Word |= ((uint32_t)GPIO_InitStruct->GPIO_Direction <<PINx);

        GPIO->PAPUEN.Word &=~((uint32_t)0x1 << PINx);
        GPIO->PAPUEN.Word |= ((uint32_t)GPIO_InitStruct->GPIO_PUEN <<PINx);


        GPIO->PAPDEN.Word &=~((uint32_t)0x1 << PINx);
        GPIO->PAPDEN.Word |= ((uint32_t)GPIO_InitStruct->GPIO_PDEN <<PINx);


        GPIO->PAOD.Word &=~((uint32_t)0x1 << PINx);
        GPIO->PAOD.Word |= ((uint32_t)GPIO_InitStruct->GPIO_OD <<PINx);


    }
    else if(GPIOx == GPIOB)
    {
        if(PINx/8 == 0)
        {
            GPIO->PBFUN0.Word &=~ ((uint32_t)0x3 << (PINx*4));
            GPIO->PBFUN0.Word |= ((uint32_t)GPIO_InitStruct->GPIO_Func << (PINx*4));
        }
        else if(PINx/8 == 1)
        {
            GPIO->PBFUN1.Word &=~ ((uint32_t)0x3 << (PINx%8*4));
            GPIO->PBFUN1.Word |= ((uint32_t)GPIO_InitStruct->GPIO_Func << (PINx%8*4));
        }
        else 
        {
            GPIO->PBFUN2.Word &=~ ((uint32_t)0x3 << (PINx%8*4));
            GPIO->PBFUN2.Word |= ((uint32_t)GPIO_InitStruct->GPIO_Func << (PINx%8*4));                                                              
        }

        GPIO->PBDIR.Word &=~((uint32_t)0x1 << PINx);
        GPIO->PBDIR.Word |= ((uint32_t)GPIO_InitStruct->GPIO_Direction <<PINx);

        GPIO->PBPUEN.Word &=~((uint32_t)0x1 << PINx);
        GPIO->PBPUEN.Word |= ((uint32_t)GPIO_InitStruct->GPIO_PUEN <<PINx);


        GPIO->PBPDEN.Word &=~((uint32_t)0x1 << PINx);
        GPIO->PBPDEN.Word |= ((uint32_t)GPIO_InitStruct->GPIO_PDEN <<PINx);


        GPIO->PBOD.Word &=~((uint32_t)0x1 << PINx);
        GPIO->PBOD.Word |= ((uint32_t)GPIO_InitStruct->GPIO_OD <<PINx);

    }
    else
    {
        if(PINx/8 == 0)
        {
            GPIO->PCFUN0.Word &=~ ((uint32_t)0x3 << (PINx*4));
            GPIO->PCFUN0.Word |= ((uint32_t)GPIO_InitStruct->GPIO_Func << (PINx*4));
        }
        else
        {
            GPIO->PCFUN1.Word &=~ ((uint32_t)0x3 << (PINx%8*4));
            GPIO->PCFUN1.Word |= ((uint32_t)GPIO_InitStruct->GPIO_Func << (PINx%8*4));
        }


        GPIO->PCDIR.Word &=~((uint32_t)0x1 << PINx);
        GPIO->PCDIR.Word |= ((uint32_t)GPIO_InitStruct->GPIO_Direction <<PINx);

        GPIO->PCPUEN.Word &=~((uint32_t)0x1 << PINx);
        GPIO->PCPUEN.Word |= ((uint32_t)GPIO_InitStruct->GPIO_PUEN <<PINx);


        GPIO->PCPDEN.Word &=~((uint32_t)0x1 << PINx);
        GPIO->PCPDEN.Word |= ((uint32_t)GPIO_InitStruct->GPIO_PDEN <<PINx);


        GPIO->PCOD.Word &=~((uint32_t)0x1 << PINx);
        GPIO->PCOD.Word |= ((uint32_t)GPIO_InitStruct->GPIO_OD <<PINx);

    }

}

void GPIO_Write(GPIO_TYPE GPIOx, uint32_t Value)
{
    if(GPIOx == GPIOA)
        GPIO->PA.Word = Value;
    else if(GPIOx == GPIOB)
        GPIO->PB.Word = Value;
    else
        GPIO->PC.Word = Value;          
}

uint32_t GPIO_Read(GPIO_TYPE GPIOx)
{
    if(GPIOx == GPIOA)
        return  GPIO->PA.Word;
    else if(GPIOx == GPIOB)
        return GPIO->PB.Word;
    else
        return GPIO->PC.Word;           
}

PinStatus GPIO_ReadBit(GPIO_TYPE GPIOx,GPIO_TYPE_PIN PINx)
{
    PinStatus bitstatus = RESET;
    if(GPIOx == GPIOA)
    {
        if((GPIO->PA.Word&((uint32_t)0x1<<PINx)) != RESET)
            bitstatus = SET;
        else
            bitstatus = RESET;
    }
    else if(GPIOx == GPIOB)
    {
        if((GPIO->PB.Word&((uint32_t)0x1<<PINx)) != RESET)
            bitstatus = SET;
        else
            bitstatus = RESET;
    }
    else 
    {
        if((GPIO->PC.Word&((uint32_t)0x1<<PINx)) != RESET)
            bitstatus = SET;
        else
            bitstatus = RESET;
    }
    return bitstatus;
}

void GPIOA_SetBit(GPIO_TYPE_PIN PINx)
{
    GPIO->PAS.Word = (uint32_t)0x1<<PINx;
}

void GPIOA_ResetBit(GPIO_TYPE_PIN PINx)
{
    GPIO->PAC.Word = (uint32_t)0x1<<PINx;
}

void GPIOA_ToggleBit(GPIO_TYPE_PIN PINx)
{


    GPIO->PAI.Word = (uint32_t)0x1<<PINx;
}

void GPIOB_SetBit(GPIO_TYPE_PIN PINx)
{
    GPIO->PBS.Word = (uint32_t)0x1<<PINx;
}

void GPIOB_ResetBit(GPIO_TYPE_PIN PINx)
{
    GPIO->PBC.Word = (uint32_t)0x1<<PINx;
}

void GPIOB_ToggleBit(GPIO_TYPE_PIN PINx)
{

    GPIO->PBI.Word = (uint32_t)0x1<<PINx; 
}

void GPIOC_SetBit(GPIO_TYPE_PIN PINx)
{
    GPIO->PCS.Word = (uint32_t)0x1<<PINx;
}

void GPIOC_ResetBit(GPIO_TYPE_PIN PINx)
{
    GPIO->PCC.Word = (uint32_t)0x1<<PINx;
}

void GPIOC_ToggleBit(GPIO_TYPE_PIN PINx)
{

    GPIO->PCI.Word = (uint32_t)0x1<<PINx; 
}

void GPIOA_SetDirection(GPIO_TYPE_PIN PINx, GPIO_TYPE_DIR Dir_Type)
{
    GPIO->PADIR.Word &=~((uint32_t)0x1 << PINx);
    GPIO->PADIR.Word |= ((uint32_t)Dir_Type <<PINx);
}

void GPIOB_SetDirection(GPIO_TYPE_PIN PINx, GPIO_TYPE_DIR Dir_Type)
{
    GPIO->PBDIR.Word &=~((uint32_t)0x1 << PINx);
    GPIO->PBDIR.Word |= ((uint32_t)Dir_Type <<PINx);
}

void GPIOC_SetDirection(GPIO_TYPE_PIN PINx, GPIO_TYPE_DIR Dir_Type)
{
    GPIO->PCDIR.Word &=~((uint32_t)0x1 << PINx);
    GPIO->PCDIR.Word |= ((uint32_t)Dir_Type <<PINx);
}

void PINT_Config(PINT_TYPE PINTx, PINT_TYPE_SEL SELx, PINT_TYPE_TRIG TRIGx)
{
    GPIO->PSEL.Word &= ~((uint32_t)0x07<<(PINTx*4));
    GPIO->PSEL.Word |=((uint32_t)SELx << (PINTx*4));

    GPIO->PCFG.Word &= ~((uint32_t)0x07<<(PINTx*4));    
    GPIO->PCFG.Word |=((uint32_t)TRIGx << (PINTx*4));
}

FlagStatus  PINT_GetITStatus(PINT_TYPE_IT PINT_Flag)
{
    FlagStatus bitstatus = RESET;

    if((GPIO->PIF.Word & (uint32_t)PINT_Flag) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else   
    {
        bitstatus = RESET;
    }
    return  bitstatus;
}

void PINT_ClearITPendingBit(PINT_TYPE_IT PINT_Flag)
{
    GPIO->PIF.Word = (uint32_t)PINT_Flag;
}

/*************************END OF FILE**********************/




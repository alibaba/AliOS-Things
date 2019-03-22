/******************************************************************************************************************************************
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, SYNWIT SHALL NOT BE HELD LIABLE
* FOR ANY DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
* OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONN-
* -ECTION WITH THEIR PRODUCTS.
*
* COPYRIGHT 2012 Synwit Technology
*******************************************************************************************************************************************/

#include "SWM320.h"
#include "SWM320_exti.h"

void EXTI_Init(GPIO_TypeDef * GPIOx, uint32_t n, uint32_t mode)
{
    EXTI_Close(GPIOx, n);

    if(mode & 0x10)
    {
        GPIOx->INTLVLTRG |= (0x01 << n);

        if(mode & 0x01)
            GPIOx->INTRISEEN |= (0x01 << n);
        else
            GPIOx->INTRISEEN &= ~(0x01 << n);
    }
    else
    {
        GPIOx->INTLVLTRG &= ~(0x01 << n);

        if(mode & 0x02)
        {
            GPIOx->INTBE |= (0x01 << n);
        }
        else
        {
            GPIOx->INTBE &= ~(0x01 << n);

            if(mode & 0x01)
                GPIOx->INTRISEEN |= (0x01 << n);
            else
                GPIOx->INTRISEEN &= ~(0x01 << n);
        }
    }

    GPIOx->INTCLR = (1 << n);
}

void EXTI_Open(GPIO_TypeDef * GPIOx, uint32_t n)
{
    GPIOx->INTEN |= (0x01 << n);
}

void EXTI_Close(GPIO_TypeDef * GPIOx, uint32_t n)
{
    GPIOx->INTEN &= ~(0x01 << n);
}

uint32_t EXTI_State(GPIO_TypeDef * GPIOx, uint32_t n)
{
    return (GPIOx->INTSTAT >> n) & 0x01;
}

uint32_t EXTI_RawState(GPIO_TypeDef * GPIOx, uint32_t n)
{
    return (GPIOx->INTRAWSTAT >> 1) & 0x01;
}

void EXTI_Clear(GPIO_TypeDef * GPIOx, uint32_t n)
{
    GPIOx->INTCLR = (0x01 << n);
}

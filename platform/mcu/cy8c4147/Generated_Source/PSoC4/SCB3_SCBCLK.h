/*******************************************************************************
* File Name: SCB3_SCBCLK.h
* Version 2.20
*
*  Description:
*   Provides the function and constant definitions for the clock component.
*
*  Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CLOCK_SCB3_SCBCLK_H)
#define CY_CLOCK_SCB3_SCBCLK_H

#include <cytypes.h>
#include <cyfitter.h>


/***************************************
*        Function Prototypes
***************************************/
#if defined CYREG_PERI_DIV_CMD

void SCB3_SCBCLK_StartEx(uint32 alignClkDiv);
#define SCB3_SCBCLK_Start() \
    SCB3_SCBCLK_StartEx(SCB3_SCBCLK__PA_DIV_ID)

#else

void SCB3_SCBCLK_Start(void);

#endif/* CYREG_PERI_DIV_CMD */

void SCB3_SCBCLK_Stop(void);

void SCB3_SCBCLK_SetFractionalDividerRegister(uint16 clkDivider, uint8 clkFractional);

uint16 SCB3_SCBCLK_GetDividerRegister(void);
uint8  SCB3_SCBCLK_GetFractionalDividerRegister(void);

#define SCB3_SCBCLK_Enable()                         SCB3_SCBCLK_Start()
#define SCB3_SCBCLK_Disable()                        SCB3_SCBCLK_Stop()
#define SCB3_SCBCLK_SetDividerRegister(clkDivider, reset)  \
    SCB3_SCBCLK_SetFractionalDividerRegister((clkDivider), 0u)
#define SCB3_SCBCLK_SetDivider(clkDivider)           SCB3_SCBCLK_SetDividerRegister((clkDivider), 1u)
#define SCB3_SCBCLK_SetDividerValue(clkDivider)      SCB3_SCBCLK_SetDividerRegister((clkDivider) - 1u, 1u)


/***************************************
*             Registers
***************************************/
#if defined CYREG_PERI_DIV_CMD

#define SCB3_SCBCLK_DIV_ID     SCB3_SCBCLK__DIV_ID

#define SCB3_SCBCLK_CMD_REG    (*(reg32 *)CYREG_PERI_DIV_CMD)
#define SCB3_SCBCLK_CTRL_REG   (*(reg32 *)SCB3_SCBCLK__CTRL_REGISTER)
#define SCB3_SCBCLK_DIV_REG    (*(reg32 *)SCB3_SCBCLK__DIV_REGISTER)

#define SCB3_SCBCLK_CMD_DIV_SHIFT          (0u)
#define SCB3_SCBCLK_CMD_PA_DIV_SHIFT       (8u)
#define SCB3_SCBCLK_CMD_DISABLE_SHIFT      (30u)
#define SCB3_SCBCLK_CMD_ENABLE_SHIFT       (31u)

#define SCB3_SCBCLK_CMD_DISABLE_MASK       ((uint32)((uint32)1u << SCB3_SCBCLK_CMD_DISABLE_SHIFT))
#define SCB3_SCBCLK_CMD_ENABLE_MASK        ((uint32)((uint32)1u << SCB3_SCBCLK_CMD_ENABLE_SHIFT))

#define SCB3_SCBCLK_DIV_FRAC_MASK  (0x000000F8u)
#define SCB3_SCBCLK_DIV_FRAC_SHIFT (3u)
#define SCB3_SCBCLK_DIV_INT_MASK   (0xFFFFFF00u)
#define SCB3_SCBCLK_DIV_INT_SHIFT  (8u)

#else 

#define SCB3_SCBCLK_DIV_REG        (*(reg32 *)SCB3_SCBCLK__REGISTER)
#define SCB3_SCBCLK_ENABLE_REG     SCB3_SCBCLK_DIV_REG
#define SCB3_SCBCLK_DIV_FRAC_MASK  SCB3_SCBCLK__FRAC_MASK
#define SCB3_SCBCLK_DIV_FRAC_SHIFT (16u)
#define SCB3_SCBCLK_DIV_INT_MASK   SCB3_SCBCLK__DIVIDER_MASK
#define SCB3_SCBCLK_DIV_INT_SHIFT  (0u)

#endif/* CYREG_PERI_DIV_CMD */

#endif /* !defined(CY_CLOCK_SCB3_SCBCLK_H) */

/* [] END OF FILE */

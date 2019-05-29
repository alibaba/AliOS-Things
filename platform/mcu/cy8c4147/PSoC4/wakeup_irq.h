/*******************************************************************************
* File Name: wakeup_irq.h
* Version 1.70
*
*  Description:
*   Provides the function definitions for the Interrupt Controller.
*
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/
#if !defined(CY_ISR_wakeup_irq_H)
#define CY_ISR_wakeup_irq_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void wakeup_irq_Start(void);
void wakeup_irq_StartEx(cyisraddress address);
void wakeup_irq_Stop(void);

CY_ISR_PROTO(wakeup_irq_Interrupt);

void wakeup_irq_SetVector(cyisraddress address);
cyisraddress wakeup_irq_GetVector(void);

void wakeup_irq_SetPriority(uint8 priority);
uint8 wakeup_irq_GetPriority(void);

void wakeup_irq_Enable(void);
uint8 wakeup_irq_GetState(void);
void wakeup_irq_Disable(void);

void wakeup_irq_SetPending(void);
void wakeup_irq_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the wakeup_irq ISR. */
#define wakeup_irq_INTC_VECTOR            ((reg32 *) wakeup_irq__INTC_VECT)

/* Address of the wakeup_irq ISR priority. */
#define wakeup_irq_INTC_PRIOR             ((reg32 *) wakeup_irq__INTC_PRIOR_REG)

/* Priority of the wakeup_irq interrupt. */
#define wakeup_irq_INTC_PRIOR_NUMBER      wakeup_irq__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable wakeup_irq interrupt. */
#define wakeup_irq_INTC_SET_EN            ((reg32 *) wakeup_irq__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the wakeup_irq interrupt. */
#define wakeup_irq_INTC_CLR_EN            ((reg32 *) wakeup_irq__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the wakeup_irq interrupt state to pending. */
#define wakeup_irq_INTC_SET_PD            ((reg32 *) wakeup_irq__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the wakeup_irq interrupt. */
#define wakeup_irq_INTC_CLR_PD            ((reg32 *) wakeup_irq__INTC_CLR_PD_REG)



#endif /* CY_ISR_wakeup_irq_H */


/* [] END OF FILE */

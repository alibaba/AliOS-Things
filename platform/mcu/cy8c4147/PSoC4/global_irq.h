/*******************************************************************************
* File Name: global_irq.h
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
#if !defined(CY_ISR_global_irq_H)
#define CY_ISR_global_irq_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void global_irq_Start(void);
void global_irq_StartEx(cyisraddress address);
void global_irq_Stop(void);

CY_ISR_PROTO(global_irq_Interrupt);

void global_irq_SetVector(cyisraddress address);
cyisraddress global_irq_GetVector(void);

void global_irq_SetPriority(uint8 priority);
uint8 global_irq_GetPriority(void);

void global_irq_Enable(void);
uint8 global_irq_GetState(void);
void global_irq_Disable(void);

void global_irq_SetPending(void);
void global_irq_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the global_irq ISR. */
#define global_irq_INTC_VECTOR            ((reg32 *) global_irq__INTC_VECT)

/* Address of the global_irq ISR priority. */
#define global_irq_INTC_PRIOR             ((reg32 *) global_irq__INTC_PRIOR_REG)

/* Priority of the global_irq interrupt. */
#define global_irq_INTC_PRIOR_NUMBER      global_irq__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable global_irq interrupt. */
#define global_irq_INTC_SET_EN            ((reg32 *) global_irq__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the global_irq interrupt. */
#define global_irq_INTC_CLR_EN            ((reg32 *) global_irq__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the global_irq interrupt state to pending. */
#define global_irq_INTC_SET_PD            ((reg32 *) global_irq__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the global_irq interrupt. */
#define global_irq_INTC_CLR_PD            ((reg32 *) global_irq__INTC_CLR_PD_REG)



#endif /* CY_ISR_global_irq_H */


/* [] END OF FILE */

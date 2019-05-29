/*******************************************************************************
* File Name: pin_wakeup.h  
* Version 2.20
*
* Description:
*  This file contains the Alias definitions for Per-Pin APIs in cypins.h. 
*  Information on using these APIs can be found in the System Reference Guide.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_pin_wakeup_ALIASES_H) /* Pins pin_wakeup_ALIASES_H */
#define CY_PINS_pin_wakeup_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define pin_wakeup_0			(pin_wakeup__0__PC)
#define pin_wakeup_0_PS		(pin_wakeup__0__PS)
#define pin_wakeup_0_PC		(pin_wakeup__0__PC)
#define pin_wakeup_0_DR		(pin_wakeup__0__DR)
#define pin_wakeup_0_SHIFT	(pin_wakeup__0__SHIFT)
#define pin_wakeup_0_INTR	((uint16)((uint16)0x0003u << (pin_wakeup__0__SHIFT*2u)))

#define pin_wakeup_INTR_ALL	 ((uint16)(pin_wakeup_0_INTR))


#endif /* End Pins pin_wakeup_ALIASES_H */


/* [] END OF FILE */

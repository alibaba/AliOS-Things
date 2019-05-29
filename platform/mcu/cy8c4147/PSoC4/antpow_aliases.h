/*******************************************************************************
* File Name: antpow.h  
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

#if !defined(CY_PINS_antpow_ALIASES_H) /* Pins antpow_ALIASES_H */
#define CY_PINS_antpow_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define antpow_0			(antpow__0__PC)
#define antpow_0_PS		(antpow__0__PS)
#define antpow_0_PC		(antpow__0__PC)
#define antpow_0_DR		(antpow__0__DR)
#define antpow_0_SHIFT	(antpow__0__SHIFT)
#define antpow_0_INTR	((uint16)((uint16)0x0003u << (antpow__0__SHIFT*2u)))

#define antpow_INTR_ALL	 ((uint16)(antpow_0_INTR))


#endif /* End Pins antpow_ALIASES_H */


/* [] END OF FILE */

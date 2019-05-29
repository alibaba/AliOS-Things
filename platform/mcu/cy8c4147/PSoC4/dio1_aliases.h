/*******************************************************************************
* File Name: dio1.h  
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

#if !defined(CY_PINS_dio1_ALIASES_H) /* Pins dio1_ALIASES_H */
#define CY_PINS_dio1_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define dio1_0			(dio1__0__PC)
#define dio1_0_PS		(dio1__0__PS)
#define dio1_0_PC		(dio1__0__PC)
#define dio1_0_DR		(dio1__0__DR)
#define dio1_0_SHIFT	(dio1__0__SHIFT)
#define dio1_0_INTR	((uint16)((uint16)0x0003u << (dio1__0__SHIFT*2u)))

#define dio1_INTR_ALL	 ((uint16)(dio1_0_INTR))


#endif /* End Pins dio1_ALIASES_H */


/* [] END OF FILE */

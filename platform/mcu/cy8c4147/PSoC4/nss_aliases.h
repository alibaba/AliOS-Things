/*******************************************************************************
* File Name: nss.h  
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

#if !defined(CY_PINS_nss_ALIASES_H) /* Pins nss_ALIASES_H */
#define CY_PINS_nss_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define nss_0			(nss__0__PC)
#define nss_0_PS		(nss__0__PS)
#define nss_0_PC		(nss__0__PC)
#define nss_0_DR		(nss__0__DR)
#define nss_0_SHIFT	(nss__0__SHIFT)
#define nss_0_INTR	((uint16)((uint16)0x0003u << (nss__0__SHIFT*2u)))

#define nss_INTR_ALL	 ((uint16)(nss_0_INTR))


#endif /* End Pins nss_ALIASES_H */


/* [] END OF FILE */

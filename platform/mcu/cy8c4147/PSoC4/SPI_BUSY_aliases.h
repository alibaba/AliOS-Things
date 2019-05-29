/*******************************************************************************
* File Name: SPI_BUSY.h  
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

#if !defined(CY_PINS_SPI_BUSY_ALIASES_H) /* Pins SPI_BUSY_ALIASES_H */
#define CY_PINS_SPI_BUSY_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define SPI_BUSY_0			(SPI_BUSY__0__PC)
#define SPI_BUSY_0_PS		(SPI_BUSY__0__PS)
#define SPI_BUSY_0_PC		(SPI_BUSY__0__PC)
#define SPI_BUSY_0_DR		(SPI_BUSY__0__DR)
#define SPI_BUSY_0_SHIFT	(SPI_BUSY__0__SHIFT)
#define SPI_BUSY_0_INTR	((uint16)((uint16)0x0003u << (SPI_BUSY__0__SHIFT*2u)))

#define SPI_BUSY_INTR_ALL	 ((uint16)(SPI_BUSY_0_INTR))


#endif /* End Pins SPI_BUSY_ALIASES_H */


/* [] END OF FILE */

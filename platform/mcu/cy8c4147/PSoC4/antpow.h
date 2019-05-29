/*******************************************************************************
* File Name: antpow.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_antpow_H) /* Pins antpow_H */
#define CY_PINS_antpow_H

#include "cytypes.h"
#include "cyfitter.h"
#include "antpow_aliases.h"


/***************************************
*     Data Struct Definitions
***************************************/

/**
* \addtogroup group_structures
* @{
*/
    
/* Structure for sleep mode support */
typedef struct
{
    uint32 pcState; /**< State of the port control register */
    uint32 sioState; /**< State of the SIO configuration */
    uint32 usbState; /**< State of the USBIO regulator */
} antpow_BACKUP_STRUCT;

/** @} structures */


/***************************************
*        Function Prototypes             
***************************************/
/**
* \addtogroup group_general
* @{
*/
uint8   antpow_Read(void);
void    antpow_Write(uint8 value);
uint8   antpow_ReadDataReg(void);
#if defined(antpow__PC) || (CY_PSOC4_4200L) 
    void    antpow_SetDriveMode(uint8 mode);
#endif
void    antpow_SetInterruptMode(uint16 position, uint16 mode);
uint8   antpow_ClearInterrupt(void);
/** @} general */

/**
* \addtogroup group_power
* @{
*/
void antpow_Sleep(void); 
void antpow_Wakeup(void);
/** @} power */


/***************************************
*           API Constants        
***************************************/
#if defined(antpow__PC) || (CY_PSOC4_4200L) 
    /* Drive Modes */
    #define antpow_DRIVE_MODE_BITS        (3)
    #define antpow_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - antpow_DRIVE_MODE_BITS))

    /**
    * \addtogroup group_constants
    * @{
    */
        /** \addtogroup driveMode Drive mode constants
         * \brief Constants to be passed as "mode" parameter in the antpow_SetDriveMode() function.
         *  @{
         */
        #define antpow_DM_ALG_HIZ         (0x00u) /**< \brief High Impedance Analog   */
        #define antpow_DM_DIG_HIZ         (0x01u) /**< \brief High Impedance Digital  */
        #define antpow_DM_RES_UP          (0x02u) /**< \brief Resistive Pull Up       */
        #define antpow_DM_RES_DWN         (0x03u) /**< \brief Resistive Pull Down     */
        #define antpow_DM_OD_LO           (0x04u) /**< \brief Open Drain, Drives Low  */
        #define antpow_DM_OD_HI           (0x05u) /**< \brief Open Drain, Drives High */
        #define antpow_DM_STRONG          (0x06u) /**< \brief Strong Drive            */
        #define antpow_DM_RES_UPDWN       (0x07u) /**< \brief Resistive Pull Up/Down  */
        /** @} driveMode */
    /** @} group_constants */
#endif

/* Digital Port Constants */
#define antpow_MASK               antpow__MASK
#define antpow_SHIFT              antpow__SHIFT
#define antpow_WIDTH              1u

/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in antpow_SetInterruptMode() function.
     *  @{
     */
        #define antpow_INTR_NONE      ((uint16)(0x0000u)) /**< \brief Disabled             */
        #define antpow_INTR_RISING    ((uint16)(0x5555u)) /**< \brief Rising edge trigger  */
        #define antpow_INTR_FALLING   ((uint16)(0xaaaau)) /**< \brief Falling edge trigger */
        #define antpow_INTR_BOTH      ((uint16)(0xffffu)) /**< \brief Both edge trigger    */
    /** @} intrMode */
/** @} group_constants */

/* SIO LPM definition */
#if defined(antpow__SIO)
    #define antpow_SIO_LPM_MASK       (0x03u)
#endif

/* USBIO definitions */
#if !defined(antpow__PC) && (CY_PSOC4_4200L)
    #define antpow_USBIO_ENABLE               ((uint32)0x80000000u)
    #define antpow_USBIO_DISABLE              ((uint32)(~antpow_USBIO_ENABLE))
    #define antpow_USBIO_SUSPEND_SHIFT        CYFLD_USBDEVv2_USB_SUSPEND__OFFSET
    #define antpow_USBIO_SUSPEND_DEL_SHIFT    CYFLD_USBDEVv2_USB_SUSPEND_DEL__OFFSET
    #define antpow_USBIO_ENTER_SLEEP          ((uint32)((1u << antpow_USBIO_SUSPEND_SHIFT) \
                                                        | (1u << antpow_USBIO_SUSPEND_DEL_SHIFT)))
    #define antpow_USBIO_EXIT_SLEEP_PH1       ((uint32)~((uint32)(1u << antpow_USBIO_SUSPEND_SHIFT)))
    #define antpow_USBIO_EXIT_SLEEP_PH2       ((uint32)~((uint32)(1u << antpow_USBIO_SUSPEND_DEL_SHIFT)))
    #define antpow_USBIO_CR1_OFF              ((uint32)0xfffffffeu)
#endif


/***************************************
*             Registers        
***************************************/
/* Main Port Registers */
#if defined(antpow__PC)
    /* Port Configuration */
    #define antpow_PC                 (* (reg32 *) antpow__PC)
#endif
/* Pin State */
#define antpow_PS                     (* (reg32 *) antpow__PS)
/* Data Register */
#define antpow_DR                     (* (reg32 *) antpow__DR)
/* Input Buffer Disable Override */
#define antpow_INP_DIS                (* (reg32 *) antpow__PC2)

/* Interrupt configuration Registers */
#define antpow_INTCFG                 (* (reg32 *) antpow__INTCFG)
#define antpow_INTSTAT                (* (reg32 *) antpow__INTSTAT)

/* "Interrupt cause" register for Combined Port Interrupt (AllPortInt) in GSRef component */
#if defined (CYREG_GPIO_INTR_CAUSE)
    #define antpow_INTR_CAUSE         (* (reg32 *) CYREG_GPIO_INTR_CAUSE)
#endif

/* SIO register */
#if defined(antpow__SIO)
    #define antpow_SIO_REG            (* (reg32 *) antpow__SIO)
#endif /* (antpow__SIO_CFG) */

/* USBIO registers */
#if !defined(antpow__PC) && (CY_PSOC4_4200L)
    #define antpow_USB_POWER_REG       (* (reg32 *) CYREG_USBDEVv2_USB_POWER_CTRL)
    #define antpow_CR1_REG             (* (reg32 *) CYREG_USBDEVv2_CR1)
    #define antpow_USBIO_CTRL_REG      (* (reg32 *) CYREG_USBDEVv2_USB_USBIO_CTRL)
#endif    
    
    
/***************************************
* The following code is DEPRECATED and 
* must not be used in new designs.
***************************************/
/**
* \addtogroup group_deprecated
* @{
*/
#define antpow_DRIVE_MODE_SHIFT       (0x00u)
#define antpow_DRIVE_MODE_MASK        (0x07u << antpow_DRIVE_MODE_SHIFT)
/** @} deprecated */

#endif /* End Pins antpow_H */


/* [] END OF FILE */

/*******************************************************************************
* File Name: nss.h  
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

#if !defined(CY_PINS_nss_H) /* Pins nss_H */
#define CY_PINS_nss_H

#include "cytypes.h"
#include "cyfitter.h"
#include "nss_aliases.h"


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
} nss_BACKUP_STRUCT;

/** @} structures */


/***************************************
*        Function Prototypes             
***************************************/
/**
* \addtogroup group_general
* @{
*/
uint8   nss_Read(void);
void    nss_Write(uint8 value);
uint8   nss_ReadDataReg(void);
#if defined(nss__PC) || (CY_PSOC4_4200L) 
    void    nss_SetDriveMode(uint8 mode);
#endif
void    nss_SetInterruptMode(uint16 position, uint16 mode);
uint8   nss_ClearInterrupt(void);
/** @} general */

/**
* \addtogroup group_power
* @{
*/
void nss_Sleep(void); 
void nss_Wakeup(void);
/** @} power */


/***************************************
*           API Constants        
***************************************/
#if defined(nss__PC) || (CY_PSOC4_4200L) 
    /* Drive Modes */
    #define nss_DRIVE_MODE_BITS        (3)
    #define nss_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - nss_DRIVE_MODE_BITS))

    /**
    * \addtogroup group_constants
    * @{
    */
        /** \addtogroup driveMode Drive mode constants
         * \brief Constants to be passed as "mode" parameter in the nss_SetDriveMode() function.
         *  @{
         */
        #define nss_DM_ALG_HIZ         (0x00u) /**< \brief High Impedance Analog   */
        #define nss_DM_DIG_HIZ         (0x01u) /**< \brief High Impedance Digital  */
        #define nss_DM_RES_UP          (0x02u) /**< \brief Resistive Pull Up       */
        #define nss_DM_RES_DWN         (0x03u) /**< \brief Resistive Pull Down     */
        #define nss_DM_OD_LO           (0x04u) /**< \brief Open Drain, Drives Low  */
        #define nss_DM_OD_HI           (0x05u) /**< \brief Open Drain, Drives High */
        #define nss_DM_STRONG          (0x06u) /**< \brief Strong Drive            */
        #define nss_DM_RES_UPDWN       (0x07u) /**< \brief Resistive Pull Up/Down  */
        /** @} driveMode */
    /** @} group_constants */
#endif

/* Digital Port Constants */
#define nss_MASK               nss__MASK
#define nss_SHIFT              nss__SHIFT
#define nss_WIDTH              1u

/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in nss_SetInterruptMode() function.
     *  @{
     */
        #define nss_INTR_NONE      ((uint16)(0x0000u)) /**< \brief Disabled             */
        #define nss_INTR_RISING    ((uint16)(0x5555u)) /**< \brief Rising edge trigger  */
        #define nss_INTR_FALLING   ((uint16)(0xaaaau)) /**< \brief Falling edge trigger */
        #define nss_INTR_BOTH      ((uint16)(0xffffu)) /**< \brief Both edge trigger    */
    /** @} intrMode */
/** @} group_constants */

/* SIO LPM definition */
#if defined(nss__SIO)
    #define nss_SIO_LPM_MASK       (0x03u)
#endif

/* USBIO definitions */
#if !defined(nss__PC) && (CY_PSOC4_4200L)
    #define nss_USBIO_ENABLE               ((uint32)0x80000000u)
    #define nss_USBIO_DISABLE              ((uint32)(~nss_USBIO_ENABLE))
    #define nss_USBIO_SUSPEND_SHIFT        CYFLD_USBDEVv2_USB_SUSPEND__OFFSET
    #define nss_USBIO_SUSPEND_DEL_SHIFT    CYFLD_USBDEVv2_USB_SUSPEND_DEL__OFFSET
    #define nss_USBIO_ENTER_SLEEP          ((uint32)((1u << nss_USBIO_SUSPEND_SHIFT) \
                                                        | (1u << nss_USBIO_SUSPEND_DEL_SHIFT)))
    #define nss_USBIO_EXIT_SLEEP_PH1       ((uint32)~((uint32)(1u << nss_USBIO_SUSPEND_SHIFT)))
    #define nss_USBIO_EXIT_SLEEP_PH2       ((uint32)~((uint32)(1u << nss_USBIO_SUSPEND_DEL_SHIFT)))
    #define nss_USBIO_CR1_OFF              ((uint32)0xfffffffeu)
#endif


/***************************************
*             Registers        
***************************************/
/* Main Port Registers */
#if defined(nss__PC)
    /* Port Configuration */
    #define nss_PC                 (* (reg32 *) nss__PC)
#endif
/* Pin State */
#define nss_PS                     (* (reg32 *) nss__PS)
/* Data Register */
#define nss_DR                     (* (reg32 *) nss__DR)
/* Input Buffer Disable Override */
#define nss_INP_DIS                (* (reg32 *) nss__PC2)

/* Interrupt configuration Registers */
#define nss_INTCFG                 (* (reg32 *) nss__INTCFG)
#define nss_INTSTAT                (* (reg32 *) nss__INTSTAT)

/* "Interrupt cause" register for Combined Port Interrupt (AllPortInt) in GSRef component */
#if defined (CYREG_GPIO_INTR_CAUSE)
    #define nss_INTR_CAUSE         (* (reg32 *) CYREG_GPIO_INTR_CAUSE)
#endif

/* SIO register */
#if defined(nss__SIO)
    #define nss_SIO_REG            (* (reg32 *) nss__SIO)
#endif /* (nss__SIO_CFG) */

/* USBIO registers */
#if !defined(nss__PC) && (CY_PSOC4_4200L)
    #define nss_USB_POWER_REG       (* (reg32 *) CYREG_USBDEVv2_USB_POWER_CTRL)
    #define nss_CR1_REG             (* (reg32 *) CYREG_USBDEVv2_CR1)
    #define nss_USBIO_CTRL_REG      (* (reg32 *) CYREG_USBDEVv2_USB_USBIO_CTRL)
#endif    
    
    
/***************************************
* The following code is DEPRECATED and 
* must not be used in new designs.
***************************************/
/**
* \addtogroup group_deprecated
* @{
*/
#define nss_DRIVE_MODE_SHIFT       (0x00u)
#define nss_DRIVE_MODE_MASK        (0x07u << nss_DRIVE_MODE_SHIFT)
/** @} deprecated */

#endif /* End Pins nss_H */


/* [] END OF FILE */

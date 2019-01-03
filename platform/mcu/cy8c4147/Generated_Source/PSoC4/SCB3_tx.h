/*******************************************************************************
* File Name: SCB3_tx.h  
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

#if !defined(CY_PINS_SCB3_tx_H) /* Pins SCB3_tx_H */
#define CY_PINS_SCB3_tx_H

#include "cytypes.h"
#include "cyfitter.h"
#include "SCB3_tx_aliases.h"


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
} SCB3_tx_BACKUP_STRUCT;

/** @} structures */


/***************************************
*        Function Prototypes             
***************************************/
/**
* \addtogroup group_general
* @{
*/
uint8   SCB3_tx_Read(void);
void    SCB3_tx_Write(uint8 value);
uint8   SCB3_tx_ReadDataReg(void);
#if defined(SCB3_tx__PC) || (CY_PSOC4_4200L) 
    void    SCB3_tx_SetDriveMode(uint8 mode);
#endif
void    SCB3_tx_SetInterruptMode(uint16 position, uint16 mode);
uint8   SCB3_tx_ClearInterrupt(void);
/** @} general */

/**
* \addtogroup group_power
* @{
*/
void SCB3_tx_Sleep(void); 
void SCB3_tx_Wakeup(void);
/** @} power */


/***************************************
*           API Constants        
***************************************/
#if defined(SCB3_tx__PC) || (CY_PSOC4_4200L) 
    /* Drive Modes */
    #define SCB3_tx_DRIVE_MODE_BITS        (3)
    #define SCB3_tx_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - SCB3_tx_DRIVE_MODE_BITS))

    /**
    * \addtogroup group_constants
    * @{
    */
        /** \addtogroup driveMode Drive mode constants
         * \brief Constants to be passed as "mode" parameter in the SCB3_tx_SetDriveMode() function.
         *  @{
         */
        #define SCB3_tx_DM_ALG_HIZ         (0x00u) /**< \brief High Impedance Analog   */
        #define SCB3_tx_DM_DIG_HIZ         (0x01u) /**< \brief High Impedance Digital  */
        #define SCB3_tx_DM_RES_UP          (0x02u) /**< \brief Resistive Pull Up       */
        #define SCB3_tx_DM_RES_DWN         (0x03u) /**< \brief Resistive Pull Down     */
        #define SCB3_tx_DM_OD_LO           (0x04u) /**< \brief Open Drain, Drives Low  */
        #define SCB3_tx_DM_OD_HI           (0x05u) /**< \brief Open Drain, Drives High */
        #define SCB3_tx_DM_STRONG          (0x06u) /**< \brief Strong Drive            */
        #define SCB3_tx_DM_RES_UPDWN       (0x07u) /**< \brief Resistive Pull Up/Down  */
        /** @} driveMode */
    /** @} group_constants */
#endif

/* Digital Port Constants */
#define SCB3_tx_MASK               SCB3_tx__MASK
#define SCB3_tx_SHIFT              SCB3_tx__SHIFT
#define SCB3_tx_WIDTH              1u

/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in SCB3_tx_SetInterruptMode() function.
     *  @{
     */
        #define SCB3_tx_INTR_NONE      ((uint16)(0x0000u)) /**< \brief Disabled             */
        #define SCB3_tx_INTR_RISING    ((uint16)(0x5555u)) /**< \brief Rising edge trigger  */
        #define SCB3_tx_INTR_FALLING   ((uint16)(0xaaaau)) /**< \brief Falling edge trigger */
        #define SCB3_tx_INTR_BOTH      ((uint16)(0xffffu)) /**< \brief Both edge trigger    */
    /** @} intrMode */
/** @} group_constants */

/* SIO LPM definition */
#if defined(SCB3_tx__SIO)
    #define SCB3_tx_SIO_LPM_MASK       (0x03u)
#endif

/* USBIO definitions */
#if !defined(SCB3_tx__PC) && (CY_PSOC4_4200L)
    #define SCB3_tx_USBIO_ENABLE               ((uint32)0x80000000u)
    #define SCB3_tx_USBIO_DISABLE              ((uint32)(~SCB3_tx_USBIO_ENABLE))
    #define SCB3_tx_USBIO_SUSPEND_SHIFT        CYFLD_USBDEVv2_USB_SUSPEND__OFFSET
    #define SCB3_tx_USBIO_SUSPEND_DEL_SHIFT    CYFLD_USBDEVv2_USB_SUSPEND_DEL__OFFSET
    #define SCB3_tx_USBIO_ENTER_SLEEP          ((uint32)((1u << SCB3_tx_USBIO_SUSPEND_SHIFT) \
                                                        | (1u << SCB3_tx_USBIO_SUSPEND_DEL_SHIFT)))
    #define SCB3_tx_USBIO_EXIT_SLEEP_PH1       ((uint32)~((uint32)(1u << SCB3_tx_USBIO_SUSPEND_SHIFT)))
    #define SCB3_tx_USBIO_EXIT_SLEEP_PH2       ((uint32)~((uint32)(1u << SCB3_tx_USBIO_SUSPEND_DEL_SHIFT)))
    #define SCB3_tx_USBIO_CR1_OFF              ((uint32)0xfffffffeu)
#endif


/***************************************
*             Registers        
***************************************/
/* Main Port Registers */
#if defined(SCB3_tx__PC)
    /* Port Configuration */
    #define SCB3_tx_PC                 (* (reg32 *) SCB3_tx__PC)
#endif
/* Pin State */
#define SCB3_tx_PS                     (* (reg32 *) SCB3_tx__PS)
/* Data Register */
#define SCB3_tx_DR                     (* (reg32 *) SCB3_tx__DR)
/* Input Buffer Disable Override */
#define SCB3_tx_INP_DIS                (* (reg32 *) SCB3_tx__PC2)

/* Interrupt configuration Registers */
#define SCB3_tx_INTCFG                 (* (reg32 *) SCB3_tx__INTCFG)
#define SCB3_tx_INTSTAT                (* (reg32 *) SCB3_tx__INTSTAT)

/* "Interrupt cause" register for Combined Port Interrupt (AllPortInt) in GSRef component */
#if defined (CYREG_GPIO_INTR_CAUSE)
    #define SCB3_tx_INTR_CAUSE         (* (reg32 *) CYREG_GPIO_INTR_CAUSE)
#endif

/* SIO register */
#if defined(SCB3_tx__SIO)
    #define SCB3_tx_SIO_REG            (* (reg32 *) SCB3_tx__SIO)
#endif /* (SCB3_tx__SIO_CFG) */

/* USBIO registers */
#if !defined(SCB3_tx__PC) && (CY_PSOC4_4200L)
    #define SCB3_tx_USB_POWER_REG       (* (reg32 *) CYREG_USBDEVv2_USB_POWER_CTRL)
    #define SCB3_tx_CR1_REG             (* (reg32 *) CYREG_USBDEVv2_CR1)
    #define SCB3_tx_USBIO_CTRL_REG      (* (reg32 *) CYREG_USBDEVv2_USB_USBIO_CTRL)
#endif    
    
    
/***************************************
* The following code is DEPRECATED and 
* must not be used in new designs.
***************************************/
/**
* \addtogroup group_deprecated
* @{
*/
#define SCB3_tx_DRIVE_MODE_SHIFT       (0x00u)
#define SCB3_tx_DRIVE_MODE_MASK        (0x07u << SCB3_tx_DRIVE_MODE_SHIFT)
/** @} deprecated */

#endif /* End Pins SCB3_tx_H */


/* [] END OF FILE */

/*******************************************************************************
* File Name: SPI_BUSY.h  
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

#if !defined(CY_PINS_SPI_BUSY_H) /* Pins SPI_BUSY_H */
#define CY_PINS_SPI_BUSY_H

#include "cytypes.h"
#include "cyfitter.h"
#include "SPI_BUSY_aliases.h"


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
} SPI_BUSY_BACKUP_STRUCT;

/** @} structures */


/***************************************
*        Function Prototypes             
***************************************/
/**
* \addtogroup group_general
* @{
*/
uint8   SPI_BUSY_Read(void);
void    SPI_BUSY_Write(uint8 value);
uint8   SPI_BUSY_ReadDataReg(void);
#if defined(SPI_BUSY__PC) || (CY_PSOC4_4200L) 
    void    SPI_BUSY_SetDriveMode(uint8 mode);
#endif
void    SPI_BUSY_SetInterruptMode(uint16 position, uint16 mode);
uint8   SPI_BUSY_ClearInterrupt(void);
/** @} general */

/**
* \addtogroup group_power
* @{
*/
void SPI_BUSY_Sleep(void); 
void SPI_BUSY_Wakeup(void);
/** @} power */


/***************************************
*           API Constants        
***************************************/
#if defined(SPI_BUSY__PC) || (CY_PSOC4_4200L) 
    /* Drive Modes */
    #define SPI_BUSY_DRIVE_MODE_BITS        (3)
    #define SPI_BUSY_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - SPI_BUSY_DRIVE_MODE_BITS))

    /**
    * \addtogroup group_constants
    * @{
    */
        /** \addtogroup driveMode Drive mode constants
         * \brief Constants to be passed as "mode" parameter in the SPI_BUSY_SetDriveMode() function.
         *  @{
         */
        #define SPI_BUSY_DM_ALG_HIZ         (0x00u) /**< \brief High Impedance Analog   */
        #define SPI_BUSY_DM_DIG_HIZ         (0x01u) /**< \brief High Impedance Digital  */
        #define SPI_BUSY_DM_RES_UP          (0x02u) /**< \brief Resistive Pull Up       */
        #define SPI_BUSY_DM_RES_DWN         (0x03u) /**< \brief Resistive Pull Down     */
        #define SPI_BUSY_DM_OD_LO           (0x04u) /**< \brief Open Drain, Drives Low  */
        #define SPI_BUSY_DM_OD_HI           (0x05u) /**< \brief Open Drain, Drives High */
        #define SPI_BUSY_DM_STRONG          (0x06u) /**< \brief Strong Drive            */
        #define SPI_BUSY_DM_RES_UPDWN       (0x07u) /**< \brief Resistive Pull Up/Down  */
        /** @} driveMode */
    /** @} group_constants */
#endif

/* Digital Port Constants */
#define SPI_BUSY_MASK               SPI_BUSY__MASK
#define SPI_BUSY_SHIFT              SPI_BUSY__SHIFT
#define SPI_BUSY_WIDTH              1u

/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in SPI_BUSY_SetInterruptMode() function.
     *  @{
     */
        #define SPI_BUSY_INTR_NONE      ((uint16)(0x0000u)) /**< \brief Disabled             */
        #define SPI_BUSY_INTR_RISING    ((uint16)(0x5555u)) /**< \brief Rising edge trigger  */
        #define SPI_BUSY_INTR_FALLING   ((uint16)(0xaaaau)) /**< \brief Falling edge trigger */
        #define SPI_BUSY_INTR_BOTH      ((uint16)(0xffffu)) /**< \brief Both edge trigger    */
    /** @} intrMode */
/** @} group_constants */

/* SIO LPM definition */
#if defined(SPI_BUSY__SIO)
    #define SPI_BUSY_SIO_LPM_MASK       (0x03u)
#endif

/* USBIO definitions */
#if !defined(SPI_BUSY__PC) && (CY_PSOC4_4200L)
    #define SPI_BUSY_USBIO_ENABLE               ((uint32)0x80000000u)
    #define SPI_BUSY_USBIO_DISABLE              ((uint32)(~SPI_BUSY_USBIO_ENABLE))
    #define SPI_BUSY_USBIO_SUSPEND_SHIFT        CYFLD_USBDEVv2_USB_SUSPEND__OFFSET
    #define SPI_BUSY_USBIO_SUSPEND_DEL_SHIFT    CYFLD_USBDEVv2_USB_SUSPEND_DEL__OFFSET
    #define SPI_BUSY_USBIO_ENTER_SLEEP          ((uint32)((1u << SPI_BUSY_USBIO_SUSPEND_SHIFT) \
                                                        | (1u << SPI_BUSY_USBIO_SUSPEND_DEL_SHIFT)))
    #define SPI_BUSY_USBIO_EXIT_SLEEP_PH1       ((uint32)~((uint32)(1u << SPI_BUSY_USBIO_SUSPEND_SHIFT)))
    #define SPI_BUSY_USBIO_EXIT_SLEEP_PH2       ((uint32)~((uint32)(1u << SPI_BUSY_USBIO_SUSPEND_DEL_SHIFT)))
    #define SPI_BUSY_USBIO_CR1_OFF              ((uint32)0xfffffffeu)
#endif


/***************************************
*             Registers        
***************************************/
/* Main Port Registers */
#if defined(SPI_BUSY__PC)
    /* Port Configuration */
    #define SPI_BUSY_PC                 (* (reg32 *) SPI_BUSY__PC)
#endif
/* Pin State */
#define SPI_BUSY_PS                     (* (reg32 *) SPI_BUSY__PS)
/* Data Register */
#define SPI_BUSY_DR                     (* (reg32 *) SPI_BUSY__DR)
/* Input Buffer Disable Override */
#define SPI_BUSY_INP_DIS                (* (reg32 *) SPI_BUSY__PC2)

/* Interrupt configuration Registers */
#define SPI_BUSY_INTCFG                 (* (reg32 *) SPI_BUSY__INTCFG)
#define SPI_BUSY_INTSTAT                (* (reg32 *) SPI_BUSY__INTSTAT)

/* "Interrupt cause" register for Combined Port Interrupt (AllPortInt) in GSRef component */
#if defined (CYREG_GPIO_INTR_CAUSE)
    #define SPI_BUSY_INTR_CAUSE         (* (reg32 *) CYREG_GPIO_INTR_CAUSE)
#endif

/* SIO register */
#if defined(SPI_BUSY__SIO)
    #define SPI_BUSY_SIO_REG            (* (reg32 *) SPI_BUSY__SIO)
#endif /* (SPI_BUSY__SIO_CFG) */

/* USBIO registers */
#if !defined(SPI_BUSY__PC) && (CY_PSOC4_4200L)
    #define SPI_BUSY_USB_POWER_REG       (* (reg32 *) CYREG_USBDEVv2_USB_POWER_CTRL)
    #define SPI_BUSY_CR1_REG             (* (reg32 *) CYREG_USBDEVv2_CR1)
    #define SPI_BUSY_USBIO_CTRL_REG      (* (reg32 *) CYREG_USBDEVv2_USB_USBIO_CTRL)
#endif    
    
    
/***************************************
* The following code is DEPRECATED and 
* must not be used in new designs.
***************************************/
/**
* \addtogroup group_deprecated
* @{
*/
#define SPI_BUSY_DRIVE_MODE_SHIFT       (0x00u)
#define SPI_BUSY_DRIVE_MODE_MASK        (0x07u << SPI_BUSY_DRIVE_MODE_SHIFT)
/** @} deprecated */

#endif /* End Pins SPI_BUSY_H */


/* [] END OF FILE */

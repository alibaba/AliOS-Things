/*******************************************************************************
* \file cypins.h
* \version 5.70
*
* \brief This file contains the function prototypes and constants used for
* port/pin in access and control.
*
* \note Documentation of the API's in this file is located in the System
* Reference Guide provided with PSoC Creator.
*
********************************************************************************
* \copyright
* Copyright 2008-2018, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_BOOT_CYPINS_H)
#define CY_BOOT_CYPINS_H

#include "cytypes.h"

/**
* \addtogroup group_pins Pins
* \brief For PSoC 4, there are status registers, data output registers, and port
configuration registers only, so the macro takes two arguments: port register
and pin number. Each port has these registers addresses defined:
CYREG_PRTx_DR
CYREG_PRTx_PS
CYREG_PRTx_PC

The x is the port number, and the second argument is the pin number.

* @{
*/

/** @} group_pins */


/**************************************
*       Register Constants
**************************************/


#define CY_SYS_PINS_PC_DATAOUT                 ((uint32) 0x01u)
#define CY_SYS_PINS_PC_DRIVE_MODE_BITS         ((uint32) 0x03u)
#define CY_SYS_PINS_PC_DRIVE_MODE_MASK         ((uint32) 0x07u)


/**************************************
*        API Parameter Constants
**************************************/

/*  SetPinDriveMode */
#define CY_SYS_PINS_DM_ALG_HIZ                ((uint32) 0x00u)
#define CY_SYS_PINS_DM_DIG_HIZ                ((uint32) 0x01u)
#define CY_SYS_PINS_DM_RES_UP                 ((uint32) 0x02u)
#define CY_SYS_PINS_DM_RES_DWN                ((uint32) 0x03u)
#define CY_SYS_PINS_DM_OD_LO                  ((uint32) 0x04u)
#define CY_SYS_PINS_DM_OD_HI                  ((uint32) 0x05u)
#define CY_SYS_PINS_DM_STRONG                 ((uint32) 0x06u)
#define CY_SYS_PINS_DM_RES_UPDWN              ((uint32) 0x07u)


/**************************************
*        Compatibility Macros
**************************************/

#if(CY_IP_HOBTO_DEVICE)
    #define CYREG_PRT0_DR                     (CYREG_GPIO_PRT0_DR)
    #define CYREG_PRT0_PS                     (CYREG_GPIO_PRT0_PS)
    #define CYREG_PRT0_PC                     (CYREG_GPIO_PRT0_PC)

    #define CYREG_PRT1_DR                     (CYREG_GPIO_PRT1_DR)
    #define CYREG_PRT1_PS                     (CYREG_GPIO_PRT1_PS)
    #define CYREG_PRT1_PC                     (CYREG_GPIO_PRT1_PC)

    #define CYREG_PRT2_DR                     (CYREG_GPIO_PRT2_DR)
    #define CYREG_PRT2_PS                     (CYREG_GPIO_PRT2_PS)
    #define CYREG_PRT2_PC                     (CYREG_GPIO_PRT2_PC)

    #define CYREG_PRT3_DR                     (CYREG_GPIO_PRT3_DR)
    #define CYREG_PRT3_PS                     (CYREG_GPIO_PRT3_PS)
    #define CYREG_PRT3_PC                     (CYREG_GPIO_PRT3_PC)

    #define CYREG_PRT4_DR                     (CYREG_GPIO_PRT4_DR)
    #define CYREG_PRT4_PS                     (CYREG_GPIO_PRT4_PS)
    #define CYREG_PRT4_PC                     (CYREG_GPIO_PRT4_PC)

    #define CYREG_PRT5_DR                     (CYREG_GPIO_PRT5_DR)
    #define CYREG_PRT5_PS                     (CYREG_GPIO_PRT5_PS)
    #define CYREG_PRT5_PC                     (CYREG_GPIO_PRT5_PC)

    #define CYREG_PRT6_DR                     (CYREG_GPIO_PRT6_DR)
    #define CYREG_PRT6_PS                     (CYREG_GPIO_PRT6_PS)
    #define CYREG_PRT6_PC                     (CYREG_GPIO_PRT6_PC)

    #define CYREG_PRT7_DR                     (CYREG_GPIO_PRT7_DR)
    #define CYREG_PRT7_PS                     (CYREG_GPIO_PRT7_PS)
    #define CYREG_PRT7_PC                     (CYREG_GPIO_PRT7_PC)

    #define CYREG_PRT8_DR                     (CYREG_GPIO_PRT8_DR)
    #define CYREG_PRT8_PS                     (CYREG_GPIO_PRT8_PS)
    #define CYREG_PRT8_PC                     (CYREG_GPIO_PRT8_PC)

    #define CYREG_PRT9_DR                     (CYREG_GPIO_PRT9_DR)
    #define CYREG_PRT9_PS                     (CYREG_GPIO_PRT9_PS)
    #define CYREG_PRT9_PC                     (CYREG_GPIO_PRT9_PC)

    #define CYREG_PRT10_DR                    (CYREG_GPIO_PRT10_DR)
    #define CYREG_PRT10_PS                    (CYREG_GPIO_PRT10_PS)
    #define CYREG_PRT10_PC                    (CYREG_GPIO_PRT10_PC)

    #define CYREG_PRT11_DR                    (CYREG_GPIO_PRT11_DR)
    #define CYREG_PRT11_PS                    (CYREG_GPIO_PRT11_PS)
    #define CYREG_PRT11_PC                    (CYREG_GPIO_PRT11_PC)

    #define CYREG_PRT12_DR                    (CYREG_GPIO_PRT12_DR)
    #define CYREG_PRT12_PS                    (CYREG_GPIO_PRT12_PS)
    #define CYREG_PRT12_PC                    (CYREG_GPIO_PRT12_PC)

    #define CYREG_PRT13_DR                    (CYREG_GPIO_PRT13_DR)
    #define CYREG_PRT13_PS                    (CYREG_GPIO_PRT13_PS)
    #define CYREG_PRT13_PC                    (CYREG_GPIO_PRT13_PC)

    #define CYREG_PRT14_DR                    (CYREG_GPIO_PRT14_DR)
    #define CYREG_PRT14_PS                    (CYREG_GPIO_PRT14_PS)
    #define CYREG_PRT14_PC                    (CYREG_GPIO_PRT14_PC)

    #define CYREG_PRT15_DR                    (CYREG_GPIO_PRT15_DR)
    #define CYREG_PRT15_PS                    (CYREG_GPIO_PRT15_PS)
    #define CYREG_PRT15_PC                    (CYREG_GPIO_PRT15_PC)

#else

    #define CYREG_GPIO_PRT0_DR                (CYREG_PRT0_DR)
    #define CYREG_GPIO_PRT0_PS                (CYREG_PRT0_PS)
    #define CYREG_GPIO_PRT0_PC                (CYREG_PRT0_PC)

    #define CYREG_GPIO_PRT1_DR                (CYREG_PRT1_DR)
    #define CYREG_GPIO_PRT1_PS                (CYREG_PRT1_PS)
    #define CYREG_GPIO_PRT1_PC                (CYREG_PRT1_PC)

    #define CYREG_GPIO_PRT2_DR                (CYREG_PRT2_DR)
    #define CYREG_GPIO_PRT2_PS                (CYREG_PRT2_PS)
    #define CYREG_GPIO_PRT2_PC                (CYREG_PRT2_PC)

    #define CYREG_GPIO_PRT3_DR                (CYREG_PRT3_DR)
    #define CYREG_GPIO_PRT3_PS                (CYREG_PRT3_PS)
    #define CYREG_GPIO_PRT3_PC                (CYREG_PRT3_PC)

    #define CYREG_GPIO_PRT4_DR                (CYREG_PRT4_DR)
    #define CYREG_GPIO_PRT4_PS                (CYREG_PRT4_PS)
    #define CYREG_GPIO_PRT4_PC                (CYREG_PRT4_PC)
#endif /* (CY_IP_HOBTO_DEVICE) */


/**************************************
*       Pin API Macros
**************************************/

/**
* \defgroup group_pins Pins
* @{
*/

/*******************************************************************************
* Macro Name: CY_SYS_PINS_READ_PIN
****************************************************************************//**
*
* Reads the current value on the pin (pin state, PS).
*
* \param portPS Address of the port pin status register (uint32). Definitions
* for each port are provided in the cydevice_trm.h file  in the form:
* CYREG_GPIO_PRTx_PS, where x is a port number. The actual number depends on the
* selected device.
*
* \param pin The pin number 0 - 7. The actual number depends on the selected
* device.
*
* \return Zero - logic low, non-zero - logic high.
*
*******************************************************************************/
#define CY_SYS_PINS_READ_PIN(portPS, pin) \
            (( *(reg32 *)(portPS) >> (pin)) & CY_SYS_PINS_PC_DATAOUT)


/*******************************************************************************
* Macro Name: CY_SYS_PINS_SET_PIN
****************************************************************************//**
*
* Set the output value for the pin (data register, DR) to a logic high.
* Note that this only has an effect for pins configured as software pins that
* are not driven by hardware.
*
* The macro operation is not atomic. It is not guaranteed that shared register
* will remain uncorrupted during simultaneous read-modify-write operations
* performed by two threads (main and interrupt threads). To guarantee data
* integrity in such cases, the macro should be invoked while the specific
* interrupt is disabled or within critical section (all interrupts are
* disabled).
*
* \param portDR Address of the port output pin data register (uint32).
* Definitions for each port are provided in the cydevice_trm.h file  in the
* form: CYREG_GPIO_PRTx_PS, where x is a port number. The actual number depends
* on the selected device.
*
* \param pin The pin number 0 - 7. The actual number depends on the selected
* device.
*
*******************************************************************************/
#define CY_SYS_PINS_SET_PIN(portDR, pin)     \
            ( *(reg32 *)(portDR) |= (CY_SYS_PINS_PC_DATAOUT << (pin)) )


/*******************************************************************************
* Macro Name: CY_SYS_PINS_CLEAR_PIN
****************************************************************************//**
*
* This macro sets the state of the specified pin to zero.
*
* The macro operation is not atomic. It is not guaranteed that shared register
* will remain uncorrupted during simultaneous read-modify-write operations
* performed by two threads (main and interrupt threads). To guarantee data
* integrity in such cases, the macro should be invoked while the specific
* interrupt is disabled or within critical section (all interrupts are
* disabled).
*
* \param portDR Address of the port output pin data register (uint32).
* Definitions for each port are provided in the cydevice_trm.h file in the
* form: CYREG_GPIO_PRTx_PS, where x is a port number. The actual number
* depends on the selected device.
*
* \param pin The pin number 0 - 7. The actual number depends on the selected device.
*
*******************************************************************************/
#define CY_SYS_PINS_CLEAR_PIN(portDR, pin)   \
            ( *(reg32 *)(portDR) &= ~(CY_SYS_PINS_PC_DATAOUT << (pin)) )


/*******************************************************************************
* Macro Name: CY_SYS_PINS_SET_DRIVE_MODE
****************************************************************************//**
*
* Sets the drive mode for the pin (DM).
*
* The macro operation is not atomic. It is not guaranteed that shared register
* will remain uncorrupted during simultaneous read-modify-write operations
* performed by two threads (main and interrupt threads). To guarantee data
* integrity in such cases, the macro should be invoked while the specific
* interrupt is disabled or within critical section (all interrupts are
* disabled).
*
* \param portPC: Address of the port configuration register (uint32).
* Definitions for each port are provided in the cydevice_trm.h file  in the
* form: CYREG_GPIO_PRTx_PS, where x is a port number. The actual number
* depends on the selected device.
*
* \param pin The pin number 0 - 7. The actual number depends on the selected device.
*
* \param mode Desired drive mode.
*
*        Define                        Source
*   CY_SYS_PINS_DM_ALG_HIZ        Analog HiZ
*   CY_SYS_PINS_DM_DIG_HIZ        Digital HiZ
*   CY_SYS_PINS_DM_RES_UP         Resistive pull up
*   CY_SYS_PINS_DM_RES_DWN        Resistive pull down
*   CY_SYS_PINS_DM_OD_LO          Open drain - drive low
*   CY_SYS_PINS_DM_OD_HI          Open drain - drive high
*   CY_SYS_PINS_DM_STRONG         Strong CMOS Output
*   CY_SYS_PINS_DM_RES_UPDWN      Resistive pull up/down
*
*******************************************************************************/
#define CY_SYS_PINS_SET_DRIVE_MODE(portPC, pin, mode)    \
            ( *(reg32 *)(portPC) = (*(reg32 *)(portPC) & \
            ~(CY_SYS_PINS_PC_DRIVE_MODE_MASK << ((pin) * CY_SYS_PINS_PC_DRIVE_MODE_BITS))) | \
            ((mode) << ((pin) * CY_SYS_PINS_PC_DRIVE_MODE_BITS)))


/*******************************************************************************
* Macro Name: CY_SYS_PINS_READ_DRIVE_MODE
****************************************************************************//**
*
* Reads the drive mode for the pin (DM).
*
* \param portPC Address of the port configuration register (uint32). Definitions
* for each port are provided in the cydevice_trm.h file in the form:
* CYREG_GPIO_PRTx_PS, where x is a port number. The actual number depends on the
* selected device.
*
* \param pin The pin number 0 - 7. The actual number depends on the selected
* device.
*
* \return mode Current drive mode for the pin:
* - CY_SYS_PINS_DM_ALG_HIZ        Analog HiZ
* - CY_SYS_PINS_DM_DIG_HIZ        Digital HiZ
* - CY_SYS_PINS_DM_RES_UP         Resistive pull up
* - CY_SYS_PINS_DM_RES_DWN        Resistive pull down
* - CY_SYS_PINS_DM_OD_LO          Open drain - drive low
* - CY_SYS_PINS_DM_OD_HI          Open drain - drive high
* - CY_SYS_PINS_DM_STRONG         Strong CMOS Output
* - CY_SYS_PINS_DM_RES_UPDWN      Resistive pull up/down
*
*******************************************************************************/
#define CY_SYS_PINS_READ_DRIVE_MODE(portPC, pin)     \
        (( *(reg32 *)(portPC) & \
        (CY_SYS_PINS_PC_DRIVE_MODE_MASK << ((pin) * CY_SYS_PINS_PC_DRIVE_MODE_BITS)) ) >> \
        (pin) * CY_SYS_PINS_PC_DRIVE_MODE_BITS)

/** @} group_pins */

/* Defines function macros for mapping PSoC 4 per-pin functions to PSoC 3/5LP style functions  */
#define CyPins_ReadPin(name)                (CY_SYS_PINS_READ_PIN       (name ## _PS, name ## _SHIFT))
#define CyPins_SetPin(name)                 (CY_SYS_PINS_SET_PIN        (name ## _DR, name ## _SHIFT))
#define CyPins_ClearPin(name)               (CY_SYS_PINS_CLEAR_PIN      (name ## _DR, name ## _SHIFT))
#define CyPins_SetPinDriveMode(name, mode)  (CY_SYS_PINS_SET_DRIVE_MODE (name ## _PC, name ## _SHIFT, mode))
#define CyPins_ReadPinDriveMode(name)       (CY_SYS_PINS_READ_DRIVE_MODE(name ## _PC, name ## _SHIFT))


#endif /* (CY_BOOT_CYPINS_H) */


/* [] END OF FILE */

/*******************************************************************************
  Ethernet PHY Driver Configuration Definitions for the Template Version

  Company:
    Microchip Technology Inc.

  File Name:
    drv_ethphy_config.h

  Summary:
    Ethernet PHY driver configuration definitions template.

  Description:
    These definitions statically define the driver's mode of operation.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
//DOM-IGNORE-END

#ifndef _DRV_ETHPHY_CONFIG_H
#define _DRV_ETHPHY_CONFIG_H

// *****************************************************************************
// *****************************************************************************
// Section: Initialization Overrides
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Ethernet PHY Negotiation Initiation time out

  Summary:
    Value of the PHY negotiation initiation time out as per IEEE 802.3 spec.

  Description:
    This definition sets the time out of the PHY negotiation initiation, in ms.

  Remarks:
    None.
*/

#define DRV_ETHPHY_NEG_INIT_TMO   (1)

// *****************************************************************************
/* Ethernet PHY Negotiation Complete time out

  Summary:
    Value of the PHY negotiation complete time out as per IEEE 802.3 spec.

  Description:
    This definition sets the time out of the PHY negotiation complete, in ms.

  Remarks:
    See IEEE 802.3 Clause 28 Table 28-9 autoneg_wait_timer value (max 1s).
*/

#define DRV_ETHPHY_NEG_DONE_TMO  (2000)

// *****************************************************************************
/* Ethernet PHY Reset self clear time out

  Summary:
    Value of the PHY Reset self clear time out as per IEEE 802.3 spec.

  Description:
    This definition sets the time out of the PHY Reset self clear, in ms.

  Remarks:
    See IEEE 802.3 Clause 22 Table 22-7 and paragraph "22.2.4.1.1 Reset" (max 0.5s)
*/

#define DRV_ETHPHY_RESET_CLR_TMO   (500)

// *****************************************************************************
// *****************************************************************************
// Section: Ethernet PHY Configuration
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Ethernet PHY hardware instance configuration

  Summary:
    Selects the maximum number of hardware instances that can be supported by
    the dynamic driver.

  Description:
    This definition selects the maximum number of hardware instances that can be
    supported by the dynamic driver. Not defining it means using a static driver.

  Remarks:
    None.
*/

#define DRV_ETHPHY_INSTANCES_NUMBER                1


/*************************************************************************
  Summary:
    Selects the maximum number of clients.

  Description:
    Ethernet PHY Maximum Number of Clients
    This definition select the maximum number of clients that the Ethernet
    PHY driver can support at run time. Not defining it means using a
    single client.

  Remarks:
    The MAC driver is the client of the PHY driver.
    Multiple clients may be needed when access to MIIM bus (for PHY vendor specific functionality)
    is needed through the PHY driver.

    However MIIM operations are not supported when the PHY driver uses the MIIM driver for MIIM bus accesses.
    In this case the number of clients should be 1 and the DRV_MIIM should be used for accessing the MIIM bus.

        
  *************************************************************************/

#define DRV_ETHPHY_CLIENTS_NUMBER                1


// *****************************************************************************
/* Ethernet PHY Static Index Selection

  Summary:
    Ethernet PHY static index selection.

  Description:
    This definition selects the Ethernet PHY static index for the driver object
    reference.

  Remarks:
    This index is required to make a reference to the driver object.
*/

#define DRV_ETHPHY_INDEX                                DRV_ETHPHY_INDEX_1


// *****************************************************************************
/* Ethernet PHY Interrupt And Polled Mode Operation Control

*/
// Ethernet PHY has no interrupts
//#define DRV_ETHPHY_INTERRUPT_MODE          true


// *****************************************************************************
// *****************************************************************************
// Section: Initialization Overrides
// *****************************************************************************
// *****************************************************************************
/* This section defines the initialization overrides */

// *****************************************************************************
/* Ethernet PHY Peripheral ID Selection

  Summary:
    Defines an override of the peripheral ID.

  Description:
    Defines an override of the peripheral ID, using macros.

  Remarks:

    Note: Some devices also support ETHPHY_ID_0
*/

#define DRV_ETHPHY_PERIPHERAL_ID                         ETHPHY_ID_1


// *****************************************************************************
/* Ethernet PHY power state configuration

  Summary:
    Defines an override of the power state of the Ethernet PHY driver.

  Description:
    Defines an override of the power state of the Ethernet PHY driver.

  Remarks:
    Note: This feature may not be available in the device or the Ethernet PHY module
    selected.
*/

// Has no power modes
//#define DRV_ETHPHY_POWER_STATE                 SYS_MODULE_POWER_IDLE_STOP

// *****************************************************************************
/* Ethernet MIIM access configuration

  Summary:
    Defines the way the PHY driver accesses the MIIM bus to communicate with the PHY.

  Description:
    Defines the way the PHY driver accesses the MIIM bus to communicate with the PHY:
        - either using direct access to the ETH plibs
        - using the MIIM driver - preferred way


  Remarks:
    Using the MIIM driver to perform MIIM bus operations is more versatile and preferred.

*/

#define DRV_ETHPHY_USE_DRV_MIIM         true


#endif // #ifndef _DRV_ETHPHY_CONFIG_H

/*******************************************************************************
 End of File
*/


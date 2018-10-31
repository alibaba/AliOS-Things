/*******************************************************************************
  SYS PORTS Static Functions for PORTS System Service

  Company:
    Microchip Technology Inc.

  File Name:
    sys_ports_static_pic32c.c

  Summary:
    SYS PORTS static function implementations for the Ports System Service.

  Description:
    The Ports System Service provides a simple interface to manage the ports
    on Microchip microcontrollers. This file defines the static implementation for the
    Ports System Service.

  Remarks:
    Static functions incorporate all system ports configuration settings as
    determined by the user via the Microchip Harmony Configurator GUI.  It provides
    static version of the routines, eliminating the need for an object ID or
    object handle.

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

#include "system/ports/sys_ports.h"

typedef struct {

    /* Bit position of the target pin */
    PORTS_BIT_POS          bitPos;

    /* Callback for event on target pin*/
    SYS_PORTS_PIN_CALLBACK callback;

    /* Callback Context */
    uintptr_t              context;

} SYS_PORTS_PIN_CALLBACK_OBJ;

<#macro SYS_PORTS_INITIALIZE PORT_CHANNEL SYS_PORT_DIR SYS_PORT_LAT SYS_PORT_OD SYS_PORT_PU SYS_PORT_PD SYS_PORT_PER SYS_PORT_ABCD1
          SYS_PORT_ABCD2 SYS_PORT_INT SYS_PORT_INT_TYPE SYS_PORT_INT_EDGE SYS_PORT_INT_RE_HL SYS_PORT_CHANNEL_INT>
    /* PORT${PORT_CHANNEL} ABCD 1 */
    VCAST(port_registers_t, PORTS_CHANNEL_${PORT_CHANNEL})->PORT_ABCDSR[0].w = ${SYS_PORT_ABCD1};
    /* PORT${PORT_CHANNEL} ABCD 2 */
    VCAST(port_registers_t, PORTS_CHANNEL_${PORT_CHANNEL})->PORT_ABCDSR[1].w = ${SYS_PORT_ABCD2};
    /* PORT${PORT_CHANNEL} Pio enable */
    VCAST(port_registers_t, PORTS_CHANNEL_${PORT_CHANNEL})->PORT_PDR.w = ~${SYS_PORT_PER};
    VCAST(port_registers_t, PORTS_CHANNEL_${PORT_CHANNEL})->PORT_PER.w = ${SYS_PORT_PER};
    /* PORT${PORT_CHANNEL} Active state */
    VCAST(port_registers_t, PORTS_CHANNEL_${PORT_CHANNEL})->PORT_CODR.w = ~${SYS_PORT_LAT};
    VCAST(port_registers_t, PORTS_CHANNEL_${PORT_CHANNEL})->PORT_SODR.w = ${SYS_PORT_LAT};
    /* PORT${PORT_CHANNEL} Open drain state */
    VCAST(port_registers_t, PORTS_CHANNEL_${PORT_CHANNEL})->PORT_MDDR.w = ~${SYS_PORT_OD};
    VCAST(port_registers_t, PORTS_CHANNEL_${PORT_CHANNEL})->PORT_MDER.w = ${SYS_PORT_OD};
    /* PORT${PORT_CHANNEL} Pull Up */
    VCAST(port_registers_t, PORTS_CHANNEL_${PORT_CHANNEL})->PORT_PUDR.w = ~${SYS_PORT_PU};
    VCAST(port_registers_t, PORTS_CHANNEL_${PORT_CHANNEL})->PORT_PUER.w = ${SYS_PORT_PU};
    /* PORT${PORT_CHANNEL} Pull Down */
    VCAST(port_registers_t, PORTS_CHANNEL_${PORT_CHANNEL})->PORT_PPDDR.w = ~${SYS_PORT_PD};
    VCAST(port_registers_t, PORTS_CHANNEL_${PORT_CHANNEL})->PORT_PPDER.w = ${SYS_PORT_PD};
    /* PORT${PORT_CHANNEL} Direction */
    VCAST(port_registers_t, PORTS_CHANNEL_${PORT_CHANNEL})->PORT_ODR.w = ~${SYS_PORT_DIR};
    VCAST(port_registers_t, PORTS_CHANNEL_${PORT_CHANNEL})->PORT_OER.w = ${SYS_PORT_DIR};
<#if SYS_PORT_CHANNEL_INT == true>
    /* PORT${PORT_CHANNEL} system level interrupt enable */
    SYS_INT_SourceEnable(PORT${PORT_CHANNEL}_IRQn);
    /* PORT${PORT_CHANNEL} Type of interrupt(alternate) */
    VCAST(port_registers_t, PORTS_CHANNEL_${PORT_CHANNEL})->PORT_AIMDR.w = ~${SYS_PORT_INT_TYPE};
    VCAST(port_registers_t, PORTS_CHANNEL_${PORT_CHANNEL})->PORT_AIMER.w = ${SYS_PORT_INT_TYPE};
    /* PORT${PORT_CHANNEL} If edge, type of edge */
    VCAST(port_registers_t, PORTS_CHANNEL_${PORT_CHANNEL})->PORT_ESR.w = ~${SYS_PORT_INT_EDGE};
    VCAST(port_registers_t, PORTS_CHANNEL_${PORT_CHANNEL})->PORT_ESR.w = ${SYS_PORT_INT_EDGE};
    /* PORT${PORT_CHANNEL} Rising/high level */
    VCAST(port_registers_t, PORTS_CHANNEL_${PORT_CHANNEL})->PORT_FELLSR.w = ~${SYS_PORT_INT_RE_HL};
    VCAST(port_registers_t, PORTS_CHANNEL_${PORT_CHANNEL})->PORT_REHLSR.w = ${SYS_PORT_INT_RE_HL};
    /* PORT${PORT_CHANNEL} module level Interrupt enable */
    VCAST(port_registers_t, PORTS_CHANNEL_${PORT_CHANNEL})->PORT_IDR.w = ~${SYS_PORT_INT};
    VCAST(port_registers_t, PORTS_CHANNEL_${PORT_CHANNEL})->PORT_IER.w = ${SYS_PORT_INT};
<#else>
    /* PORT${PORT_CHANNEL} module level Interrupt disable */
    VCAST(port_registers_t, PORTS_CHANNEL_${PORT_CHANNEL})->PORT_IDR.w = ~${SYS_PORT_INT};
</#if>
</#macro>

<#macro SYS_PORTS_INT_CALLBACK PORT_CHANNEL PORT_NUM_INT_PINS SYS_PORT_CHANNEL_INT>
    <#if SYS_PORT_CHANNEL_INT == true>
/* port ${PORT_CHANNEL} current number of callbacks */
uint32_t port${PORT_CHANNEL}CurNumCb = 0;

/* port ${PORT_CHANNEL} maximum number of callbacks */
uint32_t port${PORT_CHANNEL}MaxNumCb = ${PORT_NUM_INT_PINS};

/* port ${PORT_CHANNEL} callback objects */
SYS_PORTS_PIN_CALLBACK_OBJ port${PORT_CHANNEL}PinCbObj[${PORT_NUM_INT_PINS}];
    </#if>
</#macro>
<@SYS_PORTS_INT_CALLBACK 
    PORT_CHANNEL = "A" 
    PORT_NUM_INT_PINS = "SYS_PORT_A_NUM_INT_PINS"
    SYS_PORT_CHANNEL_INT = CONFIG_SYS_PORT_A_INTERRUPT_USED
/>
<@SYS_PORTS_INT_CALLBACK 
    PORT_CHANNEL = "B" 
    PORT_NUM_INT_PINS = "SYS_PORT_B_NUM_INT_PINS"
    SYS_PORT_CHANNEL_INT = CONFIG_SYS_PORT_B_INTERRUPT_USED
/>
<@SYS_PORTS_INT_CALLBACK 
    PORT_CHANNEL = "C" 
    PORT_NUM_INT_PINS = "SYS_PORT_C_NUM_INT_PINS" 
    SYS_PORT_CHANNEL_INT = CONFIG_SYS_PORT_C_INTERRUPT_USED
/>
<@SYS_PORTS_INT_CALLBACK 
    PORT_CHANNEL = "D" 
    PORT_NUM_INT_PINS = "SYS_PORT_D_NUM_INT_PINS" 
    SYS_PORT_CHANNEL_INT = CONFIG_SYS_PORT_D_INTERRUPT_USED
/>
<@SYS_PORTS_INT_CALLBACK 
    PORT_CHANNEL = "E" 
    PORT_NUM_INT_PINS = "SYS_PORT_E_NUM_INT_PINS" 
    SYS_PORT_CHANNEL_INT = CONFIG_SYS_PORT_E_INTERRUPT_USED
/>

/******************************************************************************
  Function:
    SYS_PORTS_Initialize ( void )

  Summary:
    Initializes Ports System Service

  Description:
    This function initializes different port pins/channels to the desired state.
    It also remaps the pins to the desired specific function.

  Remarks:
    None.
*/
void SYS_PORTS_Initialize ( void )
{
<#if CONFIG_SYS_PORTS_CCFG_SYSIO_VALUE?has_content>
    /* Initialize the Debug pins as GPIO pins */
    _MATRIX_REGS->CCFG_SYSIO.w |= ${CONFIG_SYS_PORTS_CCFG_SYSIO_VALUE};
</#if>

    <#if CONFIG_SYS_PORTS_INST_IDX0 == true>
    /********** PORT A Initialization **********/
        <@SYS_PORTS_INITIALIZE
            PORT_CHANNEL = "A"
            SYS_PORT_DIR = "SYS_PORT_A_DIR"
            SYS_PORT_LAT = "SYS_PORT_A_LAT"
            SYS_PORT_OD = "SYS_PORT_A_OD"
            SYS_PORT_PU = "SYS_PORT_A_PU"
            SYS_PORT_PD = "SYS_PORT_A_PD"
            SYS_PORT_PER = "SYS_PORT_A_PER"
            SYS_PORT_ABCD1 = "SYS_PORT_A_ABCD1"
            SYS_PORT_ABCD2 = "SYS_PORT_A_ABCD2"
            SYS_PORT_INT = "SYS_PORT_A_INT"
            SYS_PORT_INT_TYPE = "SYS_PORT_A_INT_TYPE"
            SYS_PORT_INT_EDGE = "SYS_PORT_A_INT_EDGE"
            SYS_PORT_INT_RE_HL = "SYS_PORT_A_INT_RE_HL"
            SYS_PORT_CHANNEL_INT = CONFIG_SYS_PORT_A_INTERRUPT_USED
        />
    </#if>

    <#if CONFIG_SYS_PORTS_INST_IDX1 == true>
    /********** PORT B Initialization **********/
        <@SYS_PORTS_INITIALIZE
            PORT_CHANNEL = "B"
            SYS_PORT_DIR = "SYS_PORT_B_DIR"
            SYS_PORT_LAT = "SYS_PORT_B_LAT"
            SYS_PORT_OD = "SYS_PORT_B_OD"
            SYS_PORT_PU = "SYS_PORT_B_PU"
            SYS_PORT_PD = "SYS_PORT_B_PD"
            SYS_PORT_PER = "SYS_PORT_B_PER"
            SYS_PORT_ABCD1 = "SYS_PORT_B_ABCD1"
            SYS_PORT_ABCD2 = "SYS_PORT_B_ABCD2"
            SYS_PORT_INT = "SYS_PORT_B_INT"
            SYS_PORT_INT_TYPE = "SYS_PORT_B_INT_TYPE"
            SYS_PORT_INT_EDGE = "SYS_PORT_B_INT_EDGE"
            SYS_PORT_INT_RE_HL = "SYS_PORT_B_INT_RE_HL"
            SYS_PORT_CHANNEL_INT = CONFIG_SYS_PORT_B_INTERRUPT_USED
        />
    </#if>

    <#if CONFIG_SYS_PORTS_INST_IDX2 == true>
    /********** PORT C Initialization **********/
        <@SYS_PORTS_INITIALIZE
            PORT_CHANNEL = "C"
            SYS_PORT_DIR = "SYS_PORT_C_DIR"
            SYS_PORT_LAT = "SYS_PORT_C_LAT"
            SYS_PORT_OD = "SYS_PORT_C_OD"
            SYS_PORT_PU = "SYS_PORT_C_PU"
            SYS_PORT_PD = "SYS_PORT_C_PD"
            SYS_PORT_PER = "SYS_PORT_C_PER"
            SYS_PORT_ABCD1 = "SYS_PORT_C_ABCD1"
            SYS_PORT_ABCD2 = "SYS_PORT_C_ABCD2"
            SYS_PORT_INT = "SYS_PORT_C_INT"
            SYS_PORT_INT_TYPE = "SYS_PORT_C_INT_TYPE"
            SYS_PORT_INT_EDGE = "SYS_PORT_C_INT_EDGE"
            SYS_PORT_INT_RE_HL = "SYS_PORT_C_INT_RE_HL"
            SYS_PORT_CHANNEL_INT = CONFIG_SYS_PORT_C_INTERRUPT_USED
        />
    </#if>

    <#if CONFIG_SYS_PORTS_INST_IDX3 == true>
    /********** PORT D Initialization **********/
        <@SYS_PORTS_INITIALIZE
            PORT_CHANNEL = "D"
            SYS_PORT_DIR = "SYS_PORT_D_DIR"
            SYS_PORT_LAT = "SYS_PORT_D_LAT"
            SYS_PORT_OD = "SYS_PORT_D_OD"
            SYS_PORT_PU = "SYS_PORT_D_PU"
            SYS_PORT_PD = "SYS_PORT_D_PD"
            SYS_PORT_PER = "SYS_PORT_D_PER"
            SYS_PORT_ABCD1 = "SYS_PORT_D_ABCD1"
            SYS_PORT_ABCD2 = "SYS_PORT_D_ABCD2"
            SYS_PORT_INT = "SYS_PORT_D_INT"
            SYS_PORT_INT_TYPE = "SYS_PORT_D_INT_TYPE"
            SYS_PORT_INT_EDGE = "SYS_PORT_D_INT_EDGE"
            SYS_PORT_INT_RE_HL = "SYS_PORT_D_INT_RE_HL"
            SYS_PORT_CHANNEL_INT = CONFIG_SYS_PORT_D_INTERRUPT_USED
        />
    </#if>

    <#if CONFIG_SYS_PORTS_INST_IDX4 == true>
    /********** PORT E Initialization **********/
        <@SYS_PORTS_INITIALIZE
            PORT_CHANNEL = "E"
            SYS_PORT_DIR = "SYS_PORT_E_DIR"
            SYS_PORT_LAT = "SYS_PORT_E_LAT"
            SYS_PORT_OD = "SYS_PORT_E_OD"
            SYS_PORT_PU = "SYS_PORT_E_PU"
            SYS_PORT_PD = "SYS_PORT_E_PD"
            SYS_PORT_PER = "SYS_PORT_E_PER"
            SYS_PORT_ABCD1 = "SYS_PORT_E_ABCD1"
            SYS_PORT_ABCD2 = "SYS_PORT_E_ABCD2"
            SYS_PORT_INT = "SYS_PORT_E_INT"
            SYS_PORT_INT_TYPE = "SYS_PORT_E_INT_TYPE"
            SYS_PORT_INT_EDGE = "SYS_PORT_E_INT_EDGE"
            SYS_PORT_INT_RE_HL = "SYS_PORT_E_INT_RE_HL"
            SYS_PORT_CHANNEL_INT = CONFIG_SYS_PORT_E_INTERRUPT_USED
        />
    </#if>

    /* Analog pins Initialization */
    _AFEC0_REGS->AFEC_CHER.w = SYS_PORT_ANALOG0;
    _AFEC0_REGS->AFEC_CHER.w = SYS_PORT_ANALOG1;
    _DACC_REGS->DACC_CHER.w = SYS_PORT_DACC;
}

/******************************************************************************
  Function:
    PORTS_DATA_TYPE SYS_PORTS_Read ( PORTS_MODULE_ID index, PORTS_CHANNEL channel )

  Summary:
    Reads the data from the I/O port.

  Description:
    This function reads the data from the I/O port.

  Remarks:
    None.
*/

PORTS_DATA_TYPE SYS_PORTS_Read ( PORTS_MODULE_ID index, PORTS_CHANNEL channel )
{
    return VCAST(port_registers_t, channel)->PORT_PDSR.w;
}


/******************************************************************************
  Function:
    void SYS_PORTS_Write ( PORTS_MODULE_ID index,
                           PORTS_CHANNEL channel,
                           PORTS_DATA_TYPE value )

  Summary:
    Writes the data from the I/O port.

  Description:
    This function writes the data to the I/O port.

  Remarks:
    None.
*/

void SYS_PORTS_Write ( PORTS_MODULE_ID index,
                       PORTS_CHANNEL channel,
                       PORTS_DATA_TYPE value )
{
    /* Enable write to the selected port */
    VCAST(port_registers_t, channel)->PORT_OWER.w = PORT_OWER_Msk;

    /* Write the desired value */
    VCAST(port_registers_t, channel)->PORT_ODSR.w = value;
}

/******************************************************************************
  Function:
    PORTS_DATA_TYPE SYS_PORTS_LatchedGet ( PORTS_MODULE_ID index, PORTS_CHANNEL channel )

  Summary:
    Reads the data driven on the I/O port.

  Description:
    This function reads the data driven on the I/O port.

  Remarks:
    None.
*/

PORTS_DATA_TYPE SYS_PORTS_LatchedGet ( PORTS_MODULE_ID index, PORTS_CHANNEL channel )
{
    return VCAST(port_registers_t, channel)->PORT_ODSR.w;
}

/******************************************************************************
  Function:
    void SYS_PORTS_Set ( PORTS_MODULE_ID index,
                         PORTS_CHANNEL channel,
                         PORTS_DATA_TYPE value,
                         PORTS_DATA_MASK mask )

  Summary:
    Sets the selected digital port/latch based on the mask.

  Description:
    This function sets the selected digital port/latch relative to the mask.

  Remarks:
    None.
*/

void SYS_PORTS_Set ( PORTS_MODULE_ID index,
                     PORTS_CHANNEL channel,
                     PORTS_DATA_TYPE value,
                     PORTS_DATA_MASK mask )
{
    VCAST(port_registers_t, channel)->PORT_SODR.w = (value & mask);
}


/******************************************************************************
  Function:
    void SYS_PORTS_Clear ( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                           PORTS_DATA_MASK clearMask )

  Summary:
    Clears the selected digital port.

  Description:
    This function clears the selected digital port.

  Remarks:
    None.
*/

void SYS_PORTS_Clear ( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                       PORTS_DATA_MASK clearMask )
{
    VCAST(port_registers_t, channel)->PORT_CODR.w = clearMask;
}


/******************************************************************************
  Function:
    void SYS_PORTS_DirectionSelect ( PORTS_MODULE_ID index,
                                     SYS_PORTS_PIN_DIRECTION pinDir,
                                     PORTS_CHANNEL channel,
                                     PORTS_DATA_MASK mask )
  Summary:
    Enables the direction for the selected port.

  Description:
    This function enables the direction for the selected port.

  Remarks:
    None.
*/

void SYS_PORTS_DirectionSelect ( PORTS_MODULE_ID index,
                                 SYS_PORTS_PIN_DIRECTION pinDir,
                                 PORTS_CHANNEL channel,
                                 PORTS_DATA_MASK mask )
{
    if (pinDir == SYS_PORTS_DIRECTION_INPUT)
    {
        VCAST(port_registers_t, channel)->PORT_ODR.w = mask;
    }
    else
    {
        VCAST(port_registers_t, channel)->PORT_OER.w = mask;
    }
}


/******************************************************************************
  Function:
    PORTS_DATA_MASK SYS_PORTS_DirectionGet ( PORTS_MODULE_ID index,
                                             PORTS_CHANNEL channel )

  Summary:
    Reads the port direction for the selected port.

  Description:
    This function reads the port direction for the selected port.

  Remarks:
    None.
*/

PORTS_DATA_MASK SYS_PORTS_DirectionGet ( PORTS_MODULE_ID index,
                                         PORTS_CHANNEL channel )
{
    return VCAST(port_registers_t, channel)->PORT_OSR.w;
}


/******************************************************************************
  Function:
    void SYS_PORTS_Toggle ( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                            PORTS_DATA_MASK toggleMask )

  Summary:
    Toggles the selected digital port pins.

  Description:
    This function toggles the selected digital port pins.

  Remarks:
    None.
*/

void SYS_PORTS_Toggle ( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                        PORTS_DATA_MASK toggleMask )
{
    uint32_t statusReg = 0;

    statusReg = VCAST(port_registers_t, channel)->PORT_ODSR.w;

    /* Write into Clr and Set registers */
    VCAST(port_registers_t, channel)->PORT_CODR.w = ( toggleMask & (statusReg));;
    VCAST(port_registers_t, channel)->PORT_SODR.w = ( toggleMask & (~statusReg));;
}


/******************************************************************************
  Function:
    void SYS_PORTS_OpenDrainEnable ( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                                     PORTS_DATA_MASK mask )

  Summary:
    Enables the open drain functionality for the selected port.

  Description:
    This function enables the open drain functionality for the selected port.

  Remarks:
    None.
*/

void SYS_PORTS_OpenDrainEnable ( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                                 PORTS_DATA_MASK mask )
{
    VCAST(port_registers_t, channel)->PORT_MDER.w = mask;
}


/******************************************************************************
  Function:
    void SYS_PORTS_OpenDrainDisable ( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                                      PORTS_DATA_MASK mask )

  Summary:
    Disables the open drain functionality for the selected port.

  Description:
    This function disables the open drain functionality for the selected port.

  Remarks:
    None.
*/

void SYS_PORTS_OpenDrainDisable ( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                                  PORTS_DATA_MASK mask )
{
    VCAST(port_registers_t, channel)->PORT_MDDR.w = mask;
}

// *****************************************************************************
/* Function:
    PORTS_DATA_TYPE SYS_PORTS_InterruptStatusGet ( PORTS_MODULE_ID index,
                                                   PORTS_CHANNEL channel )

  Summary:
    Reads the data from the I/O port.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function reads the data from the I/O port.

*/

PORTS_DATA_TYPE SYS_PORTS_InterruptStatusGet ( PORTS_MODULE_ID index,
                                               PORTS_CHANNEL channel )
{
    return VCAST(port_registers_t, channel)->PORT_ISR.w;
}

// *****************************************************************************
// *****************************************************************************
// Section: SYS Change Notification Pins Routines
// *****************************************************************************
// *****************************************************************************

/******************************************************************************
  Function:
    void SYS_PORTS_ChangeNotificationGlobalEnable( PORTS_MODULE_ID index )

  Summary:
    Globally enables the change notification.

  Description:
    This function globally enables the change notification.

  Remarks:
    None.
*/

void SYS_PORTS_ChangeNotificationGlobalEnable ( PORTS_MODULE_ID index )
{
    SYS_ASSERT(false, "This API is not supported on this device");
    return;
}

/******************************************************************************
  Function:
    void SYS_PORTS_ChangeNotificationGlobalDisable( PORTS_MODULE_ID index )

  Summary:
    Globally disables the change notification.

  Description:
    This function globally disables the change notification.

  Remarks:
    None.
*/

void SYS_PORTS_ChangeNotificationGlobalDisable ( PORTS_MODULE_ID index )
{
    SYS_ASSERT(false, "This API is not supported on this device");
    return;
}

/******************************************************************************
  Function:
    void SYS_PORTS_GlobalChangeNotificationDisable( PORTS_MODULE_ID index )

  Summary:
    Globally disables the change notification for the selected port.

  Description:
    This function globally disables the change notification for the selected port.

  Remarks:
    None.
*/

void SYS_PORTS_GlobalChangeNotificationDisable ( PORTS_MODULE_ID index )
{
    SYS_ASSERT(false, "This API is not supported on this device");
    return;
}


/******************************************************************************
  Function:
    void SYS_PORTS_ChangeNotificationEnable ( PORTS_MODULE_ID index,
                                              PORTS_CHANGE_NOTICE_PIN pinNum,
                                              SYS_PORTS_PULLUP_PULLDOWN_STATUS value )

  Summary:
    Enables the change notification for the selected port.

  Description:
    This function enables the change notification for the selected port.

  Remarks:
    None.
*/

void SYS_PORTS_ChangeNotificationEnable ( PORTS_MODULE_ID index,
                                          PORTS_CHANGE_NOTICE_PIN pinNum,
                                          SYS_PORTS_PULLUP_PULLDOWN_STATUS value )
{
    SYS_ASSERT(false, "This API is not supported on this device");
    return;
}


/******************************************************************************
  Function:
    void SYS_PORTS_ChangeNotificationDisable ( PORTS_MODULE_ID index,
                                               PORTS_CHANGE_NOTICE_PIN pinNum )

  Summary:
    Disables the change notification for the selected port.

  Description:
    This function disables the change notification for the selected port.

  Remarks:
    None.
*/

void SYS_PORTS_ChangeNotificationDisable ( PORTS_MODULE_ID index,
                                           PORTS_CHANGE_NOTICE_PIN pinNum )
{
    SYS_ASSERT(false, "This API is not supported on this device");
    return;
}


/******************************************************************************
  Function:
    void SYS_PORTS_ChangeNotificationInIdleModeEnable ( PORTS_MODULE_ID index )

  Summary:
    Enables the change notification for the selected port in Sleep or Idle mode.

  Description:
    This function enables the change notification for the selected port in Sleep
    or Idle mode.

  Remarks:
    None.
*/

void SYS_PORTS_ChangeNotificationInIdleModeEnable ( PORTS_MODULE_ID index )
{
    SYS_ASSERT(false, "This API is not supported on this device");
    return;
}


// *****************************************************************************
/* Function:
    void SYS_PORTS_ChangeNotificationInIdleModeDisable ( PORTS_MODULE_ID index )

  Summary:
    Disables the change notification for the selected port in Sleep or Idle mode.

  Description:
    This function disables the change notification for the selected port in Sleep
    or Idle mode.

  Remarks:
    None.
*/

void SYS_PORTS_ChangeNotificationInIdleModeDisable ( PORTS_MODULE_ID index )
{
    SYS_ASSERT(false, "This API is not supported on this device");
    return;
}


// *****************************************************************************
/* Function:
    void SYS_PORTS_ChangeNotificationPullUpEnable ( PORTS_MODULE_ID index,
                                                    PORTS_CHANGE_NOTICE_PIN pinNum )

  Summary:
    Enables weak pull-up on change notification pin.

  Description:
    This function enables weak pull-up on change notification pin.

  Remarks:
    None.
*/

void SYS_PORTS_ChangeNotificationPullUpEnable ( PORTS_MODULE_ID index,
                                                PORTS_CHANGE_NOTICE_PIN pinNum )
{
    SYS_ASSERT(false, "This API is not supported on this device");
    return;
}


// *****************************************************************************
/* Function:
    void SYS_PORTS_ChangeNotificationPullUpDisable ( PORTS_MODULE_ID index,
                                                    PORTS_CHANGE_NOTICE_PIN pinNum )

  Summary:
    Disables pull-up on input change.

  Description:
    This function disables pull-up on input change.

  Remarks:
    None.
*/

void SYS_PORTS_ChangeNotificationPullUpDisable ( PORTS_MODULE_ID index,
                                                 PORTS_CHANGE_NOTICE_PIN pinNum )
{
    SYS_ASSERT(false, "This API is not supported on this device");
    return;
}

// *****************************************************************************
// *****************************************************************************
// Section: SYS PORT PINS Control Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void SYS_PORTS_PinModeSelect ( PORTS_MODULE_ID index, PORTS_ANALOG_PIN pin,
                                   PORTS_PIN_MODE mode)

  Summary:
    Enables the selected pin as analog or digital.

  Description:
    This function enables the selected pin as analog or digital.

 Remarks:
    None.
*/

void SYS_PORTS_PinModeSelect ( PORTS_MODULE_ID index, PORTS_ANALOG_PIN pin,
                               PORTS_PIN_MODE mode)
{
    SYS_ASSERT(false, "This API is not supported on this device");
    return;
}

// *****************************************************************************
/* Function:
    void SYS_PORTS_PinWrite ( PORTS_MODULE_ID index,
                              PORTS_CHANNEL channel,
                              PORTS_BIT_POS bitPos
                              bool value )
  Summary:
    Writes the selected digital pin.

  Description:
    This function writes the selected digital pin.

  Remarks:
    None.
*/

void SYS_PORTS_PinWrite ( PORTS_MODULE_ID index,
                          PORTS_CHANNEL channel,
                          PORTS_BIT_POS bitPos,
                          bool value )
{
    if (value == false)
    {
        VCAST(port_registers_t, channel)->PORT_CODR.w = 1 << bitPos;
    }
    else
    {
        VCAST(port_registers_t, channel)->PORT_SODR.w = 1 << bitPos;
    }
}


// *****************************************************************************
/* Function:
    bool SYS_PORTS_PinLatchedGet ( PORTS_MODULE_ID index,
                                   PORTS_CHANNEL channel,
                                   PORTS_BIT_POS bitPos )

  Summary:
    Reads the data driven on selected digital pin.

  Description:
    This function reads the driven data on selected digital pin.

  Remarks:
    None.
*/

bool SYS_PORTS_PinLatchedGet ( PORTS_MODULE_ID index,
                               PORTS_CHANNEL channel,
                               PORTS_BIT_POS bitPos )
{
    return (bool)((VCAST(port_registers_t, channel)->PORT_ODSR.w >> bitPos) & 0x00000001);
}


// *****************************************************************************
/* Function:
    bool SYS_PORTS_PinRead ( PORTS_MODULE_ID index,
                             PORTS_CHANNEL channel,
                             PORTS_BIT_POS bitPos )

  Summary:
    Reads the selected digital pin.

  Description:
    This function reads the selected digital pin.

  Remarks:
    None.
*/

bool SYS_PORTS_PinRead ( PORTS_MODULE_ID index,
                         PORTS_CHANNEL channel,
                         PORTS_BIT_POS bitPos )
{
    return (bool)((VCAST(port_registers_t, channel)->PORT_PDSR.w >> bitPos) & 0x00000001);
}


// *****************************************************************************
/* Function:
    void SYS_PORTS_PinToggle ( PORTS_MODULE_ID index,
                               PORTS_CHANNEL channel,
                               PORTS_BIT_POS bitPos )

  Summary:
    Toggles the selected digital pin.

  Description:
    This function toggles the selected digital pin.

  Remarks:
    None.
*/

void SYS_PORTS_PinToggle ( PORTS_MODULE_ID index,
                           PORTS_CHANNEL channel,
                           PORTS_BIT_POS bitPos )
{
    if ( ((VCAST(port_registers_t, channel)->PORT_ODSR.w >> bitPos) & 1) == 1 )
    {
        VCAST(port_registers_t, channel)->PORT_CODR.w = 1 << bitPos;
    }
    else
    {
        VCAST(port_registers_t, channel)->PORT_SODR.w = 1 << bitPos;
    }
}


// *****************************************************************************
/* Function:
    void SYS_PORTS_PinSet ( PORTS_MODULE_ID index,
                            PORTS_CHANNEL channel,
                            PORTS_BIT_POS bitPos )

  Summary:
    Sets the selected digital pin/latch.

  Description:
    This function sets the selected digital pin/latch.

  Remarks:
    None.
*/

void SYS_PORTS_PinSet ( PORTS_MODULE_ID index,
                        PORTS_CHANNEL channel,
                        PORTS_BIT_POS bitPos )
{
    VCAST(port_registers_t, channel)->PORT_SODR.w = 1 << bitPos;
}


// *****************************************************************************
/* Function:
    void SYS_PORTS_PinClear ( PORTS_MODULE_ID index,
                              PORTS_CHANNEL channel,
                              PORTS_BIT_POS bitPos )

  Summary:
    Clears the selected digital pin.

  Description:
    This function clears the selected digital pin.

  Remarks:
    None.
*/

void SYS_PORTS_PinClear ( PORTS_MODULE_ID index,
                          PORTS_CHANNEL channel,
                          PORTS_BIT_POS bitPos )
{
    VCAST(port_registers_t, channel)->PORT_CODR.w = 1 << bitPos;
}


// *****************************************************************************
/* Function:
    void SYS_PORTS_PinDirectionSelect ( PORTS_MODULE_ID index,
                                        SYS_PORTS_PIN_DIRECTION pinDir,
                                        PORTS_CHANNEL channel,
                                        PORTS_BIT_POS bitPos )
  Summary:
    Enables the direction for the selected pin.

  Description:
    This function enables the direction for the selected pin.

  Remarks:
    None.
*/

void SYS_PORTS_PinDirectionSelect ( PORTS_MODULE_ID index,
                                    SYS_PORTS_PIN_DIRECTION pinDir,
                                    PORTS_CHANNEL channel,
                                    PORTS_BIT_POS bitPos )
{
    if (pinDir == SYS_PORTS_DIRECTION_OUTPUT)
    {
        VCAST(port_registers_t, channel)->PORT_OER.w = 1 << bitPos;
    }
    else
    {
        VCAST(port_registers_t, channel)->PORT_ODR.w = 1 << bitPos;
    }
}


// *****************************************************************************
/* Function:
    void SYS_PORTS_PinOpenDrainEnable ( PORTS_MODULE_ID index,
                                        PORTS_CHANNEL channel,
                                        PORTS_BIT_POS bitPos )

  Summary:
    Enables the open-drain functionality for the selected pin.

  Description:
    This function enables the open-drain functionality for the selected pin.

  Remarks:
    None.
*/

void SYS_PORTS_PinOpenDrainEnable ( PORTS_MODULE_ID index,
                                    PORTS_CHANNEL channel,
                                    PORTS_BIT_POS bitPos )
{
    VCAST(port_registers_t, channel)->PORT_MDER.w = 1 << bitPos;
}


// *****************************************************************************
/* Function:
    void SYS_PORTS_PinOpenDrainDisable ( PORTS_MODULE_ID index,
                                         PORTS_CHANNEL channel,
                                         PORTS_BIT_POS bitPos )

  Summary:
    Disables the open-drain functionality for the selected pin.

  Description:
    This function disables the open-drain functionality for the selected pin.

  Remarks:
    None.
*/

void SYS_PORTS_PinOpenDrainDisable ( PORTS_MODULE_ID index,
                                     PORTS_CHANNEL channel,
                                     PORTS_BIT_POS bitPos )
{
    VCAST(port_registers_t, channel)->PORT_MDDR.w = 1 << bitPos;
}


// *****************************************************************************
/* Function:
    void SYS_PORTS_PinPullUpEnable ( PORTS_MODULE_ID index,
                                        PORTS_CHANNEL channel,
                                        PORTS_BIT_POS bitPos )

  Summary:
    Enables the pull-up functionality for the selected pin.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function enables the pull-up functionality for the selected pin.

  Remarks:
    Not all features are available on all devices. Refer to the specific device
    data sheet for availability.
*/

void SYS_PORTS_PinPullUpEnable ( PORTS_MODULE_ID index,
                                    PORTS_CHANNEL channel,
                                    PORTS_BIT_POS bitPos )
{
    VCAST(port_registers_t, channel)->PORT_PUER.w = 1 << bitPos;
}


// *****************************************************************************
/* Function:
    void SYS_PORTS_PinPullUpDisable ( PORTS_MODULE_ID index,
                                         PORTS_CHANNEL channel,
                                         PORTS_BIT_POS bitPos )

  Summary:
    Disables the pull-up functionality for the selected pin.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function disables the pull-up functionality for the selected pin.

  Remarks:
    Not all features are available on all devices. Refer to the specific device
    data sheet for availability.
*/

void SYS_PORTS_PinPullUpDisable ( PORTS_MODULE_ID index,
                                     PORTS_CHANNEL channel,
                                     PORTS_BIT_POS bitPos )
{
    VCAST(port_registers_t, channel)->PORT_PUDR.w = 1 << bitPos;
}


// *****************************************************************************
/* Function:
    void SYS_PORTS_PinPullDownEnable ( PORTS_MODULE_ID index,
                                        PORTS_CHANNEL channel,
                                        PORTS_BIT_POS bitPos )

  Summary:
    Enables the pull-down functionality for the selected pin.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function enables the pull-down functionality for the selected pin.

  Remarks:
    Not all features are available on all devices. Refer to the specific device
    data sheet for availability.
*/

void SYS_PORTS_PinPullDownEnable ( PORTS_MODULE_ID index,
                                    PORTS_CHANNEL channel,
                                    PORTS_BIT_POS bitPos )
{
    VCAST(port_registers_t, channel)->PORT_PPDER.w = 1 << bitPos;
}


// *****************************************************************************
/* Function:
    void SYS_PORTS_PinPullDownDisable ( PORTS_MODULE_ID index,
                                        PORTS_CHANNEL channel,
                                        PORTS_BIT_POS bitPos )

  Summary:
    Disables the pull-down functionality for the selected pin.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function disables the pull-down functionality for the selected pin.

  Remarks:
    Not all features are available on all devices. Refer to the specific device
    data sheet for availability.
*/

void SYS_PORTS_PinPullDownDisable ( PORTS_MODULE_ID index,
                                    PORTS_CHANNEL channel,
                                    PORTS_BIT_POS bitPos )
{
    VCAST(port_registers_t, channel)->PORT_PPDDR.w = 1 << bitPos;
}


/******************************************************************************
  Function:
    void SYS_PORTS_InterruptEnable
    (
        PORTS_MODULE_ID index,
        PORTS_CHANNEL channel,
        PORTS_BIT_POS bitPos,
        PORTS_PIN_INTERRUPT_TYPE pinInterruptType
    )

  Summary:
    Enables the change notification interrupt for the selected port pin.

  Description:
    This function enables the change notification interrupt of selected type
    for the selected port pin.

  Remarks:
    None.
*/

void SYS_PORTS_InterruptEnable ( PORTS_MODULE_ID index,
                                 PORTS_CHANNEL channel,
                                 PORTS_BIT_POS bitPos,
                                 PORTS_PIN_INTERRUPT_TYPE pinInterruptType )
{
    switch (pinInterruptType)
    {
        case PORTS_PIN_INTERRUPT_RISING_EDGE:
            VCAST(port_registers_t, channel)->PORT_ESR.w = 1 << bitPos;
            VCAST(port_registers_t, channel)->PORT_REHLSR.w = 1 << bitPos;
        break;

        case PORTS_PIN_INTERRUPT_FALLING_EDGE:
            VCAST(port_registers_t, channel)->PORT_ESR.w = 1 << bitPos;
            VCAST(port_registers_t, channel)->PORT_FELLSR.w = 1 << bitPos;
        break;

        case PORTS_PIN_INTERRUPT_HIGH_LEVEL:
            VCAST(port_registers_t, channel)->PORT_LSR.w = 1 << bitPos;
            VCAST(port_registers_t, channel)->PORT_REHLSR.w = 1 << bitPos;
        break;

        case PORTS_PIN_INTERRUPT_LOW_LEVEL:
            VCAST(port_registers_t, channel)->PORT_LSR.w = 1 << bitPos;
            VCAST(port_registers_t, channel)->PORT_FELLSR.w = 1 << bitPos;
        break;

        default:
        break;
    }

    if (pinInterruptType == PORTS_PIN_INTERRUPT_NONE)
    {
        /* Do nothing */
    }
    else
    {
        if (pinInterruptType == PORTS_PIN_INTERRUPT_BOTH_EDGE)
        {
           VCAST(port_registers_t, channel)->PORT_IER.w = 1 << bitPos;
        }
        else
        {
            /* For other interrupts, we have to enable additional interrupt */
            VCAST(port_registers_t, channel)->PORT_AIMER.w = 1 << bitPos;
            VCAST(port_registers_t, channel)->PORT_IER.w = 1 << bitPos;
        }
    }

    return;
}

// *****************************************************************************
/* Function:
    void SYS_PORTS_RemapInput ( PORTS_MODULE_ID index,
                                PORTS_REMAP_INPUT_FUNCTION function,
                                PORTS_REMAP_INPUT_PIN remapPin )

  Summary:
    Input/Output (I/O) function remapping.

  Description:
    This function controls the I/O function remapping.

  Precondition:
    None.
*/
void SYS_PORTS_RemapInput ( PORTS_MODULE_ID index,
                            PORTS_REMAP_INPUT_FUNCTION function,
                            PORTS_REMAP_INPUT_PIN remapPin )
{
    SYS_ASSERT(false, "This API is not supported on this device");
    return;
}

// *****************************************************************************
/* Function:
    void SYS_PORTS_RemapOutput ( PORTS_MODULE_ID index,
                                 PORTS_REMAP_OUTPUT_FUNCTION function,
                                 PORTS_REMAP_OUTPUT_PIN remapPin )

  Summary:
    Input/Output (I/O) function remapping.

  Description:
    This function controls the I/O function remapping.

  Precondition:
    None.
*/
void SYS_PORTS_RemapOutput ( PORTS_MODULE_ID index,
                             PORTS_REMAP_OUTPUT_FUNCTION function,
                             PORTS_REMAP_OUTPUT_PIN remapPin )
{
    SYS_ASSERT(false, "This API is not supported on this device");
}

//******************************************************************************
/* Function:
    void SYS_PORTS_PinCallbackRegister
    (
        PORTS_CHANNEL channel,
        PORTS_BIT_POS bitPos,
        SYS_PORTS_PIN_CALLBACK callback,
        uintptr_t context
    )

  Summary:
    Registers the event callback function for the selected port pin.

  Description:
    This function registers the event callback function for the selected port pin.

  Preconditions:
    Interrupt for the pin to be enabled from the PIO Interrupt tab of
    Pin Settings in MHC.
*/

void SYS_PORTS_PinCallbackRegister( PORTS_CHANNEL channel,
                                    PORTS_BIT_POS bitPos,
                                    SYS_PORTS_PIN_CALLBACK callback,
                                    uintptr_t context )
{
<#if CONFIG_SYS_PORT_A_INTERRUPT_USED == true ||
     CONFIG_SYS_PORT_B_INTERRUPT_USED == true ||
     CONFIG_SYS_PORT_C_INTERRUPT_USED == true ||
     CONFIG_SYS_PORT_D_INTERRUPT_USED == true ||
     CONFIG_SYS_PORT_E_INTERRUPT_USED == true >
    switch( channel )
    {
<#if CONFIG_SYS_PORT_A_INTERRUPT_USED == true>
        case PORTS_CHANNEL_A:
        {
            if( portACurNumCb < portAMaxNumCb )
            {
                portAPinCbObj[ portACurNumCb ].bitPos   = bitPos;
                portAPinCbObj[ portACurNumCb ].callback = callback;
                portAPinCbObj[ portACurNumCb ].context  = context;
                portACurNumCb++;
            }
            break;
        }
</#if>
<#if CONFIG_SYS_PORT_B_INTERRUPT_USED == true>
        case PORTS_CHANNEL_B:
        {
            if( portBCurNumCb < portBMaxNumCb )
            {
                portBPinCbObj[ portBCurNumCb ].bitPos   = bitPos;
                portBPinCbObj[ portBCurNumCb ].callback = callback;
                portBPinCbObj[ portBCurNumCb ].context  = context;
                portBCurNumCb++;
            }
            break;
        }
</#if>
<#if CONFIG_SYS_PORT_C_INTERRUPT_USED == true>
        case PORTS_CHANNEL_C:
        {
            if( portCCurNumCb < portCMaxNumCb )
            {
                portCPinCbObj[ portCCurNumCb ].bitPos   = bitPos;
                portCPinCbObj[ portCCurNumCb ].callback = callback;
                portCPinCbObj[ portCCurNumCb ].context  = context;
                portCCurNumCb++;
            }
            break;
        }
</#if>
<#if CONFIG_SYS_PORT_D_INTERRUPT_USED == true>
        case PORTS_CHANNEL_D:
        {
            if( portDCurNumCb < portDMaxNumCb )
            {
                portDPinCbObj[ portDCurNumCb ].bitPos   = bitPos;
                portDPinCbObj[ portDCurNumCb ].callback = callback;
                portDPinCbObj[ portDCurNumCb ].context  = context;
                portDCurNumCb++;
            }
            break;
        }
</#if>
<#if CONFIG_SYS_PORT_E_INTERRUPT_USED == true>
        case PORTS_CHANNEL_E:
        {
            if( portECurNumCb < portEMaxNumCb )
            {
                portEPinCbObj[ portECurNumCb ].bitPos   = bitPos;
                portEPinCbObj[ portECurNumCb ].callback = callback;
                portEPinCbObj[ portECurNumCb ].context  = context;
                portECurNumCb++;
            }
            break;
        }
</#if>
        default:
        {
            break;
        }
    }
<#else>
    SYS_ASSERT(false, "Interrupt for the pin to be enabled from the PIO Interrupt tab of Pin Settings in MHC.");
</#if>
}

//******************************************************************************
/* Function:
    void SYS_PORTS_Tasks ( PORTS_CHANNEL channel )

  Summary:
    Implements the ISR for the given channel.

  Description:
    This function implements the ISR for the given channel.

  Preconditions:
    Interrupt for the pin to be enabled from the PIO Interrupt tab of Pin Settings in MHC.
*/

void SYS_PORTS_Tasks ( PORTS_CHANNEL channel )
{
<#if CONFIG_SYS_PORT_A_INTERRUPT_USED == true ||
     CONFIG_SYS_PORT_B_INTERRUPT_USED == true ||
     CONFIG_SYS_PORT_C_INTERRUPT_USED == true ||
     CONFIG_SYS_PORT_D_INTERRUPT_USED == true ||
     CONFIG_SYS_PORT_E_INTERRUPT_USED == true >
    PORTS_DATA_TYPE status;
    uint32_t i;

    status  = VCAST(port_registers_t, channel)->PORT_ISR.w;
    status &= VCAST(port_registers_t, channel)->PORT_IMR.w;

    /* Check pending events */
    switch( channel )
    {
    <#if CONFIG_SYS_PORT_A_INTERRUPT_USED == true>
        case PORTS_CHANNEL_A:
        {
            for( i = 0; i < portACurNumCb; i++ )
            {
                if( ( status & ( 1 << portAPinCbObj[i].bitPos ) ) &&
                    portAPinCbObj[i].callback != NULL )
                {
                    portAPinCbObj[i].callback ( portAPinCbObj[i].context );
                }
            }
            break;
        }
    </#if>
    <#if CONFIG_SYS_PORT_B_INTERRUPT_USED == true>
        case PORTS_CHANNEL_B:
        {
            for( i = 0; i < portBCurNumCb; i++ )
            {
                if( ( status & ( 1 << portBPinCbObj[i].bitPos ) ) &&
                    portBPinCbObj[i].callback != NULL )
                {
                    portBPinCbObj[i].callback ( portBPinCbObj[i].context );
                }
            }
            break;
        }
    </#if>
    <#if CONFIG_SYS_PORT_C_INTERRUPT_USED == true>
        case PORTS_CHANNEL_C:
        {
            for( i = 0; i < portCCurNumCb; i++ )
            {
                if( ( status & ( 1 << portCPinCbObj[i].bitPos ) ) &&
                    portCPinCbObj[i].callback != NULL )
                {
                    portCPinCbObj[i].callback ( portCPinCbObj[i].context );
                }
            }
            break;
        }
    </#if>
    <#if CONFIG_SYS_PORT_D_INTERRUPT_USED == true>
        case PORTS_CHANNEL_D:
        {
            for( i = 0; i < portDCurNumCb; i++ )
            {
                if( ( status & ( 1 << portDPinCbObj[i].bitPos ) ) &&
                    portDPinCbObj[i].callback != NULL )
                {
                    portDPinCbObj[i].callback ( portDPinCbObj[i].context );
                }
            }
            break;
        }
    </#if>
    <#if CONFIG_SYS_PORT_E_INTERRUPT_USED == true>
        case PORTS_CHANNEL_E:
        {
            for( i = 0; i < portECurNumCb; i++ )
            {
                if( ( status & ( 1 << portEPinCbObj[i].bitPos ) ) &&
                    portEPinCbObj[i].callback != NULL )
                {
                    portEPinCbObj[i].callback ( portEPinCbObj[i].context );
                }
            }
            break;
        }
    </#if>
        default:
        {
            break;
        }
    }
<#else>
    ;
</#if>
}

/*******************************************************************************
 End of File
*/

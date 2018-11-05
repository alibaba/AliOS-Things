/*******************************************************************************
  SYS PORTS Static Functions for PORTS System Service

  Company:
    Microchip Technology Inc.

  File Name:
    sys_ports_static.c

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
Copyright 2017 Microchip Technology Incorporated and its subsidiaries.

Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software and associated documentation files (the "Software"), to deal in 
the Software without restriction, including without limitation the rights to 
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
of the Software, and to permit persons to whom the Software is furnished to do 
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE
*******************************************************************************/
//DOM-IGNORE-END

#include "system_config.h"
#include "system/ports/sys_ports.h"
#include "peripheral/devcon/plib_devcon.h"
#include "peripheral/ports/plib_ports.h"
#include "peripheral/int/plib_int.h"

/******************************************************************************
  Function:
    SYS_PORTS_Initialize(void)

  Summary:
    Initializes Ports System Service

  Description:
    This function initializes different port pins/channels to the desired state.
    It also remaps the pins to the desired specific function.

  Remarks:
    None.
*/
void SYS_PORTS_Initialize(void)
{

    /* PORT A Initialization */
    PLIB_PORTS_OpenDrainEnable(PORTS_ID_0, PORT_CHANNEL_A, SYS_PORT_A_ODC);
    PLIB_PORTS_Write( PORTS_ID_0, PORT_CHANNEL_A,  SYS_PORT_A_LAT);
    PLIB_PORTS_DirectionOutputSet( PORTS_ID_0, PORT_CHANNEL_A,  SYS_PORT_A_TRIS ^ 0xFFFF);
    PLIB_PORTS_ChangeNoticePerPortTurnOn(PORTS_ID_0, PORT_CHANNEL_A);
    PLIB_PORTS_ChannelModeSelect(PORTS_ID_0, PORT_CHANNEL_A, SYS_PORT_A_ANSEL ^ 0xFFFF, PORTS_PIN_MODE_DIGITAL);
    PLIB_PORTS_ChannelChangeNoticeEnable(PORTS_ID_0, PORT_CHANNEL_A, SYS_PORT_A_CNEN);
    PLIB_PORTS_ChannelChangeNoticePullUpEnable(PORTS_ID_0, PORT_CHANNEL_A, SYS_PORT_A_CNPU);
    PLIB_PORTS_ChannelChangeNoticePullDownEnable(PORTS_ID_0, PORT_CHANNEL_A, SYS_PORT_A_CNPD);
    
    /* PORT B Initialization */
    PLIB_PORTS_OpenDrainEnable(PORTS_ID_0, PORT_CHANNEL_B, SYS_PORT_B_ODC);
    PLIB_PORTS_Write( PORTS_ID_0, PORT_CHANNEL_B,  SYS_PORT_B_LAT);
    PLIB_PORTS_DirectionOutputSet( PORTS_ID_0, PORT_CHANNEL_B,  SYS_PORT_B_TRIS ^ 0xFFFF);
    PLIB_PORTS_ChangeNoticePerPortTurnOn(PORTS_ID_0, PORT_CHANNEL_B);
    PLIB_PORTS_ChannelModeSelect(PORTS_ID_0, PORT_CHANNEL_B, SYS_PORT_B_ANSEL ^ 0xFFFF, PORTS_PIN_MODE_DIGITAL);
    PLIB_PORTS_ChannelChangeNoticeEnable(PORTS_ID_0, PORT_CHANNEL_B, SYS_PORT_B_CNEN);
    PLIB_PORTS_ChannelChangeNoticePullUpEnable(PORTS_ID_0, PORT_CHANNEL_B, SYS_PORT_B_CNPU);
    PLIB_PORTS_ChannelChangeNoticePullDownEnable(PORTS_ID_0, PORT_CHANNEL_B, SYS_PORT_B_CNPD);
    
    /* PORT C Initialization */
    PLIB_PORTS_OpenDrainEnable(PORTS_ID_0, PORT_CHANNEL_C, SYS_PORT_C_ODC);
    PLIB_PORTS_Write( PORTS_ID_0, PORT_CHANNEL_C,  SYS_PORT_C_LAT);
    PLIB_PORTS_DirectionOutputSet( PORTS_ID_0, PORT_CHANNEL_C,  SYS_PORT_C_TRIS ^ 0xFFFF);
    PLIB_PORTS_ChangeNoticePerPortTurnOn(PORTS_ID_0, PORT_CHANNEL_C);
    PLIB_PORTS_ChannelModeSelect(PORTS_ID_0, PORT_CHANNEL_C, SYS_PORT_C_ANSEL ^ 0xFFFF, PORTS_PIN_MODE_DIGITAL);
    PLIB_PORTS_ChannelChangeNoticeEnable(PORTS_ID_0, PORT_CHANNEL_C, SYS_PORT_C_CNEN);
    PLIB_PORTS_ChannelChangeNoticePullUpEnable(PORTS_ID_0, PORT_CHANNEL_C, SYS_PORT_C_CNPU);
    PLIB_PORTS_ChannelChangeNoticePullDownEnable(PORTS_ID_0, PORT_CHANNEL_C, SYS_PORT_C_CNPD);
    
    /* PORT D Initialization */
    PLIB_PORTS_OpenDrainEnable(PORTS_ID_0, PORT_CHANNEL_D, SYS_PORT_D_ODC);
    PLIB_PORTS_Write( PORTS_ID_0, PORT_CHANNEL_D,  SYS_PORT_D_LAT);
    PLIB_PORTS_DirectionOutputSet( PORTS_ID_0, PORT_CHANNEL_D,  SYS_PORT_D_TRIS ^ 0xFFFF);
    PLIB_PORTS_ChangeNoticePerPortTurnOn(PORTS_ID_0, PORT_CHANNEL_D);
    PLIB_PORTS_ChannelModeSelect(PORTS_ID_0, PORT_CHANNEL_D, SYS_PORT_D_ANSEL ^ 0xFFFF, PORTS_PIN_MODE_DIGITAL);
    PLIB_PORTS_ChannelChangeNoticeEnable(PORTS_ID_0, PORT_CHANNEL_D, SYS_PORT_D_CNEN);
    PLIB_PORTS_ChannelChangeNoticePullUpEnable(PORTS_ID_0, PORT_CHANNEL_D, SYS_PORT_D_CNPU);
    PLIB_PORTS_ChannelChangeNoticePullDownEnable(PORTS_ID_0, PORT_CHANNEL_D, SYS_PORT_D_CNPD);
    
    /* PORT E Initialization */
    PLIB_PORTS_OpenDrainEnable(PORTS_ID_0, PORT_CHANNEL_E, SYS_PORT_E_ODC);
    PLIB_PORTS_Write( PORTS_ID_0, PORT_CHANNEL_E,  SYS_PORT_E_LAT);
    PLIB_PORTS_DirectionOutputSet( PORTS_ID_0, PORT_CHANNEL_E,  SYS_PORT_E_TRIS ^ 0xFFFF);
    PLIB_PORTS_ChangeNoticePerPortTurnOn(PORTS_ID_0, PORT_CHANNEL_E);
    PLIB_PORTS_ChannelModeSelect(PORTS_ID_0, PORT_CHANNEL_E, SYS_PORT_E_ANSEL ^ 0xFFFF, PORTS_PIN_MODE_DIGITAL);
    PLIB_PORTS_ChannelChangeNoticeEnable(PORTS_ID_0, PORT_CHANNEL_E, SYS_PORT_E_CNEN);
    PLIB_PORTS_ChannelChangeNoticePullUpEnable(PORTS_ID_0, PORT_CHANNEL_E, SYS_PORT_E_CNPU);
    PLIB_PORTS_ChannelChangeNoticePullDownEnable(PORTS_ID_0, PORT_CHANNEL_E, SYS_PORT_E_CNPD);
    
    /* PORT F Initialization */
    PLIB_PORTS_OpenDrainEnable(PORTS_ID_0, PORT_CHANNEL_F, SYS_PORT_F_ODC);
    PLIB_PORTS_Write( PORTS_ID_0, PORT_CHANNEL_F,  SYS_PORT_F_LAT);
    PLIB_PORTS_DirectionOutputSet( PORTS_ID_0, PORT_CHANNEL_F,  SYS_PORT_F_TRIS ^ 0xFFFF);
    PLIB_PORTS_ChangeNoticePerPortTurnOn(PORTS_ID_0, PORT_CHANNEL_F);
    PLIB_PORTS_ChannelModeSelect(PORTS_ID_0, PORT_CHANNEL_F, SYS_PORT_F_ANSEL ^ 0xFFFF, PORTS_PIN_MODE_DIGITAL);
    PLIB_PORTS_ChannelChangeNoticeEnable(PORTS_ID_0, PORT_CHANNEL_F, SYS_PORT_F_CNEN);
    PLIB_PORTS_ChannelChangeNoticePullUpEnable(PORTS_ID_0, PORT_CHANNEL_F, SYS_PORT_F_CNPU);
    PLIB_PORTS_ChannelChangeNoticePullDownEnable(PORTS_ID_0, PORT_CHANNEL_F, SYS_PORT_F_CNPD);
    
    /* PORT G Initialization */
    PLIB_PORTS_OpenDrainEnable(PORTS_ID_0, PORT_CHANNEL_G, SYS_PORT_G_ODC);
    PLIB_PORTS_Write( PORTS_ID_0, PORT_CHANNEL_G,  SYS_PORT_G_LAT);
    PLIB_PORTS_DirectionOutputSet( PORTS_ID_0, PORT_CHANNEL_G,  SYS_PORT_G_TRIS ^ 0xFFFF);
    PLIB_PORTS_ChangeNoticePerPortTurnOn(PORTS_ID_0, PORT_CHANNEL_G);
    PLIB_PORTS_ChannelModeSelect(PORTS_ID_0, PORT_CHANNEL_G, SYS_PORT_G_ANSEL ^ 0xFFFF, PORTS_PIN_MODE_DIGITAL);
    PLIB_PORTS_ChannelChangeNoticeEnable(PORTS_ID_0, PORT_CHANNEL_G, SYS_PORT_G_CNEN);
    PLIB_PORTS_ChannelChangeNoticePullUpEnable(PORTS_ID_0, PORT_CHANNEL_G, SYS_PORT_G_CNPU);
    PLIB_PORTS_ChannelChangeNoticePullDownEnable(PORTS_ID_0, PORT_CHANNEL_G, SYS_PORT_G_CNPD);


    /* PPS Input Remapping */
    PLIB_PORTS_RemapInput(PORTS_ID_0, INPUT_FUNC_U1RX, INPUT_PIN_RPD10 );
    PLIB_PORTS_RemapInput(PORTS_ID_0, INPUT_FUNC_SDI3, INPUT_PIN_RPB10 );
    PLIB_PORTS_RemapInput(PORTS_ID_0, INPUT_FUNC_INT3, INPUT_PIN_RPF4 );
    PLIB_PORTS_RemapInput(PORTS_ID_0, INPUT_FUNC_SDI2, INPUT_PIN_RPG0 );
    PLIB_PORTS_RemapInput(PORTS_ID_0, INPUT_FUNC_INT2, INPUT_PIN_RPF12 );

    /* PPS Output Remapping */
    PLIB_PORTS_RemapOutput(PORTS_ID_0, OUTPUT_FUNC_U1TX, OUTPUT_PIN_RPD15 );
    PLIB_PORTS_RemapOutput(PORTS_ID_0, OUTPUT_FUNC_SDO3, OUTPUT_PIN_RPB9 );
    PLIB_PORTS_RemapOutput(PORTS_ID_0, OUTPUT_FUNC_SDO2, OUTPUT_PIN_RPG7 );

    
}

/******************************************************************************
  Function:
    PORTS_DATA_TYPE SYS_PORTS_Read( PORTS_MODULE_ID index, PORTS_CHANNEL channel )

  Summary:
    Reads the data from the I/O port.

  Description:
    This function reads the data from the I/O port.

  Remarks:
    None.
*/

PORTS_DATA_TYPE SYS_PORTS_Read( PORTS_MODULE_ID index, PORTS_CHANNEL channel )
{
    return PLIB_PORTS_Read( index, channel );
}


/******************************************************************************
  Function:
    void SYS_PORTS_Write( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                      		PORTS_DATA_TYPE value )

  Summary:
    Writes the data from the I/O port.

  Description:
    This function writes the data to the I/O port.

  Remarks:
    None.
*/

void SYS_PORTS_Write( PORTS_MODULE_ID index,
                      PORTS_CHANNEL channel,
                      PORTS_DATA_TYPE value )
{
    PLIB_PORTS_Write( index, channel, value );
}

/******************************************************************************
  Function:
    PORTS_DATA_TYPE SYS_PORTS_LatchedGet( PORTS_MODULE_ID index, PORTS_CHANNEL channel )

  Summary:
    Reads the data driven on the I/O port.

  Description:
    This function reads the data driven on the I/O port.

  Remarks:
    None.
*/

PORTS_DATA_TYPE SYS_PORTS_LatchedGet( PORTS_MODULE_ID index, PORTS_CHANNEL channel )
{
    SYS_ASSERT(false, "This API is not supported on this device");
    /* TODO: Call corresponding PLIB API */
    return 0;
}

/******************************************************************************
  Function:
    void SYS_PORTS_Set( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
	                         PORTS_DATA_TYPE value,
                         PORTS_DATA_MASK mask )

  Summary:
    Sets the selected digital port/latch based on the mask.

  Description:
    This function sets the selected digital port/latch relative to the mask.

  Remarks:
    None.
*/

void SYS_PORTS_Set( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                    PORTS_DATA_TYPE value,
                    PORTS_DATA_MASK mask )
{
    PLIB_PORTS_Set( index, channel, value, mask );
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
    PLIB_PORTS_Clear ( index, channel, clearMask );
}


/******************************************************************************
  Function:
    void SYS_PORTS_DirectionSelect( PORTS_MODULE_ID index,
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

void SYS_PORTS_DirectionSelect( PORTS_MODULE_ID index,
                             SYS_PORTS_PIN_DIRECTION pinDir,
                             PORTS_CHANNEL channel,
                             PORTS_DATA_MASK mask )
{
    if (pinDir == SYS_PORTS_DIRECTION_INPUT)
    {
        PLIB_PORTS_DirectionInputSet(index, channel, mask);
    }
    else
    {
        PLIB_PORTS_DirectionOutputSet(index, channel, mask);
    }
}


/******************************************************************************
  Function:
    PORTS_DATA_MASK SYS_PORTS_DirectionGet( PORTS_MODULE_ID index,
                                            PORTS_CHANNEL channel )

  Summary:
    Reads the port direction for the selected port.

  Description:
    This function reads the port direction for the selected port.

  Remarks:
    None.
*/

PORTS_DATA_MASK SYS_PORTS_DirectionGet( PORTS_MODULE_ID index, PORTS_CHANNEL channel )
{
    return PLIB_PORTS_DirectionGet( index, channel );
}


/******************************************************************************
  Function:
    void SYS_PORTS_Toggle( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                           PORTS_DATA_MASK toggleMask )

  Summary:
    Toggles the selected digital port pins.

  Description:
    This function toggles the selected digital port pins.

  Remarks:
    None.
*/

void SYS_PORTS_Toggle( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                       PORTS_DATA_MASK toggleMask )
{
    PLIB_PORTS_Toggle( index, channel, toggleMask );
}


/******************************************************************************
  Function:
    void SYS_PORTS_OpenDrainEnable( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                                     PORTS_DATA_MASK mask )

  Summary:
    Enables the open drain functionality for the selected port.

  Description:
    This function enables the open drain functionality for the selected port.

  Remarks:
    None.
*/

void SYS_PORTS_OpenDrainEnable( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                                PORTS_DATA_MASK mask )
{
#if defined(PLIB_PORTS_ExistsPortsOpenDrain)
    if(PLIB_PORTS_ExistsPortsOpenDrain(index))
    {
        PLIB_PORTS_OpenDrainEnable( index, channel, mask );
    }
#endif
}


/******************************************************************************
  Function:
    void SYS_PORTS_OpenDrainDisable( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                                     PORTS_DATA_MASK mask )

  Summary:
    Disables the open drain functionality for the selected port.

  Description:
    This function disables the open drain functionality for the selected port.

  Remarks:
    None.
*/

void SYS_PORTS_OpenDrainDisable( PORTS_MODULE_ID index, PORTS_CHANNEL channel,
                                 PORTS_DATA_MASK mask )
{
#if defined(PLIB_PORTS_ExistsPortsOpenDrain)
    if(PLIB_PORTS_ExistsPortsOpenDrain(index))
    {
        PLIB_PORTS_OpenDrainDisable( index, channel, mask );
    }
#endif
}

// *****************************************************************************
/* Function:
    PORTS_DATA_TYPE SYS_PORTS_InterruptStatusGet
    ( 
        PORTS_MODULE_ID index, 
        PORTS_CHANNEL channel 
    )

  Summary:
    Reads the data from the I/O port.
	<p><b>Implementation:</b> Dynamic</p>

  Description:
    This function reads the data from the I/O port.

*/

PORTS_DATA_TYPE SYS_PORTS_InterruptStatusGet
( 
    PORTS_MODULE_ID index, 
    PORTS_CHANNEL channel 
)
{
    SYS_ASSERT(false, "This API is not supported on this device");
    return 0;
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

void SYS_PORTS_ChangeNotificationGlobalEnable( PORTS_MODULE_ID index )
{
#if defined(PLIB_PORTS_ExistsChangeNotice)
    if(PLIB_PORTS_ExistsChangeNotice(index))
    {
        PLIB_PORTS_ChangeNoticeEnable( index );
    }
#endif
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

void SYS_PORTS_ChangeNotificationGlobalDisable( PORTS_MODULE_ID index )
{
#if defined(PLIB_PORTS_ExistsChangeNotice)
    if(PLIB_PORTS_ExistsChangeNotice(index))
    {
        PLIB_PORTS_ChangeNoticeDisable( index );
    }
#endif
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

void SYS_PORTS_GlobalChangeNotificationDisable( PORTS_MODULE_ID index )
{
#if defined(PLIB_PORTS_ExistsChangeNotice)
    if(PLIB_PORTS_ExistsChangeNotice(index))
    {
        PLIB_PORTS_ChangeNoticeDisable( index );
    }
#endif
}


/******************************************************************************
  Function:
    void SYS_PORTS_ChangeNotificationEnable( PORTS_MODULE_ID index,
                                             PORTS_CHANGE_NOTICE_PIN pinNum,
                                             SYS_PORTS_PULLUP_PULLDOWN_STATUS value )

  Summary:
    Enables the change notification for the selected port.

  Description:
    This function enables the change notification for the selected port.

  Remarks:
    None.
*/

void SYS_PORTS_ChangeNotificationEnable( PORTS_MODULE_ID index,
                                         PORTS_CHANGE_NOTICE_PIN pinNum,
                                         SYS_PORTS_PULLUP_PULLDOWN_STATUS value )
{
#if defined(PLIB_PORTS_ExistsChangeNoticePullUp)
    if(PLIB_PORTS_ExistsChangeNoticePullUp(index))
    {
        switch(value)
        {
            case SYS_PORTS_PULLUP_DISABLE:
                PLIB_PORTS_ChangeNoticePullUpDisable(index, pinNum);
                break;
            case SYS_PORTS_PULLUP_ENABLE:
                PLIB_PORTS_ChangeNoticePullUpEnable(index, pinNum);
                break;
        }
    }
#endif

#if defined(PLIB_PORTS_ExistsPinChangeNotice)
    if(PLIB_PORTS_ExistsPinChangeNotice(index))
    {
        PLIB_PORTS_PinChangeNoticeEnable( index, pinNum );
    }
#endif
}


/******************************************************************************
  Function:
    void SYS_PORTS_ChangeNotificationDisable( PORTS_MODULE_ID index,
                                              PORTS_CHANGE_NOTICE_PIN pinNum )

  Summary:
    Disables the change notification for the selected port.

  Description:
    This function disables the change notification for the selected port.

  Remarks:
    None.
*/

void SYS_PORTS_ChangeNotificationDisable( PORTS_MODULE_ID index,
                                          PORTS_CHANGE_NOTICE_PIN pinNum )
{
#if defined(PLIB_PORTS_ExistsPinChangeNotice)
    if(PLIB_PORTS_ExistsPinChangeNotice(index))
    {
        PLIB_PORTS_PinChangeNoticeDisable( index, pinNum );
    }
#endif
}


/******************************************************************************
  Function:
    void SYS_PORTS_ChangeNotificationInIdleModeEnable( PORTS_MODULE_ID index )

  Summary:
    Enables the change notification for the selected port in Sleep or Idle mode.

  Description:
    This function enables the change notification for the selected port in Sleep
    or Idle mode.

  Remarks:
    None.
*/

void SYS_PORTS_ChangeNotificationInIdleModeEnable( PORTS_MODULE_ID index )
{
#if defined(PLIB_PORTS_ExistsChangeNoticeInIdle)
    if(PLIB_PORTS_ExistsChangeNoticeInIdle(index))
    {
        PLIB_PORTS_ChangeNoticeInIdleEnable( index );
    }
#endif
}


// *****************************************************************************
/* Function:
    void SYS_PORTS_ChangeNotificationInIdleModeDisable( PORTS_MODULE_ID index)

  Summary:
    Disables the change notification for the selected port in Sleep or Idle mode.

  Description:
    This function disables the change notification for the selected port in Sleep
    or Idle mode.

  Remarks:
    None.
*/

void SYS_PORTS_ChangeNotificationInIdleModeDisable( PORTS_MODULE_ID index )
{
#if defined(PLIB_PORTS_ExistsChangeNoticeInIdle)
    if(PLIB_PORTS_ExistsChangeNoticeInIdle(index))
    {
        PLIB_PORTS_ChangeNoticeInIdleDisable( index );
    }
#endif
}


// *****************************************************************************
/* Function:
    void SYS_PORTS_ChangeNotificationPullUpEnable ( PORTS_MODULE_ID         index,
                                                    PORTS_CHANGE_NOTICE_PIN pinNum )

  Summary:
    Enables weak pull-up on change notification pin.

  Description:
    This function enables weak pull-up on change notification pin.

  Remarks:
    None.
*/

void SYS_PORTS_ChangeNotificationPullUpEnable ( PORTS_MODULE_ID         index,
                                                PORTS_CHANGE_NOTICE_PIN pinNum )
{
#if defined(PLIB_PORTS_ExistsChangeNoticePullUp)
    if(PLIB_PORTS_ExistsChangeNoticePullUp(index))
    {
        PLIB_PORTS_ChangeNoticePullUpEnable ( index, pinNum );
    }
#endif
}


// *****************************************************************************
/* Function:
    void SYS_PORTS_ChangeNotificationPullUpDisable ( PORTS_MODULE_ID         index,
                                                    PORTS_CHANGE_NOTICE_PIN pinNum )

  Summary:
    Disables pull-up on input change.

  Description:
    This function disables pull-up on input change.

  Remarks:
    None.
*/

void SYS_PORTS_ChangeNotificationPullUpDisable ( PORTS_MODULE_ID         index,
                                                PORTS_CHANGE_NOTICE_PIN pinNum )
{
#if defined(PLIB_PORTS_ExistsChangeNoticePullUp)
    if(PLIB_PORTS_ExistsChangeNoticePullUp(index))
    {
        PLIB_PORTS_ChangeNoticePullUpDisable ( index, pinNum );
    }
#endif
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
#if defined(PLIB_PORTS_ExistsPinMode)
    if(PLIB_PORTS_ExistsPinMode(index))
    {
        PLIB_PORTS_PinModeSelect ( index, pin, mode);
    }
#endif
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
    PLIB_PORTS_PinWrite ( index, channel, bitPos, value );
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
    SYS_ASSERT(false, "This API is not supported on this device");
    /* TODO: Call corresponding PLIB API */
    return false;
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
    return PLIB_PORTS_PinGet ( index, channel, bitPos );
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
    PLIB_PORTS_PinToggle ( index, channel, bitPos );
}


// *****************************************************************************
/* Function:
    void SYS_PORTS_PinSet( PORTS_MODULE_ID index,
                           PORTS_CHANNEL channel,
                           PORTS_BIT_POS bitPos )

  Summary:
    Sets the selected digital pin/latch.

  Description:
    This function sets the selected digital pin/latch.

  Remarks:
    None.
*/

void SYS_PORTS_PinSet( PORTS_MODULE_ID index,
                       PORTS_CHANNEL channel,
                       PORTS_BIT_POS bitPos )
{
    PLIB_PORTS_PinSet( index, channel, bitPos );
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
    PLIB_PORTS_PinClear ( index, channel, bitPos );
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
        PLIB_PORTS_PinDirectionOutputSet(index, channel, bitPos);
    }
    else
    {
        PLIB_PORTS_PinDirectionInputSet(index, channel, bitPos);
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
#if defined(PLIB_PORTS_ExistsPortsOpenDrain)
    if(PLIB_PORTS_ExistsPortsOpenDrain(index))
    {
        PLIB_PORTS_PinOpenDrainEnable ( index, channel, bitPos );
    }
#endif
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
#if defined(PLIB_PORTS_ExistsPortsOpenDrain)
    if(PLIB_PORTS_ExistsPortsOpenDrain(index))
    {
        PLIB_PORTS_PinOpenDrainDisable ( index, channel, bitPos );
    }
#endif
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
    SYS_ASSERT(false, "This API is not supported on this device");
    return;
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
    SYS_ASSERT(false, "This API is not supported on this device");
    return;
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
    SYS_ASSERT(false, "This API is not supported on this device");
    return;
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
    SYS_ASSERT(false, "This API is not supported on this device");
    return;
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
void SYS_PORTS_InterruptEnable
(
    PORTS_MODULE_ID index,
    PORTS_CHANNEL channel,
    PORTS_BIT_POS bitPos,
    PORTS_PIN_INTERRUPT_TYPE pinInterruptType
)
{
    SYS_ASSERT(false, "This API is not supported on this device");
    return;
}


// *****************************************************************************
/* Function:
    void SYS_PORTS_RemapInput( PORTS_MODULE_ID      index,
							  PORTS_REMAP_INPUT_FUNCTION function,
							  PORTS_REMAP_INPUT_PIN      remapPin )

  Summary:
    Input/Output (I/O) function remapping.

  Description:
    This function controls the I/O function remapping.

  Precondition:
    None.
*/	
void SYS_PORTS_RemapInput( PORTS_MODULE_ID      index,
						   PORTS_REMAP_INPUT_FUNCTION function,
						   PORTS_REMAP_INPUT_PIN      remapPin )
{
#if defined(PLIB_PORTS_ExistsRemapInput)
    if(PLIB_PORTS_ExistsRemapInput(index))
    {
        PLIB_DEVCON_SystemUnlock(DEVCON_ID_0);
        PLIB_DEVCON_DeviceRegistersUnlock(DEVCON_ID_0, DEVCON_PPS_REGISTERS);
        PLIB_PORTS_RemapInput( index, function, remapPin);
    }
#endif
}

// *****************************************************************************
/* Function:
    void SYS_PORTS_RemapOutput( PORTS_MODULE_ID      index,
                                      PORTS_REMAP_OUTPUT_FUNCTION function,
                                      PORTS_REMAP_OUTPUT_PIN      remapPin )

  Summary:
    Input/Output (I/O) function remapping.

  Description:
    This function controls the I/O function remapping.

  Precondition:
    None.
*/
void SYS_PORTS_RemapOutput( PORTS_MODULE_ID      index,
						    PORTS_REMAP_OUTPUT_FUNCTION function,
						    PORTS_REMAP_OUTPUT_PIN      remapPin )
{
#if defined(PLIB_PORTS_ExistsRemapOutput)
    if(PLIB_PORTS_ExistsRemapOutput(index))
    {
        PLIB_DEVCON_SystemUnlock(DEVCON_ID_0);
        PLIB_DEVCON_DeviceRegistersUnlock(DEVCON_ID_0, DEVCON_PPS_REGISTERS);
        PLIB_PORTS_RemapOutput( index, function, remapPin);
    }
#endif
}

/*******************************************************************************
 End of File
*/

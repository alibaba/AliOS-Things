/*****************************************************************************
 **
 **  Name    upio_bby.c
 **
 **  Description
 **  This file contains the universal driver wrapper for the BTE-QC pio
 **  drivers
 **
 *****************************************************************************/
#include "buildcfg.h"
#include "mico_bt_dev.h"
#include "include/upio.h"
#include "platform_bluetooth.h"

/******************************************************
 *               Variables Definitions
 ******************************************************/

//static volatile mico_bool_t bus_initialised = MICO_FALSE;
//static volatile mico_bool_t device_powered = MICO_FALSE;



/*******************************************************************************
 **  UPIO Driver functions
 *******************************************************************************/

/*****************************************************************************
 **
 ** Function         UPIO_Init
 **
 ** Description
 **      Initialize the GPIO service.
 **      This function is typically called once upon system startup.
 **
 ** Returns          nothing
 **
 *****************************************************************************/
UDRV_API void UPIO_Init( void *p_cfg )
{
    //DRV_TRACE_DEBUG0("UPIO_Init");
}

/*****************************************************************************
 **
 ** Function         UPIO_Set
 **
 ** Description
 **      This function sets one or more GPIO devices to the given state.
 **      Multiple GPIOs of the same type can be masked together to set more
 **      than one GPIO. This function can only be used on types UPIO_LED and
 **      UPIO_GENERAL.
 **
 ** Input Parameters:
 **      type    The type of device.
 **      pio     Indicates the particular GPIOs.
 **      state   The desired state.
 **
 ** Output Parameter:
 **      None.
 **
 ** Returns:
 **      None.
 **
 *****************************************************************************/
UDRV_API void UPIO_Set( tUPIO_TYPE type, tUPIO pio, tUPIO_STATE state )
{
    //DRV_TRACE_DEBUG2("UPIO_Set %d, %s", pio, UPIO_OFF == state ? "UPIO_OFF" : "UPIO_ON");
#if HCILP_INCLUDED
    if (UPIO_OFF == state) {
        platform_gpio_output_low( mico_bt_control_pins[MICO_BT_PIN_DEVICE_WAKE] );
    } else {
        platform_gpio_output_high( mico_bt_control_pins[MICO_BT_PIN_DEVICE_WAKE] );
    }
#endif
}

/*****************************************************************************
 **
 ** Function         UPIO_Read
 **
 ** Description
 **      Read the state of a GPIO. This function can be used for any type of
 **      device. Parameter pio can only indicate a single GPIO; multiple GPIOs
 **      cannot be masked together.
 **
 ** Input Parameters:
 **      Type:  The type of device.
 **      pio:    Indicates the particular GUPIO.
 **
 ** Output Parameter:
 **      None.
 **
 ** Returns:
 **      State of GPIO (UPIO_ON or UPIO_OFF).
 **
 *****************************************************************************/
UDRV_API tUPIO_STATE UPIO_Read( tUPIO_TYPE type, tUPIO pio )
{
#if HCILP_INCLUDED
    //DRV_TRACE_DEBUG1("UPIO_Read %d", pio);
    return platform_gpio_input_get(mico_bt_control_pins[MICO_BT_PIN_HOST_WAKE]) ? UPIO_ON : UPIO_OFF;
#else
    return UPIO_OFF;
#endif
}

/*****************************************************************************
 **
 ** Function         UPIO_Config
 **
 ** Description      - Configure GPIOs of type UPIO_GENERAL as inputs or outputs
 **                  - Configure GPIOs to be polled or interrupt driven
 **
 **
 ** Output Parameter:
 **      None.
 **
 ** Returns:
 **      None.
 **
 *****************************************************************************/
#if 0
UDRV_API void UPIO_Config( tUPIO_TYPE type, tUPIO pio, tUPIO_CONFIG config, tUPIO_CBACK *cback )
{
}
#endif
#if 0
mico_bool_t bt_bus_is_ready( void )
{
    return ( bus_initialised == MICO_FALSE ) ? MICO_FALSE : ( ( mico_gpio_input_get( BLUETOOTH_GPIO_CTS_PIN ) == MICO_TRUE )
                                                              ? MICO_FALSE : MICO_TRUE );
}
#endif

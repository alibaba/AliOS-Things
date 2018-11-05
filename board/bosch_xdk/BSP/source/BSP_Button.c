/*----------------------------------------------------------------------------*/
/*
 * Copyright (C) Bosch Connected Devices and Solutions GmbH. 
 * All Rights Reserved. Confidential.
 *
 * Distribution only to people who need to know this information in
 * order to do their job.(Need-to-know principle).
 * Distribution to persons outside the company, only if these persons
 * signed a non-disclosure agreement.
 * Electronic transmission, e.g. via electronic mail, must be made in
 * encrypted form.
 */
/*----------------------------------------------------------------------------*/

/**
 * @file
 * @brief  Implementation of Button BSP functions
 */

/* This is used to define the correct module ID for generation of module
 * error codes
 */
#include "BCDS_BSP.h"
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID BCDS_BSP_MODULE_ID_BUTTON

#include "BCDS_HAL.h"
#if BCDS_FEATURE_BSP_BUTTON

#include "BSP_BoardSettings.h"
#include "BSP_BoardShared.h"
#include "BSP_BoardType.h"
#include "BSP_Button.h"
#include "BCDS_Assert.h"
#include "BCDS_BSP_Button.h"

#define BUTTON_COUNT UINT32_C(2)

static BSP_Button_Callback_T Button1Event;
static BSP_Button_Callback_T Button2Event;

static struct ButtonsState
{
    bool Button1isEnabled :1;
    bool Button1UsesInterrupt :1;
    bool Button2isEnabled :1;
    bool Button2UsesInterrupt;
}XDK_ButtonState;

static void Button1InterruptCallback(void);
static void Button2InterruptCallback(void);

/*
 * Refer to interface header for description
 */
Retcode_T BSP_Button_Connect(void)
{
    GPIO_PinModeSet(KEY1_PORT, KEY1_PIN, KEY1_MODE, KEY1_PULL);
    GPIO_PinModeSet(KEY2_PORT, KEY2_PIN, KEY2_MODE, KEY2_PULL);
    return RETCODE_OK;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_Button_Enable(uint32_t buttonId, BSP_Button_Callback_T callback)
{
    Retcode_T retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_UNEXPECTED_BEHAVIOR);
    switch (buttonId)
    {
        case BSP_XDK_BUTTON_1:
        if (true == XDK_ButtonState.Button1isEnabled)
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_BUTTON_ENABLED);
        }
        else
        {
            /*lint -e506 wrong config results in runtime error reported*/
            if (1 == BSP_XDK_KEY1_INTERRUPT)
            {
                if (NULL == callback)
                {
                    retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
                }
                /*lint -e506 wrong config results in runtime error reported*/
                else if ((1 != BSP_XDK_KEY1_INTERRUPT_FORCE) && (NULL != Board_GPIOInterruptGetCallback(KEY1_PIN)))
                {
                    retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_BUTTON_INTERRUPT_BUSY);
                }
                else
                {
                    NVIC_DisableIRQ(GPIO_ODD_IRQn);
                    Button1Event = callback;
                    XDK_ButtonState.Button1isEnabled = true;
                    XDK_ButtonState.Button1UsesInterrupt = true;
                    Board_GPIOInterruptRegisterCallback(KEY1_PIN, Button1InterruptCallback);
                    GPIO_IntConfig(KEY1_PORT, KEY1_PIN, KEY1_EDGE_RISING, KEY1_EDGE_FALLING, BSP_ENABLE);
                    NVIC_EnableIRQ(GPIO_ODD_IRQn);
                    retcode = RETCODE_OK;
                }
            }
            else
            {
                XDK_ButtonState.Button1isEnabled = true;
                XDK_ButtonState.Button1UsesInterrupt = false;
                retcode = RETCODE_OK;
            }
        }

        break;
        case BSP_XDK_BUTTON_2:
        if (true == XDK_ButtonState.Button2isEnabled)
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_BUTTON_ENABLED);
        }
        else
        {
            /*lint -e506 wrong config results in runtime error reported*/
            if (1 == BSP_XDK_KEY2_INTERRUPT)
            {
                if (NULL == callback)
                {
                    retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
                }
                /*lint -e506 wrong config results in runtime error reported*/
                else if ((1 != BSP_XDK_KEY2_INTERRUPT_FORCE) && (NULL != Board_GPIOInterruptGetCallback(KEY2_PIN)))
                {
                    retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_BUTTON_INTERRUPT_BUSY);
                }
                else
                {
                    NVIC_DisableIRQ(GPIO_EVEN_IRQn);

                    Button2Event = callback;

                    XDK_ButtonState.Button2isEnabled = true;
                    XDK_ButtonState.Button2UsesInterrupt = true;

                    Board_GPIOInterruptRegisterCallback(KEY2_PIN, Button2InterruptCallback);

                    GPIO_IntConfig(KEY2_PORT, KEY2_PIN, KEY2_EDGE_RISING, KEY2_EDGE_FALLING, BSP_ENABLE);

                    NVIC_EnableIRQ(GPIO_EVEN_IRQn);

                    retcode = RETCODE_OK;
                }
            }
            else
            {
                XDK_ButtonState.Button2isEnabled = true;
                XDK_ButtonState.Button2UsesInterrupt = false;
                retcode = RETCODE_OK;
            }
        }
        break;
        default:
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
        break;
    }
    return retcode;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_Button_Disable(uint32_t buttonId)
{
    Retcode_T retcode = RETCODE_OK;
    switch (buttonId)
    {
        case BSP_XDK_BUTTON_1:
        XDK_ButtonState.Button1isEnabled = false;
        if (XDK_ButtonState.Button1UsesInterrupt)
        {
            Board_GPIOInterruptUnregisterCallback(KEY1_PIN);
            XDK_ButtonState.Button1UsesInterrupt = false;
        }
        break;
        case BSP_XDK_BUTTON_2:
        XDK_ButtonState.Button2isEnabled = false;
        if (XDK_ButtonState.Button2UsesInterrupt)
        {
            Board_GPIOInterruptUnregisterCallback(KEY2_PIN);
            XDK_ButtonState.Button2UsesInterrupt = false;
        }
        break;
        default:
    	retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    }
    return retcode;
}

/*
 * Refer to interface header for description
 */
Retcode_T BSP_Button_Disconnect(void)
{
    GPIO_PinModeSet(KEY1_PORT, KEY1_PIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(KEY1_PORT, KEY2_PIN, gpioModeDisabled, 0);
    return RETCODE_OK;
}

/*
 * Refer to interface header for description
 */
uint32_t BSP_Button_GetState(uint32_t buttonId)
{
    BSP_ButtonStatus_T status = BSP_XDK_BUTTON_ERROR;

    switch (buttonId)
    {
    case BSP_XDK_BUTTON_1:
        if (true == XDK_ButtonState.Button1isEnabled)
        {
            if (BSP_IO_PIN_HIGH == GPIO_PinInGet(KEY1_PORT, KEY1_PIN))
            {
                status = BSP_XDK_BUTTON_RELEASED;
            }
            else
            {
                status = BSP_XDK_BUTTON_PRESSED;
            }
        }
        break;
    case BSP_XDK_BUTTON_2:
        if (true == XDK_ButtonState.Button2isEnabled)
        {
            if (BSP_IO_PIN_HIGH == GPIO_PinInGet(KEY2_PORT, KEY2_PIN))
            {
                status = BSP_XDK_BUTTON_RELEASED;
            }
            else
            {
                status = BSP_XDK_BUTTON_PRESSED;
            }
        }
        break;
    default:
    status = BSP_XDK_BUTTON_ERROR;
    }
    return (uint32_t) status;
}

static void Button1InterruptCallback(void)
{
    if (XDK_ButtonState.Button1UsesInterrupt)
    {
        if (BSP_IO_PIN_LOW == GPIO_PinInGet(KEY1_PORT, KEY1_PIN))
        {
            Button1Event((uint32_t)BSP_XDK_BUTTON_PRESS);
        }
        else
        {
            Button1Event((uint32_t)BSP_XDK_BUTTON_RELEASE);
        }
    }
    else
    {
        assert(0);
    }
}

static void Button2InterruptCallback(void)
{
    if (XDK_ButtonState.Button2UsesInterrupt)
    {
        if (BSP_IO_PIN_LOW == GPIO_PinInGet(KEY2_PORT, KEY2_PIN))
        {
            Button2Event((uint32_t)BSP_XDK_BUTTON_PRESS);
        }
        else
        {
            Button2Event((uint32_t)BSP_XDK_BUTTON_RELEASE);
        }
    }
    else
    {
        assert(0);
    }
}
#endif /* BCDS_FEATURE_BSP_BUTTON */


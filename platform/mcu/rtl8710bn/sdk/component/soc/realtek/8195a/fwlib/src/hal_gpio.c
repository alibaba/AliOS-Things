/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "rtl8195a.h"

#ifdef CONFIG_GPIO_EN

HAL_GPIO_ADAPTER gHAL_Gpio_Adapter;
extern PHAL_GPIO_ADAPTER _pHAL_Gpio_Adapter;

extern VOID GPIO_PullCtrl_8195a(u32 chip_pin, u8 pull_type);

/**
  * @brief  To get the GPIO IP Pin name for the given chip pin name
  *
  * @param  chip_pin: The chip pin name.
  *
  * @retval The gotten GPIO IP pin name
  */
u32 
HAL_GPIO_GetPinName(
    u32 chip_pin
)
{
    return HAL_GPIO_GetIPPinName_8195a((u32)chip_pin);
}

/**
  * @brief  Set the GPIO pad Pull type
  *
  *  @param    pin: The pin for pull type control.
  *  @param    mode: the pull type for the pin.
  *  @return   None
  */
VOID 
HAL_GPIO_PullCtrl(
    u32 pin,
    u32 mode    
)
{
    u8 pull_type;

    switch (mode) {
        case hal_PullNone:
            pull_type = DIN_PULL_NONE;
            break;
            
        case hal_PullDown:
            pull_type = DIN_PULL_LOW;
            break;
            
        case hal_PullUp:
            pull_type = DIN_PULL_HIGH;
            break;
            
        case hal_OpenDrain:
        default:
            pull_type = DIN_PULL_NONE;
            break;
    }

//    HAL_GPIO_PullCtrl_8195a (pin, pull_type);
    GPIO_PullCtrl_8195a (pin, pull_type);
}


/**
  * @brief  Initializes a GPIO Pin by the GPIO_Pin parameters.
  *
  * @param  GPIO_Pin: The data structer which contains the parameters for the GPIO Pin initialization.
  *
  * @retval HAL_Status
  */
VOID 
HAL_GPIO_Init(
    HAL_GPIO_PIN  *GPIO_Pin
)
{
    u8 port_num;
    u8 pin_num;
    u32 chip_pin;
    HAL_Status ret;

    if (_pHAL_Gpio_Adapter == NULL) {
        _pHAL_Gpio_Adapter = &gHAL_Gpio_Adapter;
//        DBG_GPIO_INFO("HAL_GPIO_Init: Initial GPIO Adapter\n ");
    }

    port_num = HAL_GPIO_GET_PORT_BY_NAME(GPIO_Pin->pin_name);
    pin_num = HAL_GPIO_GET_PIN_BY_NAME(GPIO_Pin->pin_name);
    chip_pin = GPIO_GetChipPinName_8195a(port_num, pin_num);
    if ((_PA_0 != chip_pin) && (GpioFunctionChk(chip_pin, ENABLE) == _FALSE)) {
        DBG_GPIO_ERR("HAL_GPIO_Init: GPIO Pin(%x) Unavailable\n ", chip_pin);
        return;
    }

    // Make the pin pull control default as High-Z
    GPIO_PullCtrl_8195a(chip_pin, HAL_GPIO_HIGHZ);

    ret = HAL_GPIO_Init_8195a(GPIO_Pin);
    
    if (ret != HAL_OK) {
        GpioFunctionChk(chip_pin, DISABLE);
    }
}

/**
  * @brief  Initializes a GPIO Pin as a interrupt signal
  *
  * @param  GPIO_Pin: The data structer which contains the parameters for the GPIO Pin initialization.
  *
  * @retval HAL_Status
  */
VOID 
HAL_GPIO_Irq_Init(
    HAL_GPIO_PIN  *GPIO_Pin
)
{
    u8 port_num;
    u8 pin_num;
    u32 chip_pin;
    HAL_Status ret;
    
    if (_pHAL_Gpio_Adapter == NULL) {
        _pHAL_Gpio_Adapter = &gHAL_Gpio_Adapter;
//        DBG_GPIO_INFO("%s: Initial GPIO Adapter\n ", __FUNCTION__);
    }

    if (_pHAL_Gpio_Adapter->IrqHandle.IrqFun == NULL) {
        _pHAL_Gpio_Adapter->IrqHandle.IrqFun = (IRQ_FUN)HAL_GPIO_MbedIrqHandler_8195a;
        _pHAL_Gpio_Adapter->IrqHandle.Priority = 6;
        HAL_GPIO_RegIrq_8195a(&_pHAL_Gpio_Adapter->IrqHandle);        
        InterruptEn(&_pHAL_Gpio_Adapter->IrqHandle);
//        DBG_GPIO_INFO("%s: Initial GPIO IRQ Adapter\n ", __FUNCTION__);
    }

    port_num = HAL_GPIO_GET_PORT_BY_NAME(GPIO_Pin->pin_name);
    pin_num = HAL_GPIO_GET_PIN_BY_NAME(GPIO_Pin->pin_name);
    chip_pin = GPIO_GetChipPinName_8195a(port_num, pin_num);
    if (GpioFunctionChk(chip_pin, ENABLE) == _FALSE) {
        DBG_GPIO_ERR("HAL_GPIO_Irq_Init: GPIO Pin(%x) Unavailable\n ", chip_pin);
        return;
    }

    DBG_GPIO_INFO("HAL_GPIO_Irq_Init: GPIO(name=0x%x)(mode=%d)\n ", GPIO_Pin->pin_name, 
        GPIO_Pin->pin_mode);
    HAL_GPIO_MaskIrq_8195a(GPIO_Pin);
    ret = HAL_GPIO_Init_8195a(GPIO_Pin);
    if (ret != HAL_OK) {
        GpioFunctionChk(chip_pin, DISABLE);
    }    
}

/**
  * @brief  UnInitial GPIO Adapter
  *
  *
  * @retval HAL_Status
  */
VOID
HAL_GPIO_IP_DeInit(
    VOID
)
{
    if (_pHAL_Gpio_Adapter != NULL) {
        InterruptDis(&_pHAL_Gpio_Adapter->IrqHandle);
        HAL_GPIO_UnRegIrq_8195a(&_pHAL_Gpio_Adapter->IrqHandle);                
        _pHAL_Gpio_Adapter = NULL;
    }
    
}

/**
  * @brief  De-Initializes a GPIO Pin, reset it as default setting.
  *
  * @param  GPIO_Pin: The data structer which contains the parameters for the GPIO Pin.
  *
  * @retval HAL_Status
  */
VOID 
HAL_GPIO_DeInit(
    HAL_GPIO_PIN  *GPIO_Pin
)
{
    u8 port_num;
    u8 pin_num;
    u32 chip_pin;
    
    port_num = HAL_GPIO_GET_PORT_BY_NAME(GPIO_Pin->pin_name);
    pin_num = HAL_GPIO_GET_PIN_BY_NAME(GPIO_Pin->pin_name);
    chip_pin = GPIO_GetChipPinName_8195a(port_num, pin_num);
    HAL_GPIO_DeInit_8195a(GPIO_Pin);

    GpioFunctionChk(chip_pin, DISABLE);
}


#endif // CONFIG_GPIO_EN

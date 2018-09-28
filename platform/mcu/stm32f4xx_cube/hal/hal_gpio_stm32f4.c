/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "hal/hal.h"
#include "stm32f4xx_hal.h"
#include "hal_gpio_stm32f4.h"
#ifdef HAL_GPIO_MODULE_ENABLED

#define  GPIO_IRQ_SLOP_MAX  (16)

static gpio_irq_slop_t gGpioSlop[PINS_IN_GROUP]= {
    {.pin_num=-1},{.pin_num=-1},{.pin_num=-1},{.pin_num=-1},
    {.pin_num=-1},{.pin_num=-1},{.pin_num=-1},{.pin_num=-1},
    {.pin_num=-1},{.pin_num=-1},{.pin_num=-1},{.pin_num=-1},
    {.pin_num=-1},{.pin_num=-1},{.pin_num=-1},{.pin_num=-1},
};
    
static  int8_t  gpio_slop_irq(int32_t irq_num);
static  gpio_irq_slop_t * gpio_slop_get(int32_t pin_num);
static  int32_t Get_port_TrueAddress(uint16_t port,uint16_t *CurrentPosition);


static gpio_irq_slop_t * gpio_slop_get(int32_t pin_num)
{
    int32_t index = 0;
    
    for(index = 0; index < PINS_IN_GROUP; ++index) {
        if(pin_num == gGpioSlop[index].pin_num){
            return &gGpioSlop[index];
        }
    }
    
    return(NULL);
}

static int8_t  gpio_slop_irq(int32_t irq_num)
{
    int32_t index = 0;
    
    for(index=0; index < PINS_IN_GROUP; ++index) {
        if(irq_num != gGpioSlop[index].irq_num)
            continue;

        if(NULL == gGpioSlop[index].handler)
            continue;
        
        gGpioSlop[index].handler(gGpioSlop[index].priv);
    }
    
    return(0);
}


void EXTI0_IRQHandler(void)
{
    krhino_intrpt_enter();
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
    krhino_intrpt_exit();
}


void EXTI1_IRQHandler(void)
{
    krhino_intrpt_enter();
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
    krhino_intrpt_exit();
}


void EXTI2_IRQHandler(void)
{
#ifdef SOUND_LOCATION_ENABLE
    acoustic_sl_process_callback();
#else
    krhino_intrpt_enter();
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
    krhino_intrpt_exit();
#endif
}


void EXTI3_IRQHandler(void)
{
    krhino_intrpt_enter();
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
    krhino_intrpt_exit();
}


void EXTI4_IRQHandler(void)
{
    krhino_intrpt_enter();
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
    krhino_intrpt_exit();
}


void EXTI9_5_IRQHandler(void)
{
    krhino_intrpt_enter();
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
    krhino_intrpt_exit();
}


void EXTI15_10_IRQHandler(void)
{
    krhino_intrpt_enter();
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
    krhino_intrpt_exit();
}


static int32_t Get_port_TrueAddress(uint16_t port,uint16_t *CurrentPosition)
{
    uint8_t ret = 1;
    uint16_t i = 0;
    
    for(i = 0; i < GPIO_TABLE_SZ; i++) {
        if(port == GPIO_MAPPINGConfig[i].port) { 
            *CurrentPosition=i;
            ret=0;
            break;
        }
    }

    return ret;
}

int32_t hal_gpio_init(gpio_dev_t *gpio)
{
    GPIO_TypeDef *GPIOx = NULL;
    GPIO_InitTypeDef  GPIO_InitStruct;
    int16_t CurrentPosition;

    if (NULL == gpio) {
        printf("invalid input %s \r\n", __func__);
        return -1;
    }

    if (0 == Get_port_TrueAddress(gpio->port ,&CurrentPosition)) {
        
        GPIOx = GPIO_MAPPINGConfig[CurrentPosition].GpioGroup;
        GPIO_InitStruct.Pin = GPIO_MAPPINGConfig[CurrentPosition].Pin;
        GPIO_InitStruct.Speed = GPIO_MAPPINGConfig[CurrentPosition].Speed;
    
        if(IRQ_MODE == gpio->config) {
          switch (*(gpio_irq_trigger_t *)gpio->priv) { 
            case IRQ_TRIGGER_RISING_EDGE : 
                GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING; 
                break;
            case IRQ_TRIGGER_FALLING_EDGE : 
                GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING ;
                break;
            case IRQ_TRIGGER_BOTH_EDGES : 
                GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING; 
                break;
            default :
                printf("invalid irq mode %d \r\n ", *(gpio_irq_trigger_t *)gpio->priv);
                return -1;
            }
        } else {
            GPIO_InitStruct.Mode = GPIO_MAPPINGConfig[CurrentPosition].Mode;
        }
    
        GPIO_InitStruct.Pull = GPIO_MAPPINGConfig[CurrentPosition].Pull;

        HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);

        if( GPIO_InitStruct.Mode == GPIO_OUTPUT_PP || GPIO_InitStruct.Mode == GPIO_OUTPUT_OD) {
            HAL_GPIO_WritePin(GPIOx, GPIO_InitStruct.Pin,GPIO_MAPPINGConfig[CurrentPosition].defaultLogicalVal);
            return 0;
        }
    }
    
    return 0;
}

int32_t hal_gpio_output_toggle(gpio_dev_t *gpio)
{
    uint16_t CurrentPosition;

    if (NULL == gpio) {
        printf("invalid input in %s \r\n", __func__);
        return -1;
    }
    
    if(0 == Get_port_TrueAddress(gpio->port, &CurrentPosition)) {
        HAL_GPIO_TogglePin(GPIO_MAPPINGConfig[CurrentPosition].GpioGroup, GPIO_MAPPINGConfig[CurrentPosition].Pin);
    } else {
        printf("fail to get port info from logic port %d at %s \r\n", gpio->port, __func__);
        return -1;
    }

    return 0;
}


int32_t hal_gpio_output_high(gpio_dev_t *gpio)
{
     uint16_t CurrentPosition;
     
     if (NULL == gpio) {
         printf("invalid input in %s \r\n", __func__);
         return -1;
     }

    if(0 == Get_port_TrueAddress(gpio->port,&CurrentPosition)) {
        HAL_GPIO_WritePin(GPIO_MAPPINGConfig[CurrentPosition].GpioGroup, GPIO_MAPPINGConfig[CurrentPosition].Pin, GPIO_PinState_Set);
    } else {
        printf("fail to get port info from logic port %d at %s \r\n", gpio->port, __func__);
        return -1;
    }
    
    return 0;
}

int32_t hal_gpio_output_low(gpio_dev_t *gpio)	
{
     uint16_t CurrentPosition;
     
     if (NULL == gpio) {
         printf("invalid input in %s \r\n", __func__);
         return -1;
     }

    if(0 == Get_port_TrueAddress(gpio->port,&CurrentPosition)) {
        HAL_GPIO_WritePin(GPIO_MAPPINGConfig[CurrentPosition].GpioGroup, GPIO_MAPPINGConfig[CurrentPosition].Pin, GPIO_PinState_Reset);
    } else {
        printf("fail to get port info from logic port %d at %s \r\n", gpio->port, __func__);
        return -1;
    }
    
    return 0;
}

int32_t hal_gpio_input_get(gpio_dev_t *gpio, uint32_t *value)
{
    uint16_t CurrentPosition;

    if(0==Get_port_TrueAddress(gpio->port,&CurrentPosition)) {
        *value=HAL_GPIO_ReadPin(GPIO_MAPPINGConfig[CurrentPosition].GpioGroup, GPIO_MAPPINGConfig[CurrentPosition].Pin);
    } else {
        printf("fail to get port info from logic port %d at %s \r\n", gpio->port, __func__);
        return -1;
    }

    return 0;
}

  
int32_t hal_gpio_enable_irq(gpio_dev_t *gpio, gpio_irq_trigger_t trigger,gpio_irq_handler_t handler, void *arg)
{
    IRQn_Type IRQn;
    uint16_t CurrentPosition;
    gpio_irq_slop_t * slop = gpio_slop_get(-1);
    
    if(NULL == slop) 
        return(-1);
    
    if(0 == Get_port_TrueAddress(gpio->port,&CurrentPosition)&&IRQ_MODE==gpio->config) 
    {
        switch (GPIO_MAPPINGConfig[CurrentPosition].Pin)
        { 
            case GPIO_PIN_0:
                IRQn = EXTI0_IRQn;
                break;
            case GPIO_PIN_1:
                IRQn = EXTI1_IRQn;
                break;
            case GPIO_PIN_2:
                IRQn = EXTI2_IRQn;
                break;
            case GPIO_PIN_3:
                IRQn = EXTI3_IRQn;
                break;
            case GPIO_PIN_4:
                IRQn = EXTI4_IRQn;
                break;
            case GPIO_PIN_5:
            case GPIO_PIN_6:
            case GPIO_PIN_7:
            case GPIO_PIN_8:
            case GPIO_PIN_9:
                IRQn = EXTI9_5_IRQn;
                break;
            case GPIO_PIN_10:
            case GPIO_PIN_11:
            case GPIO_PIN_12:
            case GPIO_PIN_13:
            case GPIO_PIN_14:
            case GPIO_PIN_15:
                IRQn = EXTI15_10_IRQn;
                break;
        }
        HAL_NVIC_SetPriority(IRQn, 0, 0); // need discuss the priotity.
        HAL_NVIC_EnableIRQ(IRQn);
        slop->pin_num = gpio->port;
        slop->irq_num = GPIO_MAPPINGConfig[CurrentPosition].Pin;
        slop->handler = handler;
        slop->priv = arg;
        return (0);
    }
    return (-1);
}


int32_t hal_gpio_disable_irq(gpio_dev_t *gpio)
{
    uint16_t CurrentPosition;
    IRQn_Type IRQn = 0;
    gpio_irq_slop_t * slop = gpio_slop_get(-1);
    
    if(NULL == slop)
        return(-1);
    
    if(0 == Get_port_TrueAddress(gpio->port,&CurrentPosition)&&IRQ_MODE==gpio->config) {
        
        switch (GPIO_MAPPINGConfig[CurrentPosition].Pin) { 
            case GPIO_PIN_0:
                IRQn = EXTI0_IRQn;
                break;
            case GPIO_PIN_1:
                IRQn = EXTI1_IRQn;
                break;
            case GPIO_PIN_2:
                IRQn = EXTI2_IRQn;
                break;
            case GPIO_PIN_3:
                IRQn = EXTI3_IRQn;
                break;
            case GPIO_PIN_4:
                IRQn = EXTI4_IRQn;
                break;
            case GPIO_PIN_5:
            case GPIO_PIN_6:
            case GPIO_PIN_7:
            case GPIO_PIN_8:
            case GPIO_PIN_9:
                IRQn = EXTI9_5_IRQn;
                break;
            case GPIO_PIN_10:
            case GPIO_PIN_11:
            case GPIO_PIN_12:
            case GPIO_PIN_13:
            case GPIO_PIN_14:
            case GPIO_PIN_15:
                IRQn = EXTI15_10_IRQn;
                break;
        }
        HAL_NVIC_DisableIRQ(IRQn);
        return 0;
    }
    
    return (-1);
}


int32_t hal_gpio_clear_irq(gpio_dev_t *gpio)
{
    gpio_irq_slop_t * slop = gpio_slop_get(gpio->port);
    if(NULL == slop)return(-1);
    slop->pin_num = -1;
    slop->irq_num = -1;
    slop->handler = NULL;
    slop->priv = NULL;
    return 0;
}


int32_t hal_gpio_finalize(gpio_dev_t *gpio)
{
    return 0;
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    gpio_slop_irq(GPIO_Pin);
}


#endif


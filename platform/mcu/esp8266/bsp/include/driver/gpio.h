/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "c_types.h"
#include "esp8266/gpio_register.h"
#define GPIO_Pin_0              (BIT(0))  /* Pin 0 selected */
#define GPIO_Pin_1              (BIT(1))  /* Pin 1 selected */
#define GPIO_Pin_2              (BIT(2))  /* Pin 2 selected */
#define GPIO_Pin_3              (BIT(3))  /* Pin 3 selected */
#define GPIO_Pin_4              (BIT(4))  /* Pin 4 selected */
#define GPIO_Pin_5              (BIT(5))  /* Pin 5 selected */
#define GPIO_Pin_6              (BIT(6))  /* Pin 6 selected */
#define GPIO_Pin_7              (BIT(7))  /* Pin 7 selected */
#define GPIO_Pin_8              (BIT(8))  /* Pin 8 selected */
#define GPIO_Pin_9              (BIT(9))  /* Pin 9 selected */
#define GPIO_Pin_10             (BIT(10)) /* Pin 10 selected */
#define GPIO_Pin_11             (BIT(11)) /* Pin 11 selected */
#define GPIO_Pin_12             (BIT(12)) /* Pin 12 selected */
#define GPIO_Pin_13             (BIT(13)) /* Pin 13 selected */
#define GPIO_Pin_14             (BIT(14)) /* Pin 14 selected */
#define GPIO_Pin_15             (BIT(15)) /* Pin 15 selected */
#define GPIO_Pin_All            (0xFFFF)  /* All pins selected */

#define GPIO_PIN_REG_0          PERIPHS_IO_MUX_GPIO0_U
#define GPIO_PIN_REG_1          PERIPHS_IO_MUX_U0TXD_U
#define GPIO_PIN_REG_2          PERIPHS_IO_MUX_GPIO2_U
#define GPIO_PIN_REG_3          PERIPHS_IO_MUX_U0RXD_U
#define GPIO_PIN_REG_4          PERIPHS_IO_MUX_GPIO4_U
#define GPIO_PIN_REG_5          PERIPHS_IO_MUX_GPIO5_U
#define GPIO_PIN_REG_6          PERIPHS_IO_MUX_SD_CLK_U
#define GPIO_PIN_REG_7          PERIPHS_IO_MUX_SD_DATA0_U
#define GPIO_PIN_REG_8          PERIPHS_IO_MUX_SD_DATA1_U
#define GPIO_PIN_REG_9          PERIPHS_IO_MUX_SD_DATA2_U
#define GPIO_PIN_REG_10         PERIPHS_IO_MUX_SD_DATA3_U
#define GPIO_PIN_REG_11         PERIPHS_IO_MUX_SD_CMD_U
#define GPIO_PIN_REG_12         PERIPHS_IO_MUX_MTDI_U
#define GPIO_PIN_REG_13         PERIPHS_IO_MUX_MTCK_U
#define GPIO_PIN_REG_14         PERIPHS_IO_MUX_MTMS_U
#define GPIO_PIN_REG_15         PERIPHS_IO_MUX_MTDO_U

#define GPIO_PIN_REG(i) \
    (i==0) ? GPIO_PIN_REG_0:  \
    (i==1) ? GPIO_PIN_REG_1:  \
    (i==2) ? GPIO_PIN_REG_2:  \
    (i==3) ? GPIO_PIN_REG_3:  \
    (i==4) ? GPIO_PIN_REG_4:  \
    (i==5) ? GPIO_PIN_REG_5:  \
    (i==6) ? GPIO_PIN_REG_6:  \
    (i==7) ? GPIO_PIN_REG_7:  \
    (i==8) ? GPIO_PIN_REG_8:  \
    (i==9) ? GPIO_PIN_REG_9:  \
    (i==10)? GPIO_PIN_REG_10: \
    (i==11)? GPIO_PIN_REG_11: \
    (i==12)? GPIO_PIN_REG_12: \
    (i==13)? GPIO_PIN_REG_13: \
    (i==14)? GPIO_PIN_REG_14: \
    GPIO_PIN_REG_15

#define GPIO_PIN_ADDR(i)        (GPIO_PIN0_ADDRESS + i*4)

#define GPIO_ID_IS_PIN_REGISTER(reg_id) \
    ((reg_id >= GPIO_ID_PIN0) && (reg_id <= GPIO_ID_PIN(GPIO_PIN_COUNT-1)))

#define GPIO_REGID_TO_PINIDX(reg_id) ((reg_id) - GPIO_ID_PIN0)

typedef enum {
    GPIO_PIN_INTR_DISABLE = 0,      /**< disable GPIO interrupt */
    GPIO_PIN_INTR_POSEDGE = 1,      /**< GPIO interrupt type : rising edge */
    GPIO_PIN_INTR_NEGEDGE = 2,      /**< GPIO interrupt type : falling edge */
    GPIO_PIN_INTR_ANYEDGE = 3,      /**< GPIO interrupt type : bothe rising and falling edge */
    GPIO_PIN_INTR_LOLEVEL = 4,      /**< GPIO interrupt type : low level */
    GPIO_PIN_INTR_HILEVEL = 5       /**< GPIO interrupt type : high level */
} GPIO_INT_TYPE;

typedef enum {
    GPIO_Mode_Input = 0x0,          /**< GPIO mode : Input */
    GPIO_Mode_Out_OD,               /**< GPIO mode : Output_OD */
    GPIO_Mode_Output ,              /**< GPIO mode : Output */
    GPIO_Mode_Sigma_Delta ,         /**< GPIO mode : Sigma_Delta */
} GPIOMode_TypeDef;

typedef enum {
    GPIO_PullUp_DIS = 0x0,      /**< disable GPIO pullup */
    GPIO_PullUp_EN  = 0x1,      /**< enable GPIO pullup */
} GPIO_Pullup_IF;

typedef struct {
    uint16           GPIO_Pin;      /**< GPIO pin */
    GPIOMode_TypeDef GPIO_Mode;     /**< GPIO mode */
    GPIO_Pullup_IF   GPIO_Pullup;   /**< GPIO pullup */
    GPIO_INT_TYPE    GPIO_IntrType; /**< GPIO interrupt type */
} GPIO_ConfigTypeDef;

/** \defgroup Driver_APIs Driver APIs
  * @brief Driver APIs
  */

/** @addtogroup Driver_APIs
  * @{
  */

/** \defgroup GPIO_Driver_APIs GPIO Driver APIs
  * @brief GPIO APIs
  */

/** @addtogroup GPIO_Driver_APIs
  * @{
  */

/**  
  * @brief   Set GPIO pin output level.
  * 
  * @param   gpio_no   : The GPIO sequence number.
  * @param   bit_value : GPIO pin output level.  
  *  
  * @return  null
  */
#define GPIO_OUTPUT_SET(gpio_no, bit_value) \
    gpio_output_conf(bit_value<<gpio_no, ((~bit_value)&0x01)<<gpio_no, 1<<gpio_no, 0)

/**  
  * @brief   Set GPIO pin output level.
  * 
  * @param   gpio_bits : The GPIO bit number.
  * @param   bit_value : GPIO pin output level.  
  *  
  * @return  null
  */
#define GPIO_OUTPUT(gpio_bits, bit_value) \
    if(bit_value) gpio_output_conf(gpio_bits, 0, gpio_bits, 0);\
    else gpio_output_conf(0, gpio_bits, gpio_bits, 0)

/**  
  * @brief   Disable GPIO pin output.
  * 
  * @param   gpio_no : The GPIO sequence number.
  *  
  * @return  null
  */
#define GPIO_DIS_OUTPUT(gpio_no)    gpio_output_conf(0, 0, 0, 1<<gpio_no)

/**  
  * @brief   Enable GPIO pin intput.
  * 
  * @param   gpio_bits : The GPIO bit number.
  *  
  * @return  null
  */
#define GPIO_AS_INPUT(gpio_bits)    gpio_output_conf(0, 0, 0, gpio_bits)

/**  
  * @brief   Enable GPIO pin output.
  * 
  * @param   gpio_bits : The GPIO bit number.
  *  
  * @return  null
  */
#define GPIO_AS_OUTPUT(gpio_bits)   gpio_output_conf(0, 0, gpio_bits, 0)

/**  
  * @brief   Sample the level of GPIO input.
  * 
  * @param   gpio_no : The GPIO sequence number.
  *  
  * @return  the level of GPIO input 
  */
#define GPIO_INPUT_GET(gpio_no)     ((gpio_input_get()>>gpio_no)&BIT(0))

/**  
  * @brief   Enable GPIO16 output.
  * 
  * @param   null
  *  
  * @return  null
  */
void gpio16_output_conf(void);

/**  
  * @brief   Set GPIO16 output level.
  * 
  * @param   uint8 value : GPIO16 output level.
  *  
  * @return  null
  */
void gpio16_output_set(uint8 value);

/**  
  * @brief   Enable GPIO pin intput.
  * 
  * @param   null
  *  
  * @return  null
  */
void gpio16_input_conf(void);

/**  
  * @brief   Sample the value of GPIO16 input.
  * 
  * @param   null
  *  
  * @return  the level  of GPIO16 input.
  */
uint8 gpio16_input_get(void);

/**  
  * @brief   Configure Gpio pins out or input.
  * 
  * @param   uint32 set_mask     : Set the output for the high bit, the
  *                                corresponding bit is 1, the output of high,
  *                                the corresponding bit is 0, do not change the state.
  * @param   uint32 set_mask     : Set the output for the high bit, the
  *                                corresponding bit is 1, the output of low,
  *                                the corresponding bit is 0, do not change the state.
  * @param   uint32 enable_mask  : Enable Output
  * @param   uint32 disable_mask : Enable Input
  *
  * @return  null
  */
void gpio_output_conf(uint32 set_mask, uint32 clear_mask, uint32 enable_mask, uint32 disable_mask);

/**  
  * @brief   Register an application-specific interrupt handler for GPIO pin interrupts.
  * 
  * @param   void *fn:interrupt handler for GPIO pin interrupts.
  * @param   void *arg:interrupt handler's arg
  *  
  * @return  null
  */
void gpio_intr_handler_register(void *fn, void *arg);

/**  
  * @brief   Configure GPIO wake up to light sleep,Only level way is effective.
  * 
  * @param   uint32 i : Gpio sequence number
  * @param   GPIO_INT_TYPE intr_state : the level of wake up to light sleep
  *  
  * @return  null
  */
void gpio_pin_wakeup_enable(uint32 i, GPIO_INT_TYPE intr_state);

/**  
  * @brief   Disable GPIO wake up to light sleep.
  * 
  * @param   null
  *  
  * @return  null
  */
void gpio_pin_wakeup_disable();

/**  
  * @brief   Config interrupt types of GPIO pin.
  * 
  * @param   uint32 i : The GPIO sequence number.
  * @param   GPIO_INT_TYPE intr_state : GPIO interrupt types.
  *  
  * @return  null
  */
void gpio_pin_intr_state_set(uint32 i, GPIO_INT_TYPE intr_state);

/**  
  * @brief   Sample the value of GPIO input pins and returns a bitmask.
  * 
  * @param   null
  *  
  * @return  bitmask of GPIO pins input 
  */
uint32 gpio_input_get(void);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif

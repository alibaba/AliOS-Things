/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/******************************************************************************
 * @file     devices.c
 * @brief    source file for the devices
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#include "soc.h"
#include "config.h"
#include <drv_usart.h>
#include <stdio.h>


#include "pin_name.h"
#include "pinmux.h"

//typedef int32_t pin_t;



#if CONFIG_USART
struct {
    uint32_t base;
    uint32_t irq;
}
const sg_usart_config[CONFIG_USART_NUM] = {
    {CSKY_UART0_BASE, UART0_IRQn},
    {CSKY_UART1_BASE, UART1_IRQn},
    {CSKY_UART2_BASE, UART2_IRQn},
    {CSKY_UART3_BASE, UART3_IRQn}
};
typedef struct {
    pin_t    tx;
    pin_t    rx;
    uint16_t cfg_idx;    //idx of sg_usart_config[]
    uint16_t function;
} usart_pin_map_t;
const static usart_pin_map_t s_usart_pin_map[] = {
    {
        PA8_UART0TX_XX_SPU2_SIROUT0,
        PA9_UART0RX_XX_SPU3_SIRIN0,
        0,
        0
    },
    {
        PA21_UART1TX_PWM1_SPU9_SIROUT1,
        PA20_UART1RX_PWM0_SPU8_SIRIN1,
        1,
        0
    },
    {
        PA0_I2C0SCL_SPI1CS1_SPU0_UART1TX,
        PA1_I2C0SDA_SPI1CS2_SPU1_UART1RX,
        1,
        4,
    },
    {
        PB0_UART2TX_XX_XX_SIROUT2,
        PB1_UART2RX_XX_XX_SIRIN2,
        2,
        0
    },
    {
        PB13_UART3TX_SPI1MISO_SPU29_SIROUT3,
        PB12_UART3RX_SPI1CS0_SPU28_SIRIN3,
        3,
        0
    }
};

/**
  \param[in]   instance idx, must not exceed return value of target_get_usart_count()
  \brief       get usart instance.
  \return      pointer to usart instance
*/
int32_t target_usart_init(pin_t tx, pin_t rx, uint32_t *base, uint32_t *irq)
{
    uint32_t idx;

    for (idx = 0; idx < sizeof(s_usart_pin_map) / sizeof(usart_pin_map_t); idx++) {
        if (s_usart_pin_map[idx].tx == tx && s_usart_pin_map[idx].rx == rx) {
            *base = sg_usart_config[s_usart_pin_map[idx].cfg_idx].base;
            *irq = sg_usart_config[s_usart_pin_map[idx].cfg_idx].irq;
            /*pinmux*/
            pin_mux(s_usart_pin_map[idx].tx, s_usart_pin_map[idx].function);
            pin_mux(s_usart_pin_map[idx].rx, s_usart_pin_map[idx].function);
            return s_usart_pin_map[idx].cfg_idx;
        }
    }

    return -1;

}
#endif

#if CONFIG_SPU
const pin_t spu_pin_map[CONFIG_SPU_NUM][32]=
{
  {
    PA0_I2C0SCL_SPI1CS1_SPU0_UART1TX,//pgpio0
    PA1_I2C0SDA_SPI1CS2_SPU1_UART1RX,//pgpio1
    PA8_UART0TX_XX_SPU2_SIROUT0,//pgpio2
    PA9_UART0RX_XX_SPU3_SIRIN0,//pgpio3
    -1,//pgpio4
    -1,//pgpio5
    -1,//pgpio6
    -1,//pgpio7
    PA20_UART1RX_PWM0_SPU8_SIRIN1,//pgpio8
    PA21_UART1TX_PWM1_SPU9_SIROUT1,//pgpio9
    -1,//pgpio10
    -1,//pgpio11
    -1,//pgpio12
    -1,//pgpio13
    -1,//pgpio14
    -1,//pgpio15
    -1,//pgpio16
    -1,//pgpio17
    PA30_I2C0SDA_PWM10_SPU18_XX,//pgpio18
    PA31_I2C0SCL_PWM11_SPU19_XX,//pgpio19
    -1,//pgpio20
    -1,//pgpio21
    -1,//pgpio22
    -1,//pgpio23
    -1,//pgpio24
    -1,//pgpio25
    -1,//pgpio26
    -1,//pgpio27
    -1,//pgpio28
    -1,//pgpio29
    -1,//pgpio30
    -1,//pgpio31
  }
};

const uint32_t spu_config[CONFIG_SPU_NUM][2]=
{
  {
    CSKY_SPU_BASE,
    SPU_IRQn
  }
};

int32_t target_spu_init(pin_t pgpio[],uint32_t *base, uint32_t *irq)
{
  int i,j;

  for(i=0;i<CONFIG_SPU_NUM;i++){
    j=0;

    while(j<32){
      if(pgpio[j]!=spu_pin_map[i][j]){
        break;
      }
      j++;
    }

    if(j==32){
      *base = spu_config[i][0];
      *irq = spu_config[i][1];
      return i;
    }
  }

  return -1;
}
#endif

#if CONFIG_GPIO
struct {
    uint32_t base;
    uint32_t irq;
    uint32_t pin_num;
    port_name_t port;
}
const sg_gpio_config[CONFIG_GPIO_NUM] = {
    {CSKY_GPIO0_BASE, GPIOA_IRQn, 32, PORTA},
    {CSKY_GPIO1_BASE, GPIOB_IRQn, 11, PORTB}
};

typedef struct {
    pin_t    gpio_pin;
    uint32_t cfg_idx;    //idx of sg_gpio_config[]
} gpio_pin_map_t;
const static gpio_pin_map_t s_gpio_pin_map[] = {
    {PA0_I2C0SCL_SPI1CS1_SPU0_UART1TX ,0},
    {PA1_I2C0SDA_SPI1CS2_SPU1_UART1RX,0},
    {PA2_QSPI0CLK_XX_XX_XX,0},
    {PA3_QSPI0MISO_XX_XX_XX,0},
    {PA4_QSPI0MOSI_XX_XX_XX,0},
    {PA5_QSPI0HOLD_XX_XX_XX,0},
    {PA6_QSPI0WP_XX_XX_XX,0},
    {PA7_QSPI0CS0_XX_XX_XX,0},
    {PA8_UART0TX_XX_SPU2_SIROUT0,0},
    {PA9_UART0RX_XX_SPU3_SIRIN0,0},
    {PA10_UART0CTS_USI0SCLK_SPU4_I2C0SCL,0},
    {PA11_UART0RTS_USI0SD0_SPU5_I2C0SDA,0},
    {PA12_XX_USI0SD1_XX_UART2RX,0},
    {PA13_XX_USI0NSS_XX_UART2TX,0},
    {PA14_SPI0CS2_FAULT_I2C1SDA_XX,0},
    {PA15_SPI0CS1_XX_I2C1SCL_XX,0},
    {PA16_SPI0CS0_PWMTRIG0_XX_USI1SCLK,0},
    {PA17_SPI0MOSI_PWMTRIG1_XX_USI1SD0,0},
    {PA18_SPI0MISO_XX_SPU6_USI1SD1,0},
    {PA19_SPI0SCK_FAULT_SPU7_USI1NSS,0},
    {PA20_UART1RX_PWM0_SPU8_SIRIN1,0},
    {PA21_UART1TX_PWM1_SPU9_SIROUT1,0},
    {PA22_UART1CTS_PWM2_SPU10_XX,0},
    {PA23_UART1RTS_PWM3_SPU11_XX,0},
    {PA24_USI1NSS_PWM4_SPU12_XX,0},
    {PA25_USI1SD1_PWM5_SPU13_XX,0},
    {PA26_USI1SD0_PWM6_SPU14_XX,0},
    {PA27_USI1SCLK_PWM7_SPU15_XX,0},
    {PA28_I2C1SCL_PWM8_SPU16_XX,0},
    {PA29_I2C1SDA_PWM9_SPU17_XX,0},
    {PA30_I2C0SDA_PWM10_SPU18_XX,0},
    {PA31_I2C0SCL_PWM11_SPU19_XX,0},
    {PB0_UART2TX_XX_XX_SIROUT2,1},
    {PB1_UART2RX_XX_XX_SIRIN2,1},
    {PB2_UART2RTS_XX_XX_XX,1},
    {PB3_UART2CTS_XX_XX_XX,1},
    {PB4_XX_XX_SPU20_UART3TX,1},
    {PB5_QSPI1CS1_XX_SPU21_UART3RX,1},
    {PB6_QSPI1WP_XX_SPU22_XX,1},
    {PB7_QSPI1HOLD_XX_SPU23_XX,1},
    {PB8_QSPI1CS0_PWMTRIG0_SPU24_XX,1},
    {PB9_QSPI1MOSI_PWMTRIG1_SPU25_XX,1},
    {PB10_QSPI1MISO_XX_SPU26_I2C1SDA,1},
    {PB11_QSPI1CLK_XX_SPU27_I2C1SCL,1},
    {PB12_UART3RX_SPI1CS0_SPU28_SIRIN3,1},
    {PB13_UART3TX_SPI1MISO_SPU29_SIROUT3,1},
    {PB14_UART3RTS_SPI1MOSI_SPU30_XX,1},
    {PB15_UART3CTS_SPI1SCK_SPU31_XX,1}
};

int32_t target_gpio_port_init(port_name_t port, uint32_t *base, uint32_t *irq, uint32_t *pin_num)
{
    int i;

    for (i = 0; i < CONFIG_GPIO_NUM; i++) {
        if (sg_gpio_config[i].port == port) {
            *base = sg_gpio_config[i].base;
            *irq = sg_gpio_config[i].irq;
            *pin_num = sg_gpio_config[i].pin_num;
            return i;
        }
    }

    return -1;
}

/**
  \param[in]   instance idx, must not exceed return value of target_get_gpio_count()
  \brief       get gpio instance.
  \return      pointer to gpio instance
*/
int32_t target_gpio_pin_init(pin_t gpio_pin, uint32_t *port_idx)
{
    uint32_t idx;

    for (idx = 0; idx < sizeof(s_gpio_pin_map) / sizeof(gpio_pin_map_t); idx++) {
        if (s_gpio_pin_map[idx].gpio_pin == gpio_pin) {
            *port_idx = s_gpio_pin_map[idx].cfg_idx;
            /*pinmux*/
            pin_mux(s_gpio_pin_map[idx].gpio_pin, 0xff);
            return idx;
        }
    }

    return -1;
}

#if CONFIG_TIMER
struct {
    uint32_t base;
    uint32_t irq;
}
const sg_timer_config[CONFIG_TIMER_NUM] = {
    {CSKY_TIM0_BASE, TIMA0_IRQn},
    {CSKY_TIM0_BASE + 0x14, TIMA1_IRQn},
    {CSKY_TIM1_BASE, TIMB0_IRQn},
    {CSKY_TIM1_BASE + 0x14, TIMB1_IRQn}

};

int32_t target_get_timer_count(void)
{
    return CONFIG_TIMER_NUM;
}

int32_t target_get_timer(int32_t idx, uint32_t *base, uint32_t *irq)
{
    if (idx >= target_get_timer_count()) {
        return NULL;
    }

    *base = sg_timer_config[idx].base;
    *irq = sg_timer_config[idx].irq;
    return idx;
}

#endif

#endif


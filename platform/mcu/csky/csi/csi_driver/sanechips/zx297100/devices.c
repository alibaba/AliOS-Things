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
#include <csi_config.h>
#include <drv_usart.h>
#include <drv_trng.h>
#include <drv_aes.h>
#include <drv_rsa.h>
#include <drv_sha.h>
#include <stdio.h>
#include "pin_name.h"
#include "pinmux.h"
#include "soc.h"
#include "drv_eflash.h"
#include <drv_rtc.h>

struct {
    uint32_t base;
    uint32_t irq;
}
const sg_iic_config[CONFIG_IIC_NUM] = {
    {ZX29_I2C0_BASE, PIC_IRQ_I2C0},
#ifdef CONFIG_HAVE_ICU
    {ZX29_I2C1_BASE, ICU_IRQ_I2C1}
#endif
};


typedef struct {
    int32_t    scl;
    int32_t    sda;
    uint32_t cfg_idx;    //idx of sg_iic_config[]
} iic_pin_map_t;
const static iic_pin_map_t s_iic_pin_map[] = {
    {
        GPIO6_SCL0,
        GPIO7_SDA0,
        0
    },
    {
        GPIO4_SCL1,
        GPIO5_SDA1,
        1
    },
};


/**
  \param[in]   instance idx, must not exceed return value of target_get_iic_count()
  \brief       get iic instance.
  \return      pointer to iic instance
*/
int32_t target_iic_init(int32_t idx, uint32_t *base, uint32_t *irq)
{
    if (idx >= sizeof(s_iic_pin_map) / sizeof(iic_pin_map_t)) {
        return -1;
    }

    *base = sg_iic_config[s_iic_pin_map[idx].cfg_idx].base;
    *irq = sg_iic_config[s_iic_pin_map[idx].cfg_idx].irq;
    /*pinmux*/
    pin_mux((s_iic_pin_map[idx].scl >> 24) & 0xff, s_iic_pin_map[idx].scl);
    pin_mux((s_iic_pin_map[idx].sda >> 24) & 0xff, s_iic_pin_map[idx].sda);
    return s_iic_pin_map[idx].cfg_idx;

}

struct {
    uint32_t base;
    uint32_t irq;
}
const sg_usart_config[CONFIG_USART_NUM] = {
    {ZX29_UART0_BASE, ICU_IRQ_UART0},
    {ZX29_UART1_BASE, PIC_IRQ_UART1},
    {ZX29_UART2_BASE, ICU_IRQ_UART2},
    {ZX29_UART3_BASE, PIC_IRQ_LP_UART}
};
typedef struct {
    int32_t    tx;
    int32_t    rx;
    uint16_t cfg_idx;    //idx of sg_usart_config[]
    uint16_t function;
} usart_pin_map_t;
const static usart_pin_map_t s_usart_pin_map[] = {
    {
        GPIO0_UART0_TXD,
        GPIO1_UART0_RXD,
        0,
        0
    },
    {
        98,     /* invalid pin */
        97,     /* invalid pin */
        1,
        0
    },
    {
        GPIO3_UART2_TXD,
        GPIO3_UART2_RXD,
        2,
        0
    }
};

/**
  \param[in]   instance idx, must not exceed return value of target_get_usart_count()
  \brief       get usart instance.
  \return      pointer to usart instance
*/
int32_t target_usart_init(int32_t idx, uint32_t *base, uint32_t *irq)
{
    if (idx >= sizeof(s_usart_pin_map) / sizeof(usart_pin_map_t)) {
        return -1;
    }

    *base = sg_usart_config[s_usart_pin_map[idx].cfg_idx].base;
    *irq = sg_usart_config[s_usart_pin_map[idx].cfg_idx].irq;
    /*pinmux*/
    if(s_usart_pin_map[idx].cfg_idx == 0){
        pin_mux((s_usart_pin_map[idx].tx >> 24) & 0xff, s_usart_pin_map[idx].tx);
        pin_mux((s_usart_pin_map[idx].rx >> 24) & 0xff, s_usart_pin_map[idx].rx);

        /* clk sel ,default 19.2m  00:19.2m;01:76.8m;10,11:32k*/
        *((int *)(LSP_CRPM_BASEADDR+0x04)) &= ~(0x3<<12);
        /*enable clk*/
        //temp |= (0x3<<8)|(0x7<<4);           //clk auto gate
        *((int *)(LSP_CRPM_BASEADDR+0x04)) |= (0x7<<4);
    }
    return s_usart_pin_map[idx].cfg_idx;
}

typedef struct {
    int32_t    gpio_pin;
    uint32_t cfg_idx;    //idx of sg_gpio_config[]
    uint8_t irq;
} gpio_pin_map_t;
const static gpio_pin_map_t s_gpio_pin_map[] = {
    {GPIO0_GPIO0, 0, 0xff},
    {GPIO1_GPIO1, 0, 0xff},
    {GPIO2_GPIO2, 0, 0xff},
    {GPIO3_GPIO3, 0, 0xff},
    {GPIO4_GPIO4, 0, 0xff},
    {GPIO5_GPIO5, 0, 0xff},
    {GPIO6_EXT_INT0, 0, 29},
    {GPIO7_EXT_INT1, 0, 55},
    {GPIO8_GPIO8, 1, 0xff},
    {GPIO9_GPIO9, 1, 0xff},
    {GPIO10_GPIO10, 1, 0xff},
    {GPIO11_GPIO11, 1, 0xff},
    {GPIO12_GPIO12, 1, 0xff},
    {GPIO13_EXT_INT2, 1, 37},
    {GPIO14_EXT_INT3, 1, 38},
    {GPIO15_EXT_INT4, 1, 39},
    {GPIO16_GPIO16, 1, 0xff},
    {GPIO17_GPIO17, 1, 0xff},
    {GPIO18_EXT_INT5, 1, 40},
    {GPIO19_EXT_INT6, 1, 41},
    {GPIO20_EXT_INT7, 1, 42},
    {GPIO21_GPIO21, 1, 0xff},
    {GPIO22_GPIO22, 1, 0xff},
    {GPIO23_GPIO23, 1, 0xff},
    {GPIO24_GPIO24, 1, 0xff},
    {GPIO25_GPIO25, 1, 0xff},
    {GPIO26_GPIO26, 1, 0xff},
    {GPIO27_GPIO27, 1, 0xff},
    {GPIO28_GPIO28, 1, 0xff},
    {GPIO31_GPIO31, 1, 0xff},
    {GPIO32_GPIO32, 1, 0xff},
    {GPIO33_GPIO33, 1, 0xff},
    {GPIO34_GPIO34, 1, 0xff},
    {GPIO35_GPIO35, 1, 0xff},
    {GPIO36_GPIO36, 1, 0xff},
    {GPIO37_GPIO37, 1, 0xff},
    {GPIO38_GPIO38, 1, 0xff},
    {GPIO39_GPIO39, 1, 0xff},
    {GPIO40_GPIO40, 1, 0xff},
    {GPIO41_GPIO41, 1, 0xff},
    {GPIO44_GPIO44, 1, 0xff},
    {GPIO45_GPIO45, 1, 0xff},
    {GPIO46_GPIO46, 1, 0xff},
    {GPIO47_GPIO47, 1, 0xff},
    {GPIO48_GPIO48, 1, 0xff},
    {GPIO49_GPIO49, 1, 0xff},
    {GPIO50_GPIO50, 1, 0xff},
    {GPIO53_GPIO53, 1, 0xff},
    {GPIO54_GPIO54, 1, 0xff}
};

/**
  \param[in]   instance idx, must not exceed return value of target_get_gpio_count()
  \brief       get gpio instance.
  \return      pointer to gpio instance
*/
int32_t target_gpio_pin_init(int32_t gpio_pin, uint32_t *port_idx, uint8_t *irq)
{
    uint32_t idx;

    for (idx = 0; idx < sizeof(s_gpio_pin_map) / sizeof(gpio_pin_map_t); idx++) {
        if ((s_gpio_pin_map[idx].gpio_pin >> 24) == (gpio_pin >> 24)) {
            *port_idx = s_gpio_pin_map[idx].cfg_idx;
            if (s_gpio_pin_map[idx].irq != 0xff) {
                *irq = s_gpio_pin_map[idx].irq;
            }
            /*pinmux*/
            pin_mux((s_gpio_pin_map[idx].gpio_pin >> 24) & 0xff, s_gpio_pin_map[idx].gpio_pin);
            return idx;
        }
    }

    return -1;
}

struct {
    uint32_t base;
    eflash_info_t info;
    uint32_t irq;
}
const sg_eflash_config[CONFIG_EFLASH_NUM] = {
    {EFLASH1_CTRL_BASEADDR, {EFLASH1_BASEADDR, EFLASH1_BASEADDR+1216*1024, 608}, PIC_IRQ_EFC1},
    {EFLASH2_CTRL_BASEADDR, {EFLASH2_BASEADDR, EFLASH2_BASEADDR+256*1024, 512}, PIC_IRQ_EFC2}
};

/**
  \param[in]   instance idx, must not exceed return value of target_get_eflash_count()
  \brief       get eflash instance.
  \return      pointer to eflash instance
*/
int32_t target_get_eflash(int32_t idx, uint32_t *base, uint32_t *irq, eflash_info_t *info)
{
    if (idx >= CONFIG_EFLASH_NUM) {
        return NULL;
    }

    *irq = sg_eflash_config[idx].irq;
    *base = sg_eflash_config[idx].base;
    info->start = sg_eflash_config[idx].info.start;
    info->end = sg_eflash_config[idx].info.end;
    info->sector_count = sg_eflash_config[idx].info.sector_count;
    return idx;
}

struct {
    uint32_t base;
    uint32_t irq;
}
const sg_spi_config[CONFIG_SPI_NUM] = {
    {ZX29_SPI0_BASE, PIC_IRQ_SSP0},
    {ZX29_SPI1_BASE, ICU_IRQ_SSP1},
    {ZX29_SPI2_BASE, ICU_IRQ_SSP2}
};
typedef struct {
    int32_t    mosi;
    int32_t    miso;
    int32_t    sclk;
    int32_t    ssel;
    uint32_t cfg_idx;    //idx of sg_iic_config[]
} spi_pin_map_t;
const static spi_pin_map_t s_spi_pin_map[] = {
    {
        GPIO11_SSP0_TXD,
        GPIO10_SSP0_RXD,
        GPIO9_SSP0_CLK,
        GPIO8_SSP0_CS,
        0
    },
    {
        GPIO15_SSP1_TXD,
        GPIO14_SSP1_RXD,
        GPIO13_SSP1_CLK,
        GPIO12_SSP1_CS,
        1
    },
};

/**
  \param[in]   instance idx, must not exceed return value of target_get_spi_count()
  \brief       get spi instance.
  \return      pointer to spi instance
*/
int32_t target_spi_init(int32_t idx, uint32_t *base, uint32_t *irq, uint32_t *ssel)
{
    if (idx >= sizeof(s_spi_pin_map) / sizeof(spi_pin_map_t)) {
        return -1;
    }
    *base = sg_spi_config[s_spi_pin_map[idx].cfg_idx].base;
    *irq = sg_spi_config[s_spi_pin_map[idx].cfg_idx].irq;
    *ssel = s_spi_pin_map[idx].ssel;
    /*pinmux*/
    pin_mux((s_spi_pin_map[idx].mosi >> 24) & 0xff, s_spi_pin_map[idx].mosi);
    pin_mux((s_spi_pin_map[idx].miso >> 24) & 0xff, s_spi_pin_map[idx].miso);
    pin_mux((s_spi_pin_map[idx].sclk >> 24) & 0xff, s_spi_pin_map[idx].sclk);
    pin_mux((s_spi_pin_map[idx].ssel >> 24) & 0xff, s_spi_pin_map[idx].ssel);

    return s_spi_pin_map[idx].cfg_idx;
}

struct {
    uint32_t base;
    uint32_t irq;
}
const sg_timer_config[CONFIG_TIMER_NUM] = {
//#ifdef CONFIG_SANECHIPS_TIMER0
    {CK_TIMER0_BASEADDR, PIC_IRQ_TIMER0},
//#endif
//#ifdef CONFIG_SANECHIPS_TIMER1
    {CK_TIMER1_BASEADDR, ICU_IRQ_TIMER1},
//#endif
    {ZX29_TIMER_2_BASE, PIC_IRQ_LP_TIMER},
    {AD_TIMER_0_BASE, PIC_IRQ_AD_TIMER0},
    {AD_TIMER_1_BASE, PIC_IRQ_AD_TIMER1}
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

struct {
    uint32_t base;
    uint32_t irq;
}
const sg_wdt_config[CONFIG_WDT_NUM] = {
    {AP_WDT_BASEADDR, PIC_IRQ_WDT}
};

int32_t target_get_wdt_count(void)
{
    return CONFIG_WDT_NUM;
}

int32_t target_get_wdt(int32_t idx, uint32_t *base, uint32_t *irq)
{
    if (idx >= target_get_wdt_count()) {
        return NULL;
    }

    *base = sg_wdt_config[idx].base;
    *irq = sg_wdt_config[idx].irq;
    return idx;
}

struct {
    uint32_t base;
}
const sg_trng_config[CONFIG_TRNG_NUM] = {
    {ZX29_RAND_BASE}
};

/**
  \brief       get trng instance count.
  \return      trng instance count
*/
int32_t target_get_trng_count(void)
{
    return CONFIG_TRNG_NUM;
}

/**
  \param[in]   instance idx, must not exceed return value of target_get_trng_count()
  \brief       get trng instance.
  \return      pointer to trng instance
*/
int32_t target_get_trng(int32_t idx, uint32_t *base)
{
    if (idx >= target_get_trng_count()) {
        return NULL;
    }

    *base = sg_trng_config[idx].base;
    return idx;
}

struct {
    uint32_t base;
}
const sg_sha_config[CONFIG_SHA_NUM] = {
    {ZX29_SHA_BASE}
};

/**
  \brief       get sha instance count.
  \return      sha instance count
*/
int32_t target_get_sha_count(void)
{
    return CONFIG_SHA_NUM;
}

/**
  \param[in]   instance idx, must not exceed return value of target_get_sha_count()
  \brief       get sha instance.
  \return      pointer to sha instance
*/
int32_t target_get_sha(int32_t idx, uint32_t *base, uint32_t *irq)
{
    if (idx >= target_get_sha_count()) {
        return NULL;
    }

    *base = sg_sha_config[idx].base;
    return idx;
}

struct {
    uint32_t base;
}
const sg_aes_config[CONFIG_AES_NUM] = {
    {ZX29_AES_BASE}
};

/**
  \brief       get aes instance count.
  \return      aes instance count
*/
int32_t target_get_aes_count(void)
{
    return CONFIG_AES_NUM;
}

/**
  \param[in]   instance idx, must not exceed return value of target_get_aes_count()
  \brief       get aes instance.
  \return      pointer to aes instance
*/
int32_t target_get_aes(int32_t idx, uint32_t *base, uint32_t *irq)
{
    if (idx >= target_get_aes_count()) {
        return NULL;
    }

    *base = sg_aes_config[idx].base;
    return idx;
}
struct {
    uint32_t base;
}
const sg_pmu_config[CONFIG_PMU_NUM] = {
    {ZX29_PMU_BASE}
};

/**
  \brief       get pmu instance count.
  \return      pmu instance count
*/
int32_t target_get_pmu_count(void)
{
    return CONFIG_PMU_NUM;
}

/**
  \param[in]   instance idx, must not exceed return value of target_get_pmu_count()
  \brief       get pmu instance.
  \return      pointer to pmu instance
*/
int32_t target_get_pmu(int32_t idx, uint32_t *base, uint32_t *irq)
{
    if (idx >= target_get_pmu_count()) {
        return NULL;
    }

    *base = sg_pmu_config[idx].base;
    return idx;
}

struct {
    uint32_t base;
}
const sg_bmu_config[CONFIG_BMU_NUM] = {
    {ZX29_BMU_BASE}
};

/**
  \brief       get bmu instance count.
  \return      bmu instance count
*/
int32_t target_get_bmu_count(void)
{
    return CONFIG_BMU_NUM;
}

/**
  \param[in]   instance idx, must not exceed return value of target_get_bmu_count()
  \brief       get bmu instance.
  \return      pointer to bmu instance
*/
int32_t target_get_bmu(int32_t idx, uint32_t *base, uint32_t *irq)
{
    if (idx >= target_get_bmu_count()) {
        return NULL;
    }

    *base = sg_bmu_config[idx].base;
    return idx;
}



struct {
    uint32_t base;
}
const sg_rsa_config[CONFIG_RSA_NUM] = {
    {ZX29_RSA_BASE}
};

int32_t target_get_rsa_count(void)
{
    return CONFIG_RSA_NUM;
}

int32_t target_get_rsa(int32_t idx, uint32_t *base, uint32_t *irq)
{
    if (idx >= target_get_rsa_count()) {
        return NULL;
    }

    *base = sg_rsa_config[idx].base;
    return idx;
}

struct {
    uint32_t base;
    uint32_t irq;
}
const sg_rtc_config[CONFIG_RTC_NUM] = {
    {ZX29_RTC_BASE, PIC_IRQ_RTC_TIMER}
};

int32_t target_get_rtc_count(void)
{
    return CONFIG_RTC_NUM;
}

int32_t target_get_rtc(int32_t idx, uint32_t *base, uint32_t *irq)
{
    if (idx >= target_get_rtc_count()) {
        return NULL;
    }

    *base = sg_rtc_config[idx].base;
    *irq = sg_rtc_config[idx].irq;
    return idx;
}

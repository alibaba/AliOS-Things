/*
 * Copyright (c) 2019 MediaTek Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef __MT_GPIO_H
#define __MT_GPIO_H

#include <stdint.h>
#include <mt_reg_base.h>

/******************************************************************************
* Enumeration for GPIO pin
******************************************************************************/

enum GPIO_PIN {
	GPIO_UNSUPPORTED = -1,
	GPIO0  , GPIO1  , GPIO2  , GPIO3  , GPIO4  , GPIO5  , GPIO6  , GPIO7  ,
	GPIO8  , GPIO9  , GPIO10 , GPIO11 , GPIO12 , GPIO13 , GPIO14 , GPIO15 ,
	GPIO16 , GPIO17 , GPIO18 , GPIO19 , GPIO20 , GPIO21 , GPIO22 , GPIO23 ,
	GPIO24 , GPIO25 , GPIO26 , GPIO27 , GPIO28 , GPIO29 , GPIO30 , GPIO31 ,
	GPIO32 , GPIO33 , GPIO34 , GPIO35 , GPIO36 , GPIO37 , GPIO38 , GPIO39 ,
	GPIO40 , GPIO41 , GPIO42 , GPIO43 , GPIO44 , GPIO45 , GPIO46 , GPIO47 ,
	GPIO48 , GPIO49 , GPIO50 , GPIO51 , GPIO52 , GPIO53 , GPIO54 , GPIO55 ,
	GPIO56 , GPIO57 , GPIO58 , GPIO59 , GPIO60 , GPIO61 , GPIO62 , GPIO63 ,
	GPIO64 , GPIO65 , GPIO66 , GPIO67 , GPIO68 , GPIO69 , GPIO70 , GPIO71 ,
	GPIO72 , GPIO73 , GPIO74 , GPIO75 , GPIO76 , GPIO77 , GPIO78 , GPIO79 ,
	GPIO80 , GPIO81 , GPIO82 , GPIO83 , GPIO84 , GPIO85 , GPIO86 , GPIO87 ,
	GPIO88 , GPIO89 , GPIO90 , GPIO91 , GPIO92 , GPIO93 , GPIO94 , GPIO95 ,
	GPIO96 , GPIO97 , GPIO98 , GPIO99 , GPIO100, GPIO101, GPIO102, GPIO103,
	GPIO104, GPIO105, GPIO106, GPIO107, GPIO108, GPIO109, GPIO110, GPIO111,
	GPIO112, GPIO113, GPIO114, GPIO115, MT_GPIO_BASE_MAX
};

#define MAX_GPIO_PIN    (uint16_t)116

/******************************************************************************
* Enumeration for GPIO pin
******************************************************************************/
/* GPIO MODE CONTROL VALUE*/
enum GPIO_MODE {
	GPIO_MODE_UNSUPPORTED = -1,
	GPIO_MODE_GPIO  = 0,
	GPIO_MODE_00    = 0,
	GPIO_MODE_01    = 1,
	GPIO_MODE_02    = 2,
	GPIO_MODE_03    = 3,
	GPIO_MODE_04    = 4,
	GPIO_MODE_05    = 5,
	GPIO_MODE_06    = 6,
	GPIO_MODE_07    = 7,
	GPIO_MODE_MAX,
	GPIO_MODE_DEFAULT = GPIO_MODE_01,
};
/* GPIO DIRECTION */
enum GPIO_DIR {
	GPIO_DIR_UNSUPPORTED = -1,
	GPIO_DIR_IN     = 0,
	GPIO_DIR_OUT    = 1,
	GPIO_DIR_MAX,
	GPIO_DIR_DEFAULT = GPIO_DIR_IN,
};
/* GPIO PULL ENABLE*/
enum GPIO_PULL_EN {
	GPIO_PULL_EN_UNSUPPORTED = -1,
	GPIO_PULL_DISABLE = 0,
	GPIO_PULL_ENABLE  = 1,
	GPIO_PULL_EN_MAX,
	GPIO_PULL_EN_DEFAULT = GPIO_PULL_ENABLE,
};
/* GPIO IES ENABLE*/
enum GPIO_IES_EN {
	GPIO_IES_EN_UNSUPPORTED = -1,
	GPIO_IES_DISABLE = 0,
	GPIO_IES_ENABLE  = 1,
	GPIO_IES_EN_MAX,
	GPIO_IES_EN_DEFAULT = GPIO_IES_ENABLE,
};
/* GPIO SMT ENABLE*/
enum GPIO_SMT_EN {
	GPIO_SMT_EN_UNSUPPORTED = -1,
	GPIO_SMT_DISABLE = 0,
	GPIO_SMT_ENABLE  = 1,
	GPIO_SMT_EN_MAX,
	GPIO_SMT_EN_DEFAULT = GPIO_SMT_DISABLE,
};


/* GPIO PULL-UP/PULL-DOWN*/
enum GPIO_PULL {
	GPIO_PULL_UNSUPPORTED = -1,
	GPIO_PULL_DOWN  = 0,
	GPIO_PULL_UP    = 1,
	GPIO_PULL_MAX,
	GPIO_PULL_DEFAULT = GPIO_PULL_DOWN
};
/* GPIO OUTPUT */
enum GPIO_OUT {
	GPIO_OUT_UNSUPPORTED = -1,
	GPIO_OUT_ZERO = 0,
	GPIO_OUT_ONE  = 1,
	GPIO_OUT_MAX,
	GPIO_OUT_DEFAULT = GPIO_OUT_ZERO,
	GPIO_DATA_OUT_DEFAULT = GPIO_OUT_ZERO,  /*compatible with DCT*/
};
/* GPIO DRIVING */
enum GPIO_DRIVING {
	MTK_DRIVE_2mA = 2,
	MTK_DRIVE_4mA = 4,
	MTK_DRIVE_6mA = 6,
	MTK_DRIVE_8mA = 8,
	MTK_DRIVE_10mA = 10,
	MTK_DRIVE_12mA = 12,
	MTK_DRIVE_14mA = 14,
	MTK_DRIVE_16mA = 16,
	MTK_DRIVE_20mA = 20,
	MTK_DRIVE_24mA = 24,
	MTK_DRIVE_28mA = 28,
	MTK_DRIVE_32mA = 32,
	GPIO_DRIVE_MAX = MTK_DRIVE_32mA,
};

/******************************************************************************
* GPIO Driver interface
******************************************************************************/
/*direction*/
int32_t gpio_set_dir(uint16_t pin, enum GPIO_DIR dir);
uint32_t gpio_get_dir(uint16_t pin);
/*pull enable*/
int32_t gpio_set_pull_enable(uint16_t pin,
				enum GPIO_PULL_EN enable);
uint32_t gpio_get_pull_enable(uint16_t pin);
/*pull select*/
int32_t gpio_set_pull_select(uint16_t pin, enum GPIO_PULL select);
uint32_t gpio_get_pull_select(uint16_t pin);
/*input/output*/
int32_t gpio_set_out(uint16_t pin, enum GPIO_OUT output);
uint32_t gpio_get_out(uint16_t pin);
uint32_t gpio_get_in(uint16_t pin);
/*mode control*/
int32_t gpio_set_mode(uint16_t pin, enum GPIO_MODE mode);
uint32_t gpio_get_mode(uint16_t pin);
/*driving*/
int32_t gpio_set_driving(uint16_t pin,
			 enum GPIO_DRIVING driving);
int32_t gpio_get_driving(uint16_t pin);

int32_t gpio_set_ies_enable(uint16_t pin,
			    enum GPIO_IES_EN enable);
uint32_t gpio_get_ies_enable(uint16_t pin);
int32_t gpio_set_smt_enable(uint16_t pin,
			    enum GPIO_SMT_EN enable);
uint32_t gpio_get_smt_enable(uint16_t pin);
/*gpio to irq*/
uint32_t gpio_get_irq(uint16_t gpio);
void gpio_cust_init(void);
void gpio_cli_init(void);

#endif

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
 * @file     pinmux.c
 * @brief    source file for the pinmux
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#include <stdint.h>
#include "pinmux.h"
#include "pin_name.h"
#include "io.h"
//#include "zx29_gpio.h"


#define readl(addr) \
    ({ unsigned int __v = (*(volatile unsigned int *) (addr)); __v; })

#define writel(b,addr) (void)((*(volatile unsigned int *) (addr)) = (b))

/*******************************************************************************
 * function: zx297100_ioreuse_inital
 *
 * description:
 *   initial zx297100_pinmux
 *******************************************************************************/

void zx297100_ioreuse_initial(void)
{
    /* reuse function select */

}

int32_t pin_mux(uint32_t pin, enum gpio_func func)
{
	uint32_t func_sel = (func >> FUN_SEL_SHIFT) & 0x1;
	uint32_t value = func & FUNC_SEL_MASK;
	uint32_t pinmux_io_reg;

	if ((pin != (func >> GPIO_ID_SHIFT))||(pin > SI_GPIO_MAX))
		return -1;

	if (BIT_WIDTH(pin_info[pin].io_cfg_field)) {
		if (pin < GPIO_AO_NR)
			pinmux_io_reg = PINMUX_IO_CFG_REG(pin_info[pin].io_cfg_field);
		else
			pinmux_io_reg = PINMUX_IO_CFG_PD_REG(pin_info[pin].io_cfg_field);

		__write_bits(1, BIT_OFFSET(pin_info[pin].io_cfg_field), (volatile uint32_t *)pinmux_io_reg, 0x1);
	}

	/* top_func_sel */
	if (BIT_WIDTH(pin_info[pin].top_sel_field))
	    __write_bits(BIT_WIDTH(pin_info[pin].top_sel_field),
						BIT_OFFSET(pin_info[pin].top_sel_field),
						(volatile uint32_t *)PINMUX_TOP_REG(pin_info[pin].top_sel_field),
						func_sel);

	if (PIN_FUNC_SEL_AON == func_sel) {
		if(BIT_WIDTH(pin_info[pin].aon_sel_field))
			__write_bits(BIT_WIDTH(pin_info[pin].aon_sel_field),
							BIT_OFFSET(pin_info[pin].aon_sel_field),
							(volatile uint32_t *)PINMUX_AO_REG(pin_info[pin].aon_sel_field),
							value);
	} else {
		if (BIT_WIDTH(pin_info[pin].pd_sel_field))
			__write_bits(BIT_WIDTH(pin_info[pin].pd_sel_field),
							BIT_OFFSET(pin_info[pin].pd_sel_field),
							(volatile uint32_t *)PINMUX_PD_REG(pin_info[pin].pd_sel_field),
							value);
	}

	return 0;
}

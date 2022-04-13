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
#include "freertos_to_aos.h"
#include <mt_gic_v3.h>
#include <mt_gpio.h>
#include <mt_gpio_init.h>
#include <mt_printf.h>
#include <mt_reg_base.h>
#include <stdint.h>

#include <stdbool.h>

#include <stdio.h>
#include <mt_reg_base.h>

#include "aos/cli.h"

#define dsb() __asm__ __volatile__ ("dsb" : : : "memory")

#define mb()    \
    do {    \
        dsb();    \
    } while (0)

#define DRV_Reg32(addr)             (*(volatile unsigned int *)(addr))
#define DRV_WriteReg32(addr, data)    \
    do {    \
        ((*(volatile unsigned int *)(addr)) = (unsigned int)data);    \
        mb();    \
    } while(0)

#define ARRAY_SIZE(a)       (sizeof(a) / sizeof(a[0]))

#define MAX_GPIO_REG_BITS      (uint16_t)32
#define MAX_GPIO_MODE_PER_REG  (uint16_t)10
#define GPIO_MODE_BITS         (uint16_t)3
#define BITS(x) (1UL << (uint32_t)(x))

static struct gpio_regs *mt8512_gpio = (struct gpio_regs *)GPIO_BASE;

/*********************************
R0:bit0/3/6/9/12/15/18/21/24/27
R1:bit1/4/7/10/13/16/19/22/25/28
PUPD:bit2/5/8/11/14/17/20/23/26/29;
0=pull-up 1=pull-down
***********************************/
struct mtk_spec_pupd_grp {
	uint16_t pin;
	uint32_t reg;
	uint16_t bit;
};

static const struct mtk_spec_pupd_grp pupd_infos[]=
{
	{0, 0x0F0, 12},
	{1, 0x0F0, 15},
	{2, 0x0F0, 18},
	{3, 0x0F0, 21},
	{4, 0x0F0, 24},
	{5, 0x0F0, 27},
	{6, 0x300, 0},
	{7, 0x300, 3},
	{8, 0x300, 6},
	{9, 0x300, 9},
	{10, 0x300, 12},
	{11, 0x300, 15},
	{32, 0x300, 18},
	{40, 0x070, 0},
	{41, 0x070, 3},
	{42, 0x070, 6},
	{43, 0x070, 9},
	{44, 0x300, 21},
	{45, 0x300, 24},
	{46, 0x300, 27},
	{47, 0x310, 0},
	{70, 0x080, 3},
	{71, 0x080, 6},
	{72, 0x080, 9},
	{73, 0x080, 12},
	{74, 0x080, 15},
	{75, 0x080, 18},
	{76, 0x080, 21},
	{77, 0x080, 24},
	{78, 0x080, 27},
	{79, 0x090, 0},
	{80, 0x090, 3},
	{81, 0x090, 6},
	{82, 0x090, 9},
	{83, 0x090, 12},
	{84, 0x090, 15},
	{85, 0x090, 18},
	{86, 0x090, 21},
	{87, 0x310, 3},
	{92, 0x310, 18},
	{93, 0x310, 21},
	{94, 0x310, 24},
	{95, 0x310, 27},
	{96, 0x320, 0},
	{97, 0x320, 3},
	{98, 0x320, 6},
	{99, 0x320, 9},
	{100, 0x320, 12},
	{101, 0x320, 15},
	{102, 0x320, 18},
	{103, 0x320, 21},
	{104, 0x320, 24},
	{105, 0x320, 27},
	{106, 0x330, 0},
	{107, 0x330, 3},
	{108, 0x330, 6},
	{109, 0x330, 9},
	{110, 0x330, 12},
	{111, 0x330, 15}
};

#define PUPD_OFFSET_TO_REG(pupd_infos_ctrl) ((struct val_regs*)(GPIO_BASE + pupd_infos_ctrl->reg))

const struct mtk_spec_pupd_grp * mt_get_pupd_ctrl(uint16_t pin)
{
	int i;
	bool find = false;

	for (i = 0; i < ARRAY_SIZE(pupd_infos); i++) {
		if (pin == pupd_infos[i].pin) {  
			find = true;
			break;
		}
	}

	if (!find)
		return NULL;

    return &pupd_infos[i];
}

static int32_t gpio_cust_set_dir_chip(uint16_t pin, enum GPIO_DIR dir)
{
	uint32_t pos;
	uint16_t bit = 0;
	int32_t ret = 0;

	if (pin >= MAX_GPIO_PIN) {
		ret = -1;
	} else if (dir >= GPIO_DIR_MAX) {
		ret = -1;
	} else {
		pos = (uint32_t)(pin / MAX_GPIO_REG_BITS);
		bit = pin % MAX_GPIO_REG_BITS;

		if (dir == GPIO_DIR_IN) {
			DRV_WriteReg32(&mt8512_gpio->dir[pos].rst,
				1 << bit);
		} else {
			DRV_WriteReg32(&mt8512_gpio->dir[pos].set,
				1 << bit);
		}
		ret = 0;
	}
	return ret;
}

static uint32_t gpio_cust_get_dir_chip(uint16_t pin)
{
	uint32_t pos =0;
	uint16_t bit;
	uint32_t reg;
	uint32_t ret;

	if (pin >= MAX_GPIO_PIN) {
		ret = ~(0UL);
	} else {
		pos = (uint32_t)(pin / MAX_GPIO_REG_BITS);
		bit = pin % MAX_GPIO_REG_BITS;
		reg = DRV_Reg32(&mt8512_gpio->dir[pos].val);
		ret = ((reg & BITS(bit)) != 0UL) ? 1UL : 0UL;
	}
	return ret;
}

static int32_t gpio_cust_set_pull_enable_chip(uint16_t pin,
						enum GPIO_PULL_EN enable)
{
	uint32_t pos=0;
	uint16_t bit=0;
	int32_t ret = 0;
	const struct mtk_spec_pupd_grp * pupd_infos_ctrl;

	if (pin >= MAX_GPIO_PIN) {
		ret = -1;
	} else if (enable >= GPIO_PULL_EN_MAX) {
		ret = -1;
	} else {
		/*for special msdc pupd*/
    	if(NULL != (pupd_infos_ctrl = mt_get_pupd_ctrl(pin))) {
        	if (enable == GPIO_PULL_DISABLE) {
            	DRV_WriteReg32(&(PUPD_OFFSET_TO_REG(pupd_infos_ctrl))->rst, (1L << (pupd_infos_ctrl->bit)));
            	DRV_WriteReg32(&(PUPD_OFFSET_TO_REG(pupd_infos_ctrl))->rst, (1L << (pupd_infos_ctrl->bit + 1)));
        	}
        	else {
            	DRV_WriteReg32(&(PUPD_OFFSET_TO_REG(pupd_infos_ctrl))->set, (1L << (pupd_infos_ctrl->bit)));
            	DRV_WriteReg32(&(PUPD_OFFSET_TO_REG(pupd_infos_ctrl))->set, (1L << (pupd_infos_ctrl->bit + 1)));
        	}
        	ret = 0;
    	} else {
			pos = (uint32_t)(pin / MAX_GPIO_REG_BITS);
			bit = pin % MAX_GPIO_REG_BITS;

			if (enable == GPIO_PULL_DISABLE) {
				DRV_WriteReg32(&mt8512_gpio->pullen[pos].rst,
					1 << bit);
			} else {
				DRV_WriteReg32(&mt8512_gpio->pullen[pos].set,
					1 << bit);
			}
		ret = 0;
    	}
	}
	return ret;
}

static uint32_t gpio_cust_get_pull_enable_chip(uint16_t pin)
{
	uint32_t pos;
	uint16_t bit;
	uint32_t reg = 0;
	uint32_t ret = 0;
	uint32_t data = 0;
	const struct mtk_spec_pupd_grp * pupd_infos_ctrl;

	if (pin >= MAX_GPIO_PIN) {
		ret = ~(0UL);
	} else {
		/*for special msdc pupd*/
		if(NULL != (pupd_infos_ctrl = mt_get_pupd_ctrl(pin))) {
			data = DRV_Reg32(&(PUPD_OFFSET_TO_REG(pupd_infos_ctrl))->val);
			ret = (((data & (3L << (pupd_infos_ctrl->bit))) == 0)? 0: 1);
		} else {
			pos = (uint32_t)(pin / MAX_GPIO_REG_BITS);
			bit = pin % MAX_GPIO_REG_BITS;

			reg = DRV_Reg32(&mt8512_gpio->pullen[pos].val);
			ret = ((reg & BITS(bit)) != 0UL) ? 1UL : 0UL;
		}
	}
	return ret;
}

static int32_t gpio_cust_set_pull_select_chip(uint16_t pin,
						enum GPIO_PULL select)
{
	uint32_t pos;
	uint16_t bit;
	int32_t ret = 0;
	const struct mtk_spec_pupd_grp * pupd_infos_ctrl;

	if (pin >= MAX_GPIO_PIN) {
		ret = -1;
	} else if (select >= GPIO_PULL_MAX) {
		ret = -1;
	} else {
		/*for special msdc pupd*/
		if(NULL != (pupd_infos_ctrl = mt_get_pupd_ctrl(pin))) {
			if (select == GPIO_PULL_DOWN) {
				DRV_WriteReg32(&(PUPD_OFFSET_TO_REG(pupd_infos_ctrl))->set, (1L << (pupd_infos_ctrl->bit + 2)));
			}
			else {
				DRV_WriteReg32(&(PUPD_OFFSET_TO_REG(pupd_infos_ctrl))->rst, (1L << (pupd_infos_ctrl->bit + 2)));
			}
        	ret = 0;
		} else {
			pos = (uint32_t)(pin / MAX_GPIO_REG_BITS);
			bit = pin % MAX_GPIO_REG_BITS;

			if (select == GPIO_PULL_DOWN) {
				DRV_WriteReg32(&mt8512_gpio->pullsel[pos].rst,
					1 << bit);
			} else {
				DRV_WriteReg32(&mt8512_gpio->pullsel[pos].set,
					1 << bit);
			}
			ret = 0;
		}
	}
	return ret;
}

static uint32_t gpio_cust_get_pull_select_chip(uint16_t pin)
{
	uint32_t pos;
	uint16_t bit;
	uint32_t reg;
	uint32_t ret = 0;
	uint32_t data = 0;
	const struct mtk_spec_pupd_grp * pupd_infos_ctrl;

	if (pin >= MAX_GPIO_PIN) {
		ret = ~(0UL);
	} else {
		/*for special msdc pupd*/
		if(NULL != (pupd_infos_ctrl = mt_get_pupd_ctrl(pin))) {
			data = DRV_Reg32(&(PUPD_OFFSET_TO_REG(pupd_infos_ctrl))->val);
			ret = (((data & (1L << pupd_infos_ctrl->bit + 2)) != 0)? 0: 1);
		} else {
			pos = (uint32_t)(pin / MAX_GPIO_REG_BITS);
			bit = pin % MAX_GPIO_REG_BITS;

			reg = DRV_Reg32(&mt8512_gpio->pullsel[pos].val);
			ret = (((reg & BITS(bit)) != 0UL) ? 1UL : 0UL);
		}
	}
	return ret;
}

static int32_t gpio_cust_set_out_chip(uint16_t pin, enum GPIO_OUT output)
{
	uint32_t pos =0;
	uint16_t bit = 0;
	int32_t ret = 0;

	if (pin >= MAX_GPIO_PIN) {
		ret = -1;
	} else if (output >= GPIO_OUT_MAX) {
		ret = -1;
	} else {
		pos = (uint32_t)(pin / MAX_GPIO_REG_BITS);
		bit = pin % MAX_GPIO_REG_BITS;

		if (output == GPIO_OUT_ZERO) {
			DRV_WriteReg32(&mt8512_gpio->dout[pos].rst,
				1 << bit);
		} else {
			DRV_WriteReg32(&mt8512_gpio->dout[pos].set,
				1 << bit);
		}
		ret = 0;
	}
	return ret;
}

static uint32_t gpio_cust_get_out_chip(uint16_t pin)
{
	uint32_t pos;
	uint16_t bit;
	uint32_t reg;
	uint32_t ret;

	if (pin >= MAX_GPIO_PIN) {
		ret = ~(0UL);
	} else {
		pos = (uint32_t)(pin / MAX_GPIO_REG_BITS);
		bit = pin % MAX_GPIO_REG_BITS;

		reg = DRV_Reg32(&mt8512_gpio->dout[pos].val);
		ret = ((reg & BITS(bit)) != 0UL) ? 1UL : 0UL;
	}
	return ret;
}

static uint32_t gpio_cust_get_in_chip(uint16_t pin)
{
	uint32_t pos;
	uint16_t bit;
	uint32_t reg;
	uint32_t ret;

	if (pin >= MAX_GPIO_PIN) {
		ret = ~(0UL);
	} else {
		pos = (uint32_t)(pin / MAX_GPIO_REG_BITS);
		bit = pin % MAX_GPIO_REG_BITS;
		reg = DRV_Reg32(&mt8512_gpio->din[pos].val);
		ret = ((reg & BITS(bit)) != 0UL) ? 1UL : 0UL;
	}
	return ret;
}

static int32_t gpio_cust_set_mode_chip(uint16_t pin, enum GPIO_MODE mode)
{
	uint32_t pos, reg;
	uint16_t bit;
	uint16_t mask = (1 << GPIO_MODE_BITS) - 1;
	int32_t ret = 0;

	if (pin >= MAX_GPIO_PIN) {
		ret = -1;
	} else if (mode >= GPIO_MODE_MAX) {
		ret = -1;
	} else {
		pos = (uint32_t)(pin / MAX_GPIO_MODE_PER_REG);
		bit = pin % MAX_GPIO_MODE_PER_REG;

		reg = DRV_Reg32(&mt8512_gpio->mode[pos].val);
		reg &= ~(mask << (GPIO_MODE_BITS*bit));
		reg |= (mode << (GPIO_MODE_BITS*bit));

		DRV_WriteReg32(&mt8512_gpio->mode[pos].val, reg);
		ret = 0;
	}
	return ret;
}

static uint32_t gpio_cust_get_mode_chip(uint16_t pin)
{
	uint32_t pos;
	uint16_t bit;
	uint32_t reg;
	uint32_t mask = (1UL << GPIO_MODE_BITS) - 1U;
	uint32_t ret = 0;

	if (pin >= MAX_GPIO_PIN) {
		ret = ~(0UL);
	} else {
		pos = (uint32_t)(pin / MAX_GPIO_MODE_PER_REG);
		bit = pin % MAX_GPIO_MODE_PER_REG;
		reg = DRV_Reg32(&mt8512_gpio->mode[pos].val);
		ret = (reg >> (uint32_t)(GPIO_MODE_BITS*bit)) & mask;
	}
	return ret;
}

/*set driving*/
struct mtk_drv_group_desc {
	uint8_t min_drv;
	uint8_t max_drv;
	uint8_t low_bit;
	uint8_t high_bit;
	uint8_t step;
};

struct mtk_pin_drv_grp {
	uint16_t pin;
	uint16_t index;
	uint8_t bit;
	uint8_t grp;
};

static struct mtk_drv_group_desc mt8512_drv_grp[] =  {
	/* 0E4E8SR 4/8/12/16 */
	{4, 16, 1, 2, 4},
	/* 0E2E4SR  2/4/6/8 */
	{2, 8, 1, 2, 2},
	/* E8E4E2  2/4/6/8/10/12/14/16 */
	{2, 16, 0, 2, 2}
};

static struct mtk_pin_drv_grp mt8512_pin_drv[] = {
	{0, 0, 0, 2},
	{1, 0, 0, 2},
	{2, 0, 0, 2},
	{3, 0, 4, 2},
	{4, 0, 4, 2},
	{5, 0, 4, 2},
	{6, 0, 8, 2},
	{7, 0, 8, 2},
	{8, 0, 12, 2},
	{9, 0, 12, 2},
	{10, 0, 12, 2},
	{11, 0, 12, 2},
	{12, 0, 16, 2},
	{13, 0, 16, 2},
	{14, 0, 16, 2},
	{15, 0, 16, 2},
	{16, 0, 20, 2},
	{17, 0, 20, 2},
	{18, 0, 20, 2},
	{19, 0, 20, 2},
	{20, 0, 24, 2},
	{21, 0, 28, 2},
	{22, 0, 28, 2},
	{23, 0, 28, 2},
	{24, 0, 28, 2},
	{25, 0, 28, 2},
	{26, 1, 0, 2},
	{27, 1, 0, 2},
	{28, 1, 4, 2},
	{29, 1, 4, 2},
	{30, 1, 4, 2},
	{31, 1, 4, 2},
	{32, 1, 8, 2},
	{33, 1, 12, 2},
	{34, 1, 12, 2},
	{35, 1, 12, 2},
	{36, 1, 12, 2},
	{37, 1, 12, 2},
	{38, 1, 12, 2},
	{39, 1, 12, 2},
	{40, 1, 16, 2},
	{41, 1, 20, 2},
	{42, 1, 20, 2},
	{43, 1, 20, 2},
	{44, 1, 24, 2},
	{45, 1, 24, 2},
	{46, 1, 24, 2},
	{47, 1, 24, 2},
	{48, 1, 28, 2},
	{49, 1, 28, 2},
	{50, 1, 28, 2},
	{51, 1, 28, 2},
	{52, 2, 0, 2},
	{53, 2, 0, 2},
	{54, 2, 4, 2},
	{55, 2, 4, 2},
	{56, 2, 4, 2},
	{57, 2, 4, 2},
	{58, 2, 8, 2},
	{59, 2, 8, 2},
	{60, 2, 8, 2},
	{61, 2, 8, 2},
	{62, 2, 8, 2},
	{63, 2, 8, 2},
	{64, 2, 12, 2},
	{65, 2, 12, 2},
	{66, 2, 16, 2},
	{67, 2, 16, 2},
	{68, 2, 20, 2},
	{69, 2, 20, 2},
	{70, 2, 24, 2},
	{71, 2, 28, 2},
	{72, 3, 0, 2},
	{73, 3, 0, 2},
	{74, 3, 0, 2},
	{75, 3, 0, 2},
	{76, 3, 16, 2},
	{77, 3, 16, 2},
	{78, 3, 16, 2},
	{79, 3, 16, 2},
	{80, 4, 0, 2},
	{81, 4, 4, 2},
	{82, 4, 8, 2},
	{83, 3, 16, 2},
	{84, 3, 16, 2},
	{85, 3, 16, 2},
	{86, 3, 16, 2},
	{87, 4, 24, 2},
	{88, 4, 28, 2},
	{89, 4, 28, 2},
	{90, 4, 28, 2},
	{91, 4, 28, 2},
	{92, 5, 0, 2},
	{93, 5, 0, 2},
	{94, 5, 0, 2},
	{95, 5, 0, 2},
	{96, 5, 0, 2},
	{97, 5, 0, 2},
	{98, 5, 0, 2},
	{99, 5, 4, 2},
	{100, 5, 4, 2},
	{101, 5, 4, 2},
	{102, 5, 8, 2},
	{103, 5, 8, 2},
	{104, 5, 8, 2},
	{105, 5, 12, 2},
	{106, 5, 12, 2},
	{107, 5, 12, 2},
	{108, 5, 12, 2},
	{109, 5, 12, 2},
	{110, 5, 12, 2},
	{111, 5, 12, 2},
	{112, 5, 16, 2},
	{113, 5, 16, 2},
	{114, 5, 16, 2},
	{115, 5, 16, 2},
};

struct mtk_pin_ies_smt_grp {
	uint16_t pin;
	uint16_t index;
	uint16_t bit;
};

static struct mtk_pin_ies_smt_grp mt8512_pin_ies_smt[] = {
	{0, 0, 0},
	{1, 0, 0},
	{2, 0, 0},
	{3, 0, 1},
	{4, 0, 1},
	{5, 0, 1},
	{6, 0, 2},
	{7, 0, 2},
	{8, 0, 3},
	{9, 0, 3},
	{10, 0, 3},
	{11, 0, 3},
	{12, 0, 4},
	{13, 0, 4},
	{14, 0, 4},
	{15, 0, 4},
	{16, 0, 5},
	{17, 0, 5},
	{18, 0, 5},
	{19, 0, 5},
	{20, 0, 6},
	{21, 0, 7},
	{22, 0, 7},
	{23, 0, 7},
	{24, 0, 7},
	{25, 0, 7},
	{26, 0, 8},
	{27, 0, 8},
	{28, 0, 9},
	{29, 0, 9},
	{30, 0, 9},
	{31, 0, 9},
	{32, 0, 10},
	{33, 0, 11},
	{34, 0, 11},
	{35, 0, 11},
	{36, 0, 11},
	{37, 0, 11},
	{38, 0, 11},
	{39, 0, 11},
	{40, 0, 12},
	{41, 0, 13},
	{42, 0, 13},
	{43, 0, 13},
	{44, 0, 14},
	{45, 0, 14},
	{46, 0, 14},
	{47, 0, 14},
	{48, 0, 15},
	{49, 0, 15},
	{50, 0, 15},
	{51, 0, 15},
	{52, 0, 16},
	{53, 0, 16},
	{54, 0, 17},
	{55, 0, 17},
	{56, 0, 17},
	{57, 0, 17},
	{58, 0, 18},
	{59, 0, 18},
	{60, 0, 18},
	{61, 0, 18},
	{62, 0, 18},
	{63, 0, 18},
	{64, 0, 19},
	{65, 0, 19},
	{66, 0, 20},
	{67, 0, 20},
	{68, 0, 21},
	{69, 0, 21},
	{70, 0, 22},
	{71, 0, 23},
	{72, 0, 24},
	{73, 0, 25},
	{74, 0, 26},
	{75, 0, 27},
	{76, 0, 28},
	{77, 0, 29},
	{78, 0, 30},
	{79, 0, 31},
	{80, 1, 0},
	{81, 1, 1},
	{82, 1, 2},
	{83, 1, 3},
	{84, 1, 4},
	{85, 1, 5},
	{86, 1, 5},
	{87, 1, 6},
	{88, 1, 7},
	{89, 1, 7},
	{90, 1, 7},
	{91, 1, 7},
	{92, 1, 8},
	{93, 1, 8},
	{94, 1, 8},
	{95, 1, 8},
	{96, 1, 8},
	{97, 1, 8},
	{98, 1, 8},
	{99, 1, 9},
	{100, 1, 9},
	{101, 1, 9},
	{102, 1, 10},
	{103, 1, 10},
	{104, 1, 10},
	{105, 1, 11},
	{106, 1, 11},
	{107, 1, 11},
	{108, 1, 11},
	{109, 1, 11},
	{110, 1, 11},
	{111, 1, 11},
	{112, 1, 12},
	{113, 1, 12},
	{114, 1, 12},
	{115, 1, 12}
};

static const struct mtk_pin_drv_grp *mt_find_pin_drv_grp_by_pin(uint16_t pin)
{
	const struct mtk_pin_drv_grp *ret;

	if (pin >= MAX_GPIO_PIN) {
		ret = NULL;
	} else {
		ret = &mt8512_pin_drv[pin];
	}
	return ret;
}

int32_t gpio_set_driving(uint16_t pin, enum GPIO_DRIVING driving)
{
	uint16_t val, bits, mask, shift;
	const struct mtk_drv_group_desc *drv_grp;
	const struct mtk_pin_drv_grp *pin_drv;
	int32_t ret = 0;
	uint8_t drivelue = (uint8_t)driving;

	pin_drv = mt_find_pin_drv_grp_by_pin(pin);
	if (NULL == pin_drv) {
		ret = -1;
	} else {
		drv_grp = &mt8512_drv_grp[pin_drv->grp];
		if ((drivelue >= drv_grp->min_drv) &&
			(drivelue <= drv_grp->max_drv) &&
			((drivelue % (drv_grp->step)) == (uint8_t)0)) {
			val = drivelue / drv_grp->step - 1;
			bits = drv_grp->high_bit - drv_grp->low_bit
				+ 1;
			mask = (1 << (bits)) - 1;
			shift = pin_drv->bit + drv_grp->low_bit;

			DRV_WriteReg32(&mt8512_gpio->drv_mode[pin_drv->index].rst,
				mask << shift);
			DRV_WriteReg32(&mt8512_gpio->drv_mode[pin_drv->index].set,
				val << shift);
			ret = 0;
		} else {
			ret = -1;
		}
	}
	return ret;
}

int32_t gpio_get_driving(uint16_t pin)
{
	uint16_t bits, mask, shift;
	const struct mtk_drv_group_desc *drv_grp;
	const struct mtk_pin_drv_grp *pin_drv;
	int32_t ret = 0;

	pin_drv = mt_find_pin_drv_grp_by_pin(pin);
	if (NULL == pin_drv) {
		ret = -1;
	} else {
		drv_grp = &mt8512_drv_grp[pin_drv->grp];
		bits = drv_grp->high_bit - drv_grp->low_bit
			+ 1;
		mask = (1 << (bits)) - 1;
		shift = pin_drv->bit + drv_grp->low_bit;

		ret = (DRV_Reg32(&mt8512_gpio->drv_mode[pin_drv->index].val)
				& (mask << shift)) >> shift;
		ret = (ret + 1) * drv_grp->min_drv;
	}
	return ret;
}

static int32_t gpio_cust_set_ies_enable_chip(uint16_t pin,
						enum GPIO_IES_EN enable)
{
	int32_t ret = 0;
	const struct mtk_pin_ies_smt_grp *ies_ctrl;

	if (pin >= MAX_GPIO_PIN) {
		ret = -1;
	} else if (enable >= GPIO_IES_EN_MAX) {
		ret = -1;
	} else {
		ies_ctrl = &mt8512_pin_ies_smt[pin];

		if (enable == GPIO_IES_DISABLE) {
			DRV_WriteReg32(&mt8512_gpio->ies[ies_ctrl->index].rst,
				1 << (ies_ctrl->bit));
		} else {
			DRV_WriteReg32(&mt8512_gpio->ies[ies_ctrl->index].set,
				1 << (ies_ctrl->bit));
		}
		ret = 0;
	}
	return ret;
}

static uint32_t gpio_cust_get_ies_enable_chip(uint16_t pin)
{
	const struct mtk_pin_ies_smt_grp *ies_ctrl;
	uint32_t data = 0;
	uint32_t ret = 0;

	if (pin >= MAX_GPIO_PIN) {
		ret = ~(0UL);
	} else {
		ies_ctrl = &mt8512_pin_ies_smt[pin];

		data = DRV_Reg32(&mt8512_gpio->ies[ies_ctrl->index].val);
		ret = ((data & BITS(ies_ctrl->bit)) != 0UL) ? 1UL : 0UL;
	}
	return ret;
}

static int32_t gpio_cust_set_smt_enable_chip(uint16_t pin,
						enum GPIO_SMT_EN enable)
{
	int32_t ret = 0;
	const struct mtk_pin_ies_smt_grp *smt_ctrl;

	if (pin >= MAX_GPIO_PIN) {
		ret = -1;
	} else if (enable >= GPIO_SMT_EN_MAX) {
		ret = -1;
	} else {
		smt_ctrl = &mt8512_pin_ies_smt[pin];

		if (enable == GPIO_SMT_DISABLE) {
			DRV_WriteReg32(&mt8512_gpio->smt[smt_ctrl->index].rst,
				((uint16_t)1 << (smt_ctrl->bit)));
		} else {
			DRV_WriteReg32(&mt8512_gpio->smt[smt_ctrl->index].set,
				((uint16_t)1 << (smt_ctrl->bit)));
		}
		ret = 0;
	}
	return ret;
}

static uint32_t gpio_cust_get_smt_enable_chip(uint16_t pin)
{
	const struct mtk_pin_ies_smt_grp *smt_ctrl;
	uint32_t data = 0;
	uint32_t ret = 0;

	if (pin >= MAX_GPIO_PIN) {
		ret = ~(0UL);
	} else {
		smt_ctrl = &mt8512_pin_ies_smt[pin];
		data = DRV_Reg32(&mt8512_gpio->smt[smt_ctrl->index].val);
		ret = ((data & BITS(smt_ctrl->bit)) != 0UL) ? 1UL : 0UL;
	}
	return ret;
}

static void cli_gpio_read_all(char *buf, int len, int argc, char **argv)
{
	unsigned int pin;
	aos_cli_printf("PIN: (MODE)(DIR)(DOUT)(DIN)(PULL_EN)(PULL_SEL)(IES)(SMT)(DRIVE)\r\n");
	for (pin = 0; pin < 116; pin++)
	    aos_cli_printf("%d: %d %d %d %d %d %d %d %d %d\r\n",
				pin,
				gpio_get_mode(pin),
				gpio_get_dir(pin),
				gpio_get_out(pin),
				gpio_get_in(pin),
				gpio_get_pull_enable(pin),
				gpio_get_pull_select(pin),
				gpio_get_ies_enable(pin),
				gpio_get_smt_enable(pin),
				gpio_get_driving(pin));
}

static void cli_gpio_read_one(char *buf, int len, int argc, char **argv)
{
	unsigned int pin;

	if (argc < 2) {
			aos_cli_printf("Unknown input pin\r\n");
			return 0;
		}

	pin = strtoul(argv[1], NULL, 0);

	if (pin >= 116)
	{
		aos_cli_printf("input pin_num error\r\n");
		return 0;
	}

	aos_cli_printf("PIN: (MODE)(DIR)(DOUT)(DIN)(PULL_EN)(PULL_SEL)(IES)(SMT)(DRIVE)\r\n");
	aos_cli_printf("%d: %d %d %d %d %d %d %d %d %d\r\n",
			pin,
			gpio_get_mode(pin),
			gpio_get_dir(pin),
			gpio_get_out(pin),
			gpio_get_in(pin),
			gpio_get_pull_enable(pin),
			gpio_get_pull_select(pin),
			gpio_get_ies_enable(pin),
			gpio_get_smt_enable(pin),
			gpio_get_driving(pin));
}

static int cli_gpio_write(char *buf, int len, int argc, char **argv)

{
	unsigned int pin;
	int val, val_set=0;
	char *attr_name;

	if (argc < 4) {
        aos_cli_printf("gpio_set: invalid arguments\r\n");
        return 0;
    }

    attr_name= argv[1];
    pin = strtoul(argv[2], NULL, 0);
	val = strtoul(argv[3], NULL, 0);

	if (!strncmp(argv[1], "mode", 4)) {
		val_set = gpio_set_mode( pin, val);
	} else if (!strncmp(argv[1], "dir", 3)) {
		val_set  = gpio_set_dir(pin, val);
	} else if (!strncmp(argv[1], "out", 3)) {
		val_set = gpio_set_out(pin, val);
	} else if (!strncmp(argv[1], "pullen", 6)) {
		val_set = gpio_set_pull_enable(pin, val);
	} else if (!strncmp(argv[1], "pullsel", 7)) {
		val_set = gpio_set_pull_select(pin, val);
	} else if (!strncmp(argv[1], "ies", 3)) {
		val_set = gpio_set_ies_enable(pin, val);
	} else if (!strncmp(argv[1], "smt", 3)) {
		val_set = gpio_set_smt_enable(pin, val);
	} else if (!strncmp(argv[1], "drv", 3)) {
		val_set = gpio_set_driving(pin, val);
	} else {
		aos_cli_printf("input attr error\r\n");
	}
	if (val_set)
		aos_cli_printf("gpio_set [%s] error\r\n", attr_name);
	return 0;
}

static int cli_gpio_help(char *buf, int len, int argc, char **argv)
{
	aos_cli_printf("1. gpio_set,\ngpio_set <attribute> <gpio_num> <value>:\r\n");
	aos_cli_printf("Set specified GPIOattribute:\r\n");
	aos_cli_printf("mode, dir, out, pullen, pullsel, ies, smt, drv\r\n");

	aos_cli_printf("2. pio_read,\ngpio_read <gpio_num>:\nRead specified GPIO status\r\n");

	aos_cli_printf("3. gpio_read_all,\nRead all GPIOs status\r\n");
    return 0;
}

static struct cli_command mt_gpio_cmd[] = {
	{"gpio_read", "gpio_read", cli_gpio_read_one},
	{"gpio_read_all", "gpio_read_all", cli_gpio_read_all},
	{"gpio_set", "gpio_set", cli_gpio_write},
	{"gpio_help", "gpio_help", cli_gpio_help},
};

void gpio_cli_init(void)
{
	aos_cli_register_commands(&mt_gpio_cmd[0],sizeof(mt_gpio_cmd) / sizeof(struct cli_command));
}

uint32_t gpio_get_irq(uint16_t gpio)
{
	uint32_t ret;

	if (gpio >= MAX_GPIO_PIN) {
		ret = 0xffffffU;
	} else {
		ret = gpio;
	}
	return ret;
}

static uint16_t mt_gpio_pin_decrypt(uint16_t pin)
{
	uint16_t gpio;

	gpio = pin & ~(0x80000000U);

	return gpio;
}

/*set GPIO function in fact*/
int32_t gpio_set_dir(uint16_t pin, enum GPIO_DIR dir)
{
	uint16_t gpio;

	gpio = mt_gpio_pin_decrypt(pin);

	return gpio_cust_set_dir_chip(gpio, dir);
}

uint32_t gpio_get_dir(uint16_t pin)
{
	uint16_t gpio;

	gpio = mt_gpio_pin_decrypt(pin);

	return gpio_cust_get_dir_chip(gpio);
}

int32_t gpio_set_pull_enable(uint16_t pin,
				enum GPIO_PULL_EN enable)
{
	uint16_t gpio;

	gpio = mt_gpio_pin_decrypt(pin);

	return gpio_cust_set_pull_enable_chip(gpio, enable);
}

uint32_t gpio_get_pull_enable(uint16_t pin)
{
	uint16_t gpio;

	gpio = mt_gpio_pin_decrypt(pin);

	return gpio_cust_get_pull_enable_chip(gpio);
}

int32_t gpio_set_pull_select(uint16_t pin,
				enum GPIO_PULL select)
{
	uint16_t gpio;

	gpio = mt_gpio_pin_decrypt(pin);

	return gpio_cust_set_pull_select_chip(gpio, select);
}

uint32_t gpio_get_pull_select(uint16_t pin)
{
	uint16_t gpio;

	gpio = mt_gpio_pin_decrypt(pin);

	return gpio_cust_get_pull_select_chip(gpio);
}


int32_t gpio_set_ies_enable(uint16_t pin,
				enum GPIO_IES_EN enable)
{
	uint16_t gpio;

	gpio = mt_gpio_pin_decrypt(pin);

	return gpio_cust_set_ies_enable_chip(gpio, enable);
}

uint32_t gpio_get_ies_enable(uint16_t pin)
{
	uint16_t gpio;

	gpio = mt_gpio_pin_decrypt(pin);

	return gpio_cust_get_ies_enable_chip(gpio);
}

int32_t gpio_set_smt_enable(uint16_t pin,
				enum GPIO_SMT_EN enable)
{
	uint16_t gpio;

	gpio = mt_gpio_pin_decrypt(pin);

	return gpio_cust_set_smt_enable_chip(gpio, enable);
}

uint32_t gpio_get_smt_enable(uint16_t pin)
{
	uint16_t gpio;

	gpio = mt_gpio_pin_decrypt(pin);

	return gpio_cust_get_smt_enable_chip(gpio);
}

int32_t gpio_set_out(uint16_t pin, enum GPIO_OUT output)
{
	uint16_t gpio;

	gpio = mt_gpio_pin_decrypt(pin);

	return gpio_cust_set_out_chip(gpio, output);
}

uint32_t gpio_get_out(uint16_t pin)
{
	uint16_t gpio;

	gpio = mt_gpio_pin_decrypt(pin);

	return gpio_cust_get_out_chip(gpio);
}

uint32_t gpio_get_in(uint16_t pin)
{
	uint16_t gpio;

	gpio = mt_gpio_pin_decrypt(pin);

	return gpio_cust_get_in_chip(gpio);
}

int32_t gpio_set_mode(uint16_t pin, enum GPIO_MODE mode)
{
	uint16_t gpio;

	gpio = mt_gpio_pin_decrypt(pin);

	return gpio_cust_set_mode_chip(gpio, mode);
}

uint32_t gpio_get_mode(uint16_t pin)
{
	uint16_t gpio;

	gpio = mt_gpio_pin_decrypt(pin);

	return gpio_cust_get_mode_chip(gpio);
}


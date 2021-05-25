/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MBED_OBJECTS_H
#define MBED_OBJECTS_H

#include "cmsis.h"
#include "PortNames.h"
#include "PeripheralNames.h"
#include "PinNames.h"
#include "rtl8721d.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gpio_irq_s {
	PinName pin;
} gpio_irq_t;

typedef struct gpio_s {
	PinName pin;
} gpio_t;

struct port_s {
	PortName port;
	uint32_t mask;
};

struct serial_s {
	uint8_t uart_idx;
	uint32_t tx_len;
	uint32_t rx_len;
};

struct spi_s {
	/* user variables */
	uint32_t spi_idx;

	/* internal variables */
	uint32_t irq_handler;
	uint32_t irq_id;
	uint32_t	state;
	uint8_t	sclk;
	uint32_t bus_tx_done_handler;
	uint32_t bus_tx_done_irq_id;
};

struct pwmout_s {
	uint8_t pwm_idx;
	uint32_t period;//in us
	float pulse;//in us
};

struct i2c_s {
	uint32_t i2c_idx;
	I2C_TypeDef * I2Cx;
};

struct flash_s {
	FLASH_InitTypeDef SpicInitPara;
};

struct analogin_s {
	uint8_t	adc_idx;
};

struct gtimer_s {
	void *handler;
	uint32_t hid;
	uint8_t timer_id;
	uint8_t is_periodcal;    
};

struct i2s_s {
	uint8_t i2s_idx;
	uint8_t sampling_rate;
	uint8_t channel_num;
	uint8_t word_length;
	uint8_t direction;
};

struct gdma_s {
	u8 index;
	u8 ch_num;
	IRQ_FUN user_cb;
	u32 user_cb_data;
};

struct captouch_s {
	CapTouch_CHInitTypeDef CT_Channel[4];
	void* irq_handler_press;
	void* irq_handler_release;
};

struct keyscan_s {
	u32 row;
	u32 col;
	u32 clk;
	u32 workmode;	//0 for regular scan mode, 1 for event trigger mode
	u32 keylimit;
	u32 overctrl;	//0 for discard new, 1 for discard oldest
};

struct lcdc_s{
	u32 lcdc_type;
	union{
		LCDC_MCUInitTypeDef LCDC_MCUInitStruct;
		LCDC_RGBInitTypeDef LCDC_RGBInitStruct;
		LCDC_LEDInitTypeDef LCDC_LEDInitStruct;
	}lcdc_if;
};

#ifdef __cplusplus
}
#endif

#endif

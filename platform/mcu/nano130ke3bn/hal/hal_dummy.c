/* Microcontroller Library
 * Copyright (c) 2015-2016 Nuvoton
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

#include <hal/hal.h>
#include "hal_status.h"

//* This file is just only for aos vfs linking. **/

/* gpio */

int32_t hal_gpio_init(gpio_dev_t *gpio)
{
	return HAL_OK;
}

int32_t hal_gpio_output_high(gpio_dev_t *gpio)
{	
	return HAL_OK;
}

int32_t hal_gpio_output_low(gpio_dev_t *gpio)
{
	return HAL_OK;
}

int32_t hal_gpio_output_toggle(gpio_dev_t *gpio)
{
	return HAL_OK;
}

int32_t hal_gpio_input_get(gpio_dev_t *gpio, uint32_t *value)
{
	return HAL_OK;
}

int32_t hal_gpio_enable_irq(gpio_dev_t *gpio, gpio_irq_trigger_t trigger,
                            gpio_irq_handler_t handler, void *arg)
{
	return HAL_OK;
}

int32_t hal_gpio_disable_irq(gpio_dev_t *gpio)
{
	return HAL_OK;
}

int32_t hal_gpio_clear_irq(gpio_dev_t *gpio)
{
	return HAL_OK;
}

int32_t hal_gpio_finalize(gpio_dev_t *gpio)
{
	return HAL_OK;
}

/* spi master */
int32_t hal_spi_init(spi_dev_t *spi)
{
	return HAL_OK;
}

int32_t hal_spi_send(spi_dev_t *spi, const uint8_t *data, uint16_t size, uint32_t timeout)
{
	return HAL_OK;
}


int32_t hal_spi_recv(spi_dev_t *spi, uint8_t *data, uint16_t size, uint32_t timeout)
{
	return HAL_OK;
}

int32_t hal_spi_send_recv(spi_dev_t *spi, uint8_t *tx_data, uint8_t *rx_data,
                          uint16_t size, uint32_t timeout)
{
	return HAL_OK;
}

int32_t hal_spi_finalize(spi_dev_t *spi)
{
	return HAL_OK;
}

/* i2c master */

int32_t hal_i2c_init(i2c_dev_t *i2c)
{
	return HAL_OK;
}

int32_t hal_i2c_master_send(i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
	return HAL_OK;
}

int32_t hal_i2c_master_recv(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
	return HAL_OK;
}

int32_t hal_i2c_slave_send(i2c_dev_t *i2c, const uint8_t *data, uint16_t size, uint32_t timeout)
{
	return HAL_OK;
}

int32_t hal_i2c_slave_recv(i2c_dev_t *i2c, uint8_t *data, uint16_t size, uint32_t timeout)
{
	return HAL_OK;
}

int32_t hal_i2c_mem_write(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                          uint16_t mem_addr_size, const uint8_t *data, uint16_t size,
                          uint32_t timeout)
{
	return HAL_OK;
}

int32_t hal_i2c_mem_read(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                         uint16_t mem_addr_size, uint8_t *data, uint16_t size,
                         uint32_t timeout)
{
	return HAL_OK;
}

int32_t hal_i2c_finalize(i2c_dev_t *i2c)
{
	return HAL_OK;
}

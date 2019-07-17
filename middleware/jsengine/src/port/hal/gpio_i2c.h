/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef BE_GPIO_I2C_H
#define BE_GPIO_I2C_H

void gpio_i2c_delay_10us(uint32_t n);
int8_t gpio_i2c_set_in(uint32_t pin);
int8_t gpio_i2c_set_out(uint32_t pin);
int8_t gpio_i2c_set_high(uint32_t pin);
void gpio_i2c_set_low(uint32_t pin);
int8_t gpio_i2c_read_pin(uint32_t pin);
void gpio_i2c_init(uint32_t scl_pin, uint32_t sda_pin);
void gpio_i2c_start(uint32_t scl_pin, uint32_t sda_pin);
void gpio_i2c_stop(uint32_t scl_pin, uint32_t sda_pin);
uint8_t gpio_i2c_wait_ask(uint32_t scl_pin, uint32_t sda_pin);
void gpio_i2c_reset(uint32_t scl_pin, uint32_t sda_pin);
void gpio_i2c_ask(uint32_t scl_pin, uint32_t sda_pin);
int8_t gpio_i2c_write_byte(uint32_t scl_pin, uint32_t sda_pin, uint8_t dbyte);
uint8_t gpio_i2c_read_byte(uint32_t scl_pin, uint32_t sda_pin);

#endif /* BE_GPIO_I2C_H */

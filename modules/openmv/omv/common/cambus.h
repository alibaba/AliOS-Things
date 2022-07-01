/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Camera bus driver.
 */
#ifndef __CAMBUS_H__
#define __CAMBUS_H__
#include <stdint.h>
#include <stdbool.h>
#include "omv_portconfig.h"

// Transfer speeds
typedef enum _cambus_speed {
    CAMBUS_SPEED_STANDARD  = (0U),
    CAMBUS_SPEED_FULL      = (1U),
    CAMBUS_SPEED_FAST      = (2U),
    CAMBUS_SPEED_MAX       = (3U)
} cambus_speed_t;

// For use with read/write_bytes
typedef enum cambus_xfer_flags {
    // Stop condition after the transfer.
    // Normal transfer with start condition, address, data and stop condition.
    CAMBUS_XFER_NO_FLAGS =   (0<<0),
    // No stop condition after the transfer.
    // This flag allows the next transfer to change direction with repeated start.
    CAMBUS_XFER_NO_STOP  =   (1<<0),
    // No stop condition after the transfer.
    // This flag allows chaining multiple writes or reads with the same direction.
    CAMBUS_XFER_SUSPEND  =   (1<<1),
} cambus_xfer_flags_t;

typedef struct _cambus {
    uint32_t id;
    uint32_t speed;
    uint32_t initialized;
    omv_gpio_t scl_pin;
    omv_gpio_t sda_pin;
    omv_cambus_t i2c;
} cambus_t;

int cambus_init(cambus_t *bus, uint32_t bus_id, uint32_t speed);
int cambus_deinit(cambus_t *bus);
int cambus_scan(cambus_t *bus);
int cambus_gencall(cambus_t *bus, uint8_t cmd);
int cambus_pulse_scl(cambus_t *bus);
int cambus_readb(cambus_t *bus, uint8_t slv_addr, uint8_t reg_addr,  uint8_t *reg_data);
int cambus_writeb(cambus_t *bus, uint8_t slv_addr, uint8_t reg_addr, uint8_t reg_data);
int cambus_readb2(cambus_t *bus, uint8_t slv_addr, uint16_t reg_addr,  uint8_t *reg_data);
int cambus_writeb2(cambus_t *bus, uint8_t slv_addr, uint16_t reg_addr, uint8_t reg_data);
int cambus_readw(cambus_t *bus, uint8_t slv_addr, uint8_t reg_addr,  uint16_t *reg_data);
int cambus_writew(cambus_t *bus, uint8_t slv_addr, uint8_t reg_addr, uint16_t reg_data);
int cambus_readw2(cambus_t *bus, uint8_t slv_addr, uint16_t reg_addr,  uint16_t *reg_data);
int cambus_writew2(cambus_t *bus, uint8_t slv_addr, uint16_t reg_addr, uint16_t reg_data);
int cambus_read_bytes(cambus_t *bus, uint8_t slv_addr, uint8_t *buf, int len, uint32_t flags);
int cambus_write_bytes(cambus_t *bus, uint8_t slv_addr, uint8_t *buf, int len, uint32_t flags);
#endif // __CAMBUS_H__

/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "aos/hal/i2c.h"
#include "driver/i2c_master.h"
#include "esp8266/ets_sys.h"

int32_t hal_i2c_init(i2c_dev_t *i2c)
{
    i2c_master_gpio_init();

    return 0;
}

int32_t hal_i2c_master_send(i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    i2c_master_start();

    i2c_master_writeByte(dev_addr);
    if(!i2c_master_checkAck()) {
        i2c_master_stop();
        return -1;
    }

    i2c_master_writeByte(0x00);
    if(!i2c_master_checkAck()) {
        i2c_master_stop();
        return -1;
    }

    for (int i = 0; i < size; ++i)
    {
        i2c_master_writeByte(data[i]);
        if(!i2c_master_checkAck()) {
            i2c_master_stop();
            return -1;
        }
    }

    i2c_master_stop();

    return 0;
}

int32_t hal_i2c_master_recv(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    int i = 0;

    for (i = 0; i < size; i++) {
        i2c_master_start();

        i2c_master_writeByte(dev_addr);
        if(!i2c_master_checkAck()) {
            i2c_master_stop();
            return -1;
        }
        i2c_master_writeByte(i);
        if(!i2c_master_checkAck()) {
            i2c_master_stop();
            return -1;
        }
        i2c_master_start();
        i2c_master_writeByte(dev_addr + 1);
        if(!i2c_master_checkAck()) {
            i2c_master_stop();
            return -1;
        }

        data[i] = i2c_master_readByte();

        i2c_master_send_nack();
        i2c_master_stop();
    }

    return 0;
}

int32_t hal_i2c_mem_write(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                          uint16_t mem_addr_size, const uint8_t *data, uint16_t size,
                          uint32_t timeout)
{
    i2c_master_start();

    i2c_master_writeByte(dev_addr);
    if(!i2c_master_checkAck()) {
        i2c_master_stop();
        return -1;
    }

    i2c_master_writeByte(0x00);
    if(!i2c_master_checkAck()) {
        i2c_master_stop();
        return -1;
    }

    for (int i = 0; i < size; ++i)
    {
        i2c_master_writeByte(data[i]);
        if(!i2c_master_checkAck()) {
            i2c_master_stop();
            return -1;
        }
    }

    i2c_master_stop();

    return 0;
}

int32_t hal_i2c_mem_read(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                         uint16_t mem_addr_size, uint8_t *data, uint16_t size,
                         uint32_t timeout)
{
    int i = 0;

    for (i = 0; i < size; i++) {
        i2c_master_start();
        i2c_master_writeByte(dev_addr);
        if(!i2c_master_checkAck()) {
            i2c_master_stop();
            return -1;
        }
        i2c_master_writeByte(i);
        if(!i2c_master_checkAck()) {
            i2c_master_stop();
            return -1;
        }
        i2c_master_start();
        i2c_master_writeByte(dev_addr + 1);
        if(!i2c_master_checkAck()) {
            i2c_master_stop();
            return -1;
        }

        data[i] = i2c_master_readByte();

        i2c_master_send_nack();
        i2c_master_stop();
    }

    return 0;
}

int32_t hal_i2c_finalize(i2c_dev_t *i2c)
{
    return 0;
}

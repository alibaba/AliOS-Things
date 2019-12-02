/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "aos/kernel.h"
#include <aos/mbmaster.h>
#include <ulog/ulog.h>

#define DEVICE1_SLAVE_ADDR 0x1
#define DEVICE1_REG1_ADDR  0x0
#define RECV_LEN_MAX       20

#define MODBUSM_APP "modbus_app"

void mb_main()
{
    uint8_t  buf[RECV_LEN_MAX];
    uint8_t  len;
    int      status;
    uint16_t simulator1 = 0, simulator2 = 0;
    uint16_t data_write = 0, data_resp = 0;

    mb_handler_t *mb_handler;

    status = aos_mbmaster_rtu_init(&mb_handler, 2, 9600, MB_PAR_NONE);
    if (status != 0) {
        LOGE(MODBUSM_APP, "mbmaster init error");
        return;
    }

    while (1) {
        memset(buf, 0, RECV_LEN_MAX);

        status = aos_mbmaster_read_holding_register(mb_handler, DEVICE1_SLAVE_ADDR,
                                                    DEVICE1_REG1_ADDR, 2, buf, &len);
        if (status == 0) {
            simulator1 = (uint16_t)((buf[0] << 8) | buf[1]);
            simulator2 = (uint16_t)((buf[2] << 8) | buf[3]);
            LOGI(MODBUSM_APP, "read holding register simulator1: %d,simulator2: %d", simulator1, simulator2);
        } else {
            LOGE(MODBUSM_APP, "read holding register error");
        }

        aos_msleep(1000);

        status = aos_mbmaster_read_input_register(mb_handler, DEVICE1_SLAVE_ADDR,
                                                  DEVICE1_REG1_ADDR, 2, buf, &len);
        if (status == 0) {
            simulator1 = (uint16_t)((buf[0] << 8) | buf[1]);
            simulator2 = (uint16_t)((buf[2] << 8) | buf[3]);
            LOGI(MODBUSM_APP, "read input register simulator1: %d,simulator2: %d", simulator1, simulator2);
        } else {
            LOGE(MODBUSM_APP, "read input register error");
        }

        aos_msleep(1000);

        status = aos_mbmaster_write_single_register(mb_handler, DEVICE1_SLAVE_ADDR,
                                                    DEVICE1_REG1_ADDR, data_write, NULL, &data_resp, NULL);
        if (status == 0) {
            if (data_write != data_resp) {
                LOGE(MODBUSM_APP, "write single register error");
            } else {
                LOGE(MODBUSM_APP, "write single register ok");
            }
        } else {
            LOGE(MODBUSM_APP, "write single register error");
        }
        data_write++;

        aos_msleep(1000);
    }
}

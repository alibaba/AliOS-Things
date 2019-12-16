/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "aos/kernel.h"
#include "modbus/mbmaster.h"
#include "ulog/ulog.h"

/* Define the parameters */
#define SERIAL_PORT         2    /* uart port */
#define SERIAL_BAUD_RATE    9600 /* uart baud rate */
#define DEVICE1_SLAVE_ADDR  0x1  /* remote deveice address */
#define DEVICE1_REG1_ADDR   0x0  /* register address of remote device */
#define RECV_LEN_MAX        20   /* buffer len, must >=  (REQ_REGISTER_NUMBER * 2) */
#define REQ_REGISTER_NUMBER 2    /* Number of registers requested */

/* mode name used by ulog */
#define MODBUSM_APP "modbus_app"

void mb_main(void)
{
    uint8_t     buf[RECV_LEN_MAX];
    uint8_t     len;
    mb_status_t status;
    uint16_t    simulator1 = 0, simulator2 = 0;
    uint16_t    data_write = 0, data_resp = 0;

    /* The handler is allocated by calling the aos_mbmaster_rtu_init */
    mb_handler_t *mb_handler;

    /**
     * Initialize the modbus communication port, using rtu mode.
     * Need to set the appropriatet port number, baud rate, parity
     * according to the actual situation.
     */
    status = mbmaster_rtu_init(&mb_handler, SERIAL_PORT, SERIAL_BAUD_RATE, MB_PAR_NONE);
    if (status != MB_SUCCESS) {
        LOGE(MODBUSM_APP, "mbmaster init error");
        return;
    }

    /* note: This example loops every 2 seconds, first sending a write request, then sending a read request. */
    while (1) {
        /**
         * Initiate write single registers request.
         * data_resp: respond from the remote device. If the write operation succeeds,
         * it's equal to data_write.
         * written.
         */
        status = mbmaster_write_single_register(mb_handler, DEVICE1_SLAVE_ADDR,
                                                    DEVICE1_REG1_ADDR, data_write, NULL, &data_resp, NULL);
        if (status == MB_SUCCESS) {
            if (data_write != data_resp) {
                LOGE(MODBUSM_APP, "write single register error");
            } else {
                LOGI(MODBUSM_APP, "write single register ok");
            }
        } else {
            LOGE(MODBUSM_APP, "write single register error");
        }

        data_write++; /* generate a new value */

        aos_msleep(1000);

        memset(buf, 0, RECV_LEN_MAX);

        /**
         * Initiate read holding registers request
         * The buf length needs to be greater than or equal to (REQ_REGISTER_NUMBER * 2)
         */
        status = mbmaster_read_holding_registers(mb_handler, DEVICE1_SLAVE_ADDR,
                                                    DEVICE1_REG1_ADDR, REQ_REGISTER_NUMBER, buf, &len);
        if (status == MB_SUCCESS) {
            /* The register length on modbus is 16 bits, big end mode  */
            simulator1 = (uint16_t)((buf[0] << 8) | buf[1]);
            simulator2 = (uint16_t)((buf[2] << 8) | buf[3]);
            LOGI(MODBUSM_APP, "read holding register simulator1: %d,simulator2: %d", simulator1, simulator2);
        } else {
            LOGE(MODBUSM_APP, "read holding register error");
        }

        aos_msleep(1000);
    }
}

int application_start(int argc, char *argv[])
{
    LOGI(MODBUSM_APP, "modbus test");
    aos_set_log_level(AOS_LL_DEBUG);

    mb_main();

    return 0;
}


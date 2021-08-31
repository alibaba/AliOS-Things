/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "aos/kernel.h"
#include "mbmaster.h"
#include "ulog/ulog.h"
#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

/* Define the parameters */
#define SERIAL_PORT             (1)    /* uart port */
#define SERIAL_BAUD_RATE        (9600) /* uart baud rate */
#define DEVICE1_SLAVE_ADDR_1    (0x1)  /* remote deveice address */
#define DEVICE1_SLAVE_ADDR_2    (0x2)  /* remote deveice address */
#define DEVICE1_REG1_ADDR       (0x0)  /* register address of remote device */
#define RECV_LEN_MAX            (20)   /* buffer len, must >=  (REQ_REGISTER_NUMBER * 2) */
#define REQ_REGISTER_NUMBER     (10)    /* Number of registers requested */

/* mode name used by ulog */
#define MODBUSM_APP "modbus_app"

void mb_master_main(void)
{
    uint8_t     buf[RECV_LEN_MAX];
    uint8_t     len;
    mb_status_t status;
    uint16_t    simulator1 = 0, simulator2 = 0;
    uint16_t    data_write = 0, data_resp = 0;
    uint16_t   *register_buf;

    /* The handler is allocated by calling the aos_mbmaster_rtu_init */
    mb_handler_t *mb_handler_master;

    /**
     * Initialize the modbus communication port, using rtu mode.
     * Need to set the appropriatet port number, baud rate, parity
     * according to the actual situation.
     */
    status = mbmaster_rtu_init(&mb_handler_master, SERIAL_PORT, SERIAL_BAUD_RATE, MB_PAR_NONE, 500);
    LOGD(MODBUS_MOUDLE, "mbmaster_rtu_init status is %d\n", status);
    if (status != MB_SUCCESS) {
        LOGE(MODBUSM_APP, "mbmaster init error\n");
        return;
    }

    /* note: This example loops every 2 seconds, first sending a write request, then sending a read request. */
    while (1) {
#if 1
        /**
         * Initiate write single registers request.
         * data_resp: respond from the remote device. If the write operation succeeds,
         * it's equal to data_write.
         */
        status = mbmaster_write_single_register(mb_handler_master, DEVICE1_SLAVE_ADDR_1, DEVICE1_REG1_ADDR,
                                                data_write, NULL, &data_resp, NULL, AOS_WAIT_FOREVER);
        if (status == MB_SUCCESS) {
            if (data_write != data_resp) {
                LOGE(MODBUSM_APP, "write single register error\n");
            } else {
                LOGI(MODBUSM_APP, "write single register ok\n");
            }
        } else {
            LOGE(MODBUSM_APP, "write single register error\n");
        }

        data_write++; /* generate a new value */

        aos_msleep(1000 * 3);
#endif

#if 1
        memset(buf, 0, RECV_LEN_MAX);
        /**
         * Initiate read holding registers request
         * The buf length needs to be greater than or equal to (REQ_REGISTER_NUMBER * 2)
         */
        status = mbmaster_read_holding_registers(mb_handler_master, DEVICE1_SLAVE_ADDR_2, DEVICE1_REG1_ADDR,
                                                 REQ_REGISTER_NUMBER, buf, &len, AOS_WAIT_FOREVER);
        if (status == MB_SUCCESS) {
            /* The register length on modbus is 16 bits */
            register_buf = (uint16_t *)buf;
            simulator1 = register_buf[0];
            simulator2 = register_buf[1];
            LOGI(MODBUSM_APP, "read holding register simulator1: %d,simulator2: %d\n", simulator1, simulator2);
        } else {
            LOGE(MODBUSM_APP, "read holding register error\n");
        }
#endif
        aos_msleep(1000 * 3);
    }
}

void mb_slave_main(uint8_t dev_addr)
{
    uint8_t     buf[RECV_LEN_MAX];
    uint8_t     len;
    mb_status_t status;
    uint16_t    simulator1 = 0, simulator2 = 0;
    uint16_t    data_write = 0, data_resp = 0;
    uint16_t   *register_buf;

    /* The handler is allocated by calling the aos_mbmaster_rtu_init */
    mb_handler_t *mb_handler_slave = NULL;

    /**
     * Initialize the modbus communication port, using rtu mode.
     * Need to set the appropriatet port number, baud rate, parity
     * according to the actual situation.
     */
    status = mbmaster_rtu_init(&mb_handler_slave, SERIAL_PORT, SERIAL_BAUD_RATE, MB_PAR_NONE, 100);
    LOGD(MODBUS_MOUDLE, "mbmaster_rtu_init status is %d\n", status);
    if (status != MB_SUCCESS) {
        LOGE(MODBUSM_APP, "mbmaster init error\n");
        return;
    }

    /* wait slave respond */
    LOGD(MODBUS_MOUDLE, "device [0x%x] waiting for cmd\n", dev_addr);
    while (1) {
        /* frame recv */
        status = mb_handler_slave->frame_recv(mb_handler_slave);
        if (status == MB_SUCCESS) {
            /* frame handle and response */
            status = mb_handler_slave->adu_disassemble(mb_handler_slave);
            if (status == MB_SUCCESS) {
                uint8_t addr = mb_handler_slave->slave_addr;
                uint8_t len = mb_handler_slave->pdu_length;
                uint8_t offset = mb_handler_slave->pdu_offset;
                uint8_t *data = mb_handler_slave->mb_frame_buff;

                if (addr == dev_addr) {
                    switch (addr) {
                        case DEVICE1_SLAVE_ADDR_1:
                        // no need to process 0x06
                            mb_handler_slave->pdu_length = len;
                            break;

                        case DEVICE1_SLAVE_ADDR_2:
                        // 0x01 0x03 0x00 0x00 0x00 0x0a 0xc5 0xcd
                        {
                            uint16_t reg_start = data[2] << 8 | data[3];
                            uint16_t reg_len   = data[4] << 8 | data[5];

                            mb_handler_slave->pdu_length = reg_len * 2 + 2;

                            data[ADU_SER_PDU_OFF + 1] = reg_len * 2;
                            for (uint16_t k = 0; k < reg_len; k++) {
                                data[ADU_SER_PDU_OFF + 2 + k * 2] = 0x00;
                                data[ADU_SER_PDU_OFF + 2 + k * 2 + 1] = k & 0xFF;
                            }
                            break;
                        }

                        default:
                            LOGE(MODBUS_MOUDLE, "Unsupported device addr [0x%2X]\n", addr);
                    }

                    status = mb_handler_slave->adu_assemble(mb_handler_slave);
                    status = mb_handler_slave->frame_send(mb_handler_slave, 100);
                    if (status != MB_SUCCESS) {
                        LOGD(MODBUS_MOUDLE, "Failed to send response to master\n");
                    }
                }
            }
        }
    }
}

int mbmaster_comp_example(int argc, char *argv[])
{
    aos_set_log_level(AOS_LL_DEBUG);

    LOGI(MODBUSM_APP, "mbmaster test\n");

    mb_master_main();

    return 0;
}

int mbslave_comp_example(int argc, char *argv[])
{
    aos_set_log_level(AOS_LL_DEBUG);

    if (argc == 1) {
        mb_slave_main(DEVICE1_SLAVE_ADDR_1);
    } else {
        mb_slave_main(*argv[1] - 48);
    }

    return 0;
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(mbmaster_comp_example, mbmaster_example, mbmaster component base master example)
ALIOS_CLI_CMD_REGISTER(mbslave_comp_example, mbslave_example, mbmaster component base salve example)
#endif


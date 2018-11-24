/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <string.h>

#include <aos/aos.h>
#include <k_api.h>

#include <aos/aos_mbmaster.h>

#define READCOILS_TEST 0
#define ZZ_S_TH_C      0
#define MSF1000MB      0
#define MSPM25MB       0
#define SIMULATOR      1

#define ZZ_S_TH_C_SLAVE_ADDR 0x1
#define MSF1000MB_SLAVE_ADDR 0x4
#define MSPM25MB_SLAVE_ADDR  0x5
#define SIMULATOR_SLAVE_ADDR 0x1

#define SIMULATOR_REG1_ADDR 0x0
#define SIMULATOR_REG2_ADDR 0x1

#define HUMIDITY_REG_ADDR    0x0
#define TEMPERATURE_REG_ADDR 0x1
#define COILS_ADDR           0x2
#define FD_REG_ADDR          0x8
#define PM25_REG_ADDR        0x5

#define HUMIDITY_IS_VALID     (0x1 << 0)
#define TEMPERATURE_IS_VALID  (0x1 << 1)
#define FORMALDEHYDE_IS_VALID (0x1 << 2)
#define PM25_IS_VALID         (0x1 << 3)
#define PM1_IS_VALID          (0x1 << 4)
#define PM10_IS_VALID         (0x1 << 5)
#define SIMULATOR_IS_VALID    (0x1 << 6)

void mb_main()
{
    uint8_t  buf[20];
    uint8_t  len;
    int      status;
    float    humidity = 0;
    float    temperature = 0;
    int16_t  formaldehyde = 0;
    int16_t  pm25 = 0;
    int16_t  pm1  = 0;
    int16_t  pm10 = 0;
    uint16_t simulator1 = 0, simulator2 = 0;
    uint32_t data_valid = 0;

    mb_handler_t *mb_handler;

    aos_mbmaster_rtu_init(&mb_handler, 2, 9600, MB_PAR_NONE);

    while (1) {
        data_valid = 0;
        memset(buf, 0, 10);

#if SIMULATOR
        status = aos_mbmaster_read_holding_reginster(mb_handler, SIMULATOR_SLAVE_ADDR,
                                                     SIMULATOR_REG1_ADDR, 2, buf, &len);

        if (status == 0) {
            simulator1 = (uint16_t)((buf[0] << 8) | buf[1]);
            simulator2 = (uint16_t)((buf[2] << 8) | buf[3]);
            data_valid |= SIMULATOR_IS_VALID;
        } else {
            printf("read holding register error\n");
        }
#endif

#if ZZ_S_TH_C
        status = aos_mbmaster_read_holding_reginster(mb_handler, ZZ_S_TH_C_SLAVE_ADDR,
                                                     HUMIDITY_REG_ADDR, 1, buf, &len);

        if (status == 0) {
            humidity = (float)((buf[0] << 8) | buf[1]);
            humidity = humidity / 10.0;
            data_valid |= HUMIDITY_IS_VALID;
        } else {
            printf("read holding register error\n");
        }

        status = aos_mbmaster_read_holding_reginster(mb_handler, 0x1, TEMPERATURE_REG_ADDR,
                                                     1, buf, &len);
        if (status == 0) {
            temperature = (float)((buf[0] << 8) | buf[1]);
            temperature = temperature / 10.0;
            data_valid |= TEMPERATURE_IS_VALID;
        } else {
            printf("read holding register error\n");
        }
#endif

#if MSPM25MB
        status = aos_mbmaster_read_holding_reginster(mb_handler, MSPM25MB_SLAVE_ADDR,
                                                     PM25_REG_ADDR, 3, buf, &len);
        if (status == 0) {
            pm25 = (buf[0] << 8) | buf[1];
            pm1  = (buf[2] << 8) | buf[3];
            pm10 = (buf[4] << 8) | buf[5];
            data_valid |= PM25_IS_VALID | PM1_IS_VALID | PM10_IS_VALID;
        } else {
            printf("read holding register error\n");
        }
#endif

#if MSF1000MB
        status = aos_mbmaster_read_holding_reginster(mb_handler, MSF1000MB_SLAVE_ADDR,
                                                     FD_REG_ADDR, 1, buf, &len);
        if (status == 0) {
            formaldehyde = (buf[0] << 8) | buf[1];
            data_valid |= FORMALDEHYDE_IS_VALID;
        } else {
            printf("read holding register error\n");
        }
#endif

        if (data_valid & HUMIDITY_IS_VALID) {
            printf("humidity    : %4.1f %%RH\n", humidity);
        }

        if (data_valid & TEMPERATURE_IS_VALID) {
            printf("temperature : %4.1f C\n", temperature);
        }

        if (data_valid & PM25_IS_VALID) {
            printf("pm25        : %d ug/m3\n", pm25);
        }

        if (data_valid & PM1_IS_VALID) {
            printf("pm1         : %d ug/m3\n", pm1);
        }

        if (data_valid & PM10_IS_VALID) {
            printf("pm10        : %d ug/m3\n", pm10);
        }

        if (data_valid & FORMALDEHYDE_IS_VALID) {
            printf("formaldehyde: %d ug/m3\n", formaldehyde);
        }

        if (data_valid & SIMULATOR_IS_VALID) {
            printf("simulator1: %d,simulator2: %d\n", simulator1, simulator2);
        }

        aos_msleep(1000);
    }
}

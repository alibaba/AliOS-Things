/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <k_api.h>

#include <mbmaster_api.h>

#define TEST_CONFIG_STACK_SIZE 256

static ktask_t     g_task;
static cpu_stack_t stack_buf[TEST_CONFIG_STACK_SIZE];

#define READCOILS_TEST 0
#define ZZ_S_TH_C      0
#define MSF1000MB      0
#define MSPM25MB       0
#define SIMULATOR      1

#define ZZ_S_TH_C_SLAVE_ADDR 0x1
#define MSF1000MB_SLAVE_ADDR 0x4
#define MSPM25MB_SLAVE_ADDR  0x5
#define SIMULATOR_SLAVE_ADDR 0x1

#define SIMULATOR_REG1_ADDR 0x40000
#define SIMULATOR_REG2_ADDR 0x40001

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

static void mb_task();

int mb_main()
{
    kstat_t        ret;
    uint8_t        buf[10];
    uint8_t        len;
    uint8_t        i;
    mb_exception_t status;
    float          humidity;
    float          temperature;
    int16_t        formaldehyde;
    int16_t        pm25;
    int16_t        pm1;
    int16_t        pm10;
    uint16_t       simulator1, simulator2;
    uint32_t       data_valid;

    ret = krhino_task_create(&g_task, "mbtask", NULL, 10, 50, stack_buf,
                             TEST_CONFIG_STACK_SIZE, mb_task, 1);
    if (ret != RHINO_SUCCESS) {
        return false;
    }

    while (1) {
        data_valid = 0;

#if SIMULATOR
        status = mb_read_holding_reginster(SIMULATOR_SLAVE_ADDR,
                                           SIMULATOR_REG1_ADDR, 2, buf, &len);

        if (status == MB_EX_NONE) {
            simulator1 = (uint16_t)((buf[0] << 8) | buf[1]);
            simulator2 = (uint16_t)((buf[2] << 8) | buf[3]);
            data_valid |= SIMULATOR_IS_VALID;
        } else {
            printf("%s\n", status_to_string(status));
        }
#endif

#if ZZ_S_TH_C
        status = mb_read_holding_reginster(ZZ_S_TH_C_SLAVE_ADDR,
                                           HUMIDITY_REG_ADDR, 1, buf, &len);

        if (status == MB_EX_NONE) {
            humidity = (float)((buf[0] << 8) | buf[1]);
            humidity = humidity / 10.0;
            data_valid |= HUMIDITY_IS_VALID;
        } else {
            printf("%s\n", status_to_string(status));
        }

        status =
          mb_read_holding_reginster(0x1, TEMPERATURE_REG_ADDR, 1, buf, &len);
        if (status == MB_EX_NONE) {
            temperature = (float)((buf[0] << 8) | buf[1]);
            temperature = temperature / 10.0;
            data_valid |= TEMPERATURE_IS_VALID;
        } else {
            printf("%s\n", status_to_string(status));
        }
#endif

#if MSPM25MB
        status = mb_read_holding_reginster(MSPM25MB_SLAVE_ADDR, PM25_REG_ADDR,
                                           3, buf, &len);
        if (status == MB_EX_NONE) {
            pm25 = (buf[0] << 8) | buf[1];
            pm1  = (buf[2] << 8) | buf[3];
            pm10 = (buf[4] << 8) | buf[5];
            data_valid |= PM25_IS_VALID | PM1_IS_VALID | PM10_IS_VALID;
        } else {
            printf("%s\n", status_to_string(status));
        }
#endif

#if MSF1000MB
        status = mb_read_holding_reginster(MSF1000MB_SLAVE_ADDR, FD_REG_ADDR, 1,
                                           buf, &len);
        if (status == MB_EX_NONE) {
            formaldehyde = (buf[0] << 8) | buf[1];
            data_valid |= FORMALDEHYDE_IS_VALID;
        } else {
            printf("%s\n", status_to_string(status));
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

        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND * 10);
    }
}

static void mb_task()
{

    mb_init(MB_RTU, 2, 9600, MB_PAR_NONE); /* uart2 */
    mb_enable();

    while (1) {
        mb_poll();
    }
}

/*
 * Copyright (C) 2021 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <aos/adc.h>
#include <aos/kernel.h>

#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

/*TESTCASE1: Get ADC raw data, and then convert it into voltage. */
static int aos_adc_test_raw_data(uint32_t id, uint32_t channel)
{
    int ret;
    aos_adc_ref_t ref;
    int32_t data = -1;
    uint32_t resolution = 0;
    uint32_t range = 0;
    int32_t voltage = 0;

    ret = aos_adc_get(&ref, id);
    if (ret != 0) {
        printf("aos_adc_get fail, ret:%d\r\n", ret);
        return -1;
    }

    ret = aos_adc_read(&ref, channel, &data);
    if (ret != 0) {
        printf("aos_adc_read fail, ret:%d\r\n", ret);
        return -1;
    }
    printf("Read raw data: %d rom device:%d channel:%d\r\n", data, id, channel);

    ret = aos_adc_get_resolution(&ref, &resolution);
    if (ret != 0) {
        printf("aos_adc_get_resolution fail, ret:%d\r\n", ret);
        return -1;
    }

    ret = aos_adc_get_range(&ref, channel, &range);
    if (ret != 0) {
        printf("aos_adc_get_range fail, ret:%d\r\n", ret);
        return -1;
    }

    voltage = (data * range) / (1 << resolution);

    printf("%s:Read : %d.%dV from device:%d channel:%d\r\n",
            __func__, voltage / 1000, voltage % 1000, id, channel);

    aos_adc_put(&ref);
    return 0;
}

/*TESTCASE2: Get the input voltage directly. */
static int aos_adc_test_voltage(uint32_t id, uint32_t channel)
{
    int ret;
    aos_adc_ref_t ref;
    int32_t data = -1;

    ret = aos_adc_get(&ref, id);
    if (ret != 0) {
        printf("aos_adc_get fail, ret:%d\r\n", ret);
        return -1;
    }

    ret = aos_adc_read_voltage(&ref, channel, &data);
    if (ret != 0) {
        printf("aos_adc_read fail, ret:%d\r\n", ret);
        return -1;
    }
    printf("%s:Read voltage: %d.%dV from device:%d channel:%d\r\n",
            __func__, data / 1000, data % 1000, id, channel);

    aos_adc_put(&ref);
    return 0;
}

static void aos_adc_cli_cmd(int argc, char **argv)
{
    aos_status_t ret;
    uint32_t index = argc > 1 ? atoi(argv[1]) : 0;
    uint32_t channel = argc > 2 ? atoi(argv[2]) : 0;

    printf("aos adc example test begin ...\r\n");
    printf("argc:%d, device index:%d, channel:%d\r\n", argc, index, channel);
    ret = aos_adc_test_raw_data(index, channel);
    if (ret != 0) {
        printf("aos adc example test fail, ret:%d!\r\n", ret);
        return;
    }
    ret = aos_adc_test_voltage(index, channel);

    printf("aos adc example test %s!\r\n", (ret == 0) ? "success" : "fail");
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(aos_adc_cli_cmd, aos_adc_example, aos adc example)
#endif

/*
 * Copyright (C) 2021 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <aos/pwm.h>
#include <aos/kernel.h>

#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

void aos_pwm_cli_help(void)
{
    printf("help:\r\n");
    printf("period[ns]\r\n");
    printf("duty[ns]\r\n");
    printf("duration[s]\r\n");
    printf("polarity 0:normal 1:inverse\r\n");
    printf("aos_pwm_example channel period duty polarity enable duration\r\n");
}
static void aos_pwm_cli_cmd(int argc, char **argv)
{
    aos_status_t ret;
    aos_pwm_ref_t ref;
    uint32_t channel, period, duty_cycle, polarity, enabled, duration;
    aos_pwm_attr_t attr;

    if (argc != 7) {
        aos_pwm_cli_help();
        return;
    }
    printf("aos pwm example test begin ...\r\n");
    channel  = strtoul(argv[1], NULL, 0);
    period = strtoul(argv[2], NULL, 0);
    duty_cycle = strtoul(argv[3], NULL, 0);
    polarity = strtoul(argv[4], NULL, 0);
    enabled = strtoul(argv[5], NULL, 0);
    duration = strtoul(argv[6], NULL, 0);
    ret = aos_pwm_get(&ref, channel);
    if (ret) {
        printf("aos_pwm_get fail, ret %d\r\n", ret);
        return;
    }
    attr.period = period;
    attr.duty_cycle = duty_cycle;
    attr.enabled = enabled;
    attr.polarity = polarity;
    ret = aos_pwm_set_attr(&ref, &attr);
    if (ret) {
        printf("aos_pwm_set_attr fail, ret %d\r\n", ret);
        goto out;
    }
    ret = aos_pwm_get_attr(&ref, &attr);
    if (ret) {
        printf("aos_pwm_get_attr fail, ret %d\r\n", ret);
        goto out;
    }
    printf("aos_pwm_attr_get:\r\n");
    printf("period %u ns\r\n", attr.period);
    printf("duty_cycle %u ns\r\n", attr.duty_cycle);
    printf("polarity %s\r\n", attr.polarity ? "inverse" : "normal");
    printf("enabled %u\r\n", attr.enabled);
    aos_msleep(1000 * duration);
    printf("aos_pwm_test success\r\n");
 out:
    aos_pwm_put(&ref);
    return;
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(aos_pwm_cli_cmd, aos_pwm_example, aos pwm example)
#endif

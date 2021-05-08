/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stdint.h>
/*#include "aos/i2c.h"*/
#include "pca9544.h"
#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

static void pca9544_comp_example(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int          ret;
    uint8_t      data[7]     = { 8, 25, 18, 7, 3, 1, 21 };
    uint8_t      data_rtn[7] = { 0 };
    uint16_t     size        = 7;
    PCA9544_CH_E test_chan   = PCA9544_CH1;
    PCA9544_CH_E read_chan   = PCA9544_CH_NULL;
    uint8_t      reg_rtn;
    uint8_t      chan_mask = 0x05;

    i2c_dev_t         i2c;
    PCA9544_DEV_CFG_T dev_cfg;

    dev_cfg.dev_addr    = PCA9544_BASE_ADDR;
    dev_cfg.pca9544_ch  = test_chan;
    dev_cfg.subdev_addr = 0x32;
    dev_cfg.reg_addr    = 0x10;

    i2c.port                 = 1;
    i2c.config.address_width = 8;
    i2c.config.freq          = I2C_BUS_BIT_RATES_100K;
    i2c.config.dev_addr      = dev_cfg.dev_addr;

    ret = pca9544_init(&i2c, &dev_cfg);
    if (ret) {
        printf("=====I2C Muxer PCA9544 test: init fail =====\r\n");
        return -1;
    }

    /* chan select */
    printf("=====I2C Muxer PCA9544 test: set chan 2[%d] of PCA9544=====\r\n", test_chan);
    ret = pca9544_set_chan(test_chan);

    /*aos_sleep(2);*/

    read_chan = pca9544_read_chan();

    printf("=====I2C Muxer PCA9544 test: read chan val of PCA9544 is %d=====\r\n", read_chan);

    /*aos_sleep(2);*/
    if (test_chan == read_chan) {
        printf("=====I2C Muxer PCA9544 test: PCA9544 test:  PASS=====\r\n");
    } else {
        printf("=====I2C Muxer PCA9544 test: PCA9544 test:  FAIL=====\r\n");
        pca9544_deinit();
        ret = -1;
    }

    pca9544_deinit();
    printf("pca9544 comp test success!\r\n");
    return;
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(pca9544_comp_example, pca9544_example, pca9544 component example)
#endif

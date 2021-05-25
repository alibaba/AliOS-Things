/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#define PORT_I2C_TEST 0

/* if the slave device addr is 0xFF,  and your mcu only need to set the addr to 0xFE,
   the last bit will be added by the hardware, set I2C_ADD_LSB_AUTO to 1. If your
   mcu will not add the last bit auto, you must set the addr to 0xFF, set
   I2C_ADD_LSB_AUTO to 1.
   set  */
#define I2C_ADD_LSB_AUTO 0

void hal_i2c_test(void);

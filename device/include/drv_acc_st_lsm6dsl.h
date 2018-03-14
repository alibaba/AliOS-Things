#ifndef __DRV_ACC_ST_LSM6DSL_H
#define __DRV_ACC_ST_LSM6DSL_H

typedef enum {
	ACC_ST_LSM6DSL_ADDR_LOW = 0,
	ACC_ST_LSM6DSL_ADDR_HIGH
} ACC_ST_LSM6DSL_ADDR_SEL;

int drv_acc_st_lsm6dsl_init(i2c_dev_t *i2c_dev, ACC_ST_LSM6DSL_ADDR_SEL i2c_addr_sel);

#endif /* __DRV_ACC_ST_LSM6DSL_H */
#ifndef __DRV_ACC_MIR3_DA217_H
#define __DRV_ACC_MIR3_DA217_H

typedef enum {
	ACC_MIR3_DA217_ADDR_LOW = 0,
	ACC_MIR3_DA217_ADDR_HIGH
} ACC_MIR3_DA217_ADDR_SEL;

int drv_acc_mir3_da217_init(i2c_dev_t *i2c_dev, ACC_MIR3_DA217_ADDR_SEL i2c_addr_sel);

#endif /* __DRV_ACC_MIR3_DA217_H */
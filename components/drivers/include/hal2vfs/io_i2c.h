/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _IO_I2C_H_
#define _IO_I2C_H_

#define IOC_I2C_BASE 'I'
#define IOC_I2C_SET_FREQ IOC_I2C_BASE + 1
#define IOC_I2C_SET_CONFIG IOC_I2C_BASE + 2
#define IOC_I2C_MASTER_RX IOC_I2C_BASE + 3
#define IOC_I2C_MASTER_TX IOC_I2C_BASE + 4
#define IOC_I2C_SLAVE_RX IOC_I2C_BASE + 5
#define IOC_I2C_SLAVE_TX IOC_I2C_BASE + 6
#define IOC_I2C_MEM_RX IOC_I2C_BASE + 7
#define IOC_I2C_MEM_TX IOC_I2C_BASE + 8

typedef union {
    struct {
        unsigned int addr_width:2;
        unsigned int role:1;
        unsigned int addr:16;
    } c;
    uint32_t freq;
} io_i2c_control_u;

typedef struct {
    unsigned short addr;
    unsigned short length;
    unsigned char *data;
    unsigned short maddr; // memory address, used in IOC_I2C_MEM_TX/IOC_I2C_MEM_RX
    unsigned short mlength; // memory length, used in IOC_I2C_MEM_TX/IOC_I2C_MEM_RX
    unsigned int timeout; // in unit of ms
} io_i2c_data_t;

int vfs_i2c_drv_init (void);

#endif //_IO_I2C_H_

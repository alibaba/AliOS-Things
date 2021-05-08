/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _IO_I2C_H_
#define _IO_I2C_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup i2c_device_api
 *  @ingroup driver_api
 * @{
 */

 /* I2C VFS IOCTL CMD宏定义 */
#define IOC_I2C_BASE 'I'
#define IOC_I2C_SET_FREQ IOC_I2C_BASE + 1    /**< 设定I2C频率cmd宏定义 */
#define IOC_I2C_SET_CONFIG IOC_I2C_BASE + 2  /**< 设定除频率外其它I2C参数cmd宏定义 */
#define IOC_I2C_MASTER_RX IOC_I2C_BASE + 3   /**< 用master模式发起I2C接收操作cmd宏定义 */
#define IOC_I2C_MASTER_TX IOC_I2C_BASE + 4   /**< 用master模式发起I2C发送操作cmd宏定义 */
#define IOC_I2C_SLAVE_RX IOC_I2C_BASE + 5    /**< 用slave模式发起I2C接收操作cmd宏定义 - 暂不支持 */
#define IOC_I2C_SLAVE_TX IOC_I2C_BASE + 6    /**< 用slave模式发起I2C发送操作cmd宏定义 - 暂不支持 */
#define IOC_I2C_MEM_RX IOC_I2C_BASE + 7      /**< 用master模式发起I2C读取寄存器操作cmd宏定义 */
#define IOC_I2C_MEM_TX IOC_I2C_BASE + 8      /**< 用master模式发起I2C写入寄存器操作cmd宏定义 */

 /* 设定i2c参数所用结构体 */
typedef union {
    struct {
        unsigned int addr_width:2;  /**< 0：7-bit地址模式；1：10-bit地址模式 */
        unsigned int role:1;        /**< 1：i2c主设备模式；0：i2c从设备模式，暂不支持从设备模式设定 */
        unsigned int addr:16;       /**< i2c从设备地址 */
    } c;
    uint32_t freq;                  /**< i2c clk引脚时钟频率 */
} io_i2c_control_u;

 /* i2c数据收发操作所用结构体 */
typedef struct {
    unsigned short addr;            /**< 此次读写操作对应的i2c送设备地址 */
    unsigned short length;          /**< 此次读写操作要读写数据的长度 */
    unsigned char *data;            /**< 写操作：指向要发送数据的首地址；读操作：指向接收数据存放内存块首地址 */
    unsigned short maddr;           /**< 此次内存操作要读写的memory地址，只有在ioctl cmd id为IOC_I2C_MEM_TX/IOC_I2C_MEM_RX时有效 */
    unsigned short mlength;         /**< 此次内存操作要读写的memory的长度，只有在ioctl cmd id为IOC_I2C_MEM_TX/IOC_I2C_MEM_RX时有效 */
    unsigned int timeout;           /**< 此次操作的超时时间，单位：ms */
} io_i2c_data_t;

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif //_IO_I2C_H_

[EN](AliOS-Things-VFS-Porting-Guide)| 中文

VFS用于为各类文件（包括设备和文件系统的文件）提供了一个统一的用户操作接口。

# 目录
  * [1 设备驱动](#1设备驱动)
  * [2 文件系统](#2文件系统)
------
# 1设备驱动
## 1.1 设备驱动对接hal层
1）打开kernel/vfs/include/device/vfs_device.h。
```C
/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AOS_VFS_DEVICE_H
#define AOS_VFS_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "vfs_adc.h"
#include "vfs_gpio.h"
#include "vfs_i2c.h"
#include "vfs_pwm.h"
#include "vfs_rtc.h"
#include "vfs_spi.h"
#include "vfs_uart.h"
#include "vfs_wdg.h"

#ifdef __cplusplus
}
#endif

#endif /* AOS_VFS_DEVICE_H */
```
2）这里包含了所有支持的设备的头文件，如果包含所要使用的设备则继续，否则反馈给我们。如果所要使用的设备支持，则打开include/hal/soc/路径，找到对应的hal头文件（如uart设备则打开uart.h），并实现该头文件中所声明的所有函数。对接方法见AliOS Things HAL Porting Guide.

## 1.2 注册驱动设备
1）在kernel/vfs/include/vfs_register.h中找到注册函数
```C
int aos_register_driver(const char *path, file_ops_t *fops, void *arg)
```
第一个参数path为设备路径，如uart设备可以定义为const char* uart_path = "/dev/uart"，设备路径必须以"/"开始，总长度不能超过4096.

第二个参数fops为设备驱动结构体，在kernel/vfs/include/device/路径下对应的头文件中已经定义好，直接使用即可。 如uart对应的fops在kernel/vfs/include/device/vfs_uart.h中为uart_ops，引用该目录下的vfs_device.h即可引用该结构体。

第三个参数arg为指向设备配置信息的指针，adc的设备配置信息结构体定义在hal层include/hal/soc/uart.h中，为uart_dev_t。定义uart配置信息结构体，如定义uart_dev_t    uart_dev_test，并根据具体设备进行初始化，并调用aos_register_driver函数注册。
```C
ret = aos_register_driver（uart_path, &uart_ops, &uart_dev_test)
```
如果返回值为0则注册成功。
如果系统中有多个同类型设备，则需要对每一个设备进行注册，如存在两个uart，分别为uart1和uart2。
```C
const char* uart1_path = "/dev/uart1";
const char* uart2_path = "/dev/uart2"

i2c_dev_t uart1_dev_test =
{
    .port = PORT_UART1,
    .config.baud_rate = 115200,
    .config.data_width = DATA_WIDTH_8BIT,
    .config.parity = ODD_PARITY,
    .config.hal_uart_stop_bits_t = STOP_BITS_1,
    .config.hal_uart_flow_control_t = FLOW_CONTROL_CTS
};

i2c_dev_t uart2_dev_test =
{
    .port = PORT_UART2,
    .config.baud_rate = 460800,
    .config.data_width = DATA_WIDTH_8BIT,
    .config.parity = EVEN_PARITY,
    .config.hal_uart_stop_bits_t = STOP_BITS_1,
    .config.hal_uart_flow_control_t = FLOW_CONTROL_CTS
};

ret1 = aos_register_driver（uart1_path, &uart_ops, &uart1_dev_test);
ret2 = aos_register_driver（uart2_path, &uart_ops, &uart2_dev_test);
```
设备ops保持一致均为uart_ops，其他两个参数根据具体进行配置。注意设备配置信息中的port参数与HAL对阶层保持一致，便于驱动进行解析。
## 1.3 设备打开
```C
int fd_uart;
fd_uart = aos_open(uart_path,0);
```
第二个参数目前未使用，可直接设置为0，如果返回值大于AOS_CONFIG_VFS_FD_OFFSET则打开成功.

## 1.4 设备读操作
如果要从串口读取10个字节的数据，可采用如下方式：
```C
int ret;
char read_buf[10];

ret = aos_read(fd_uart, read_buf, 10);
```
返回值等于10则数据读取成功.

## 1.5 设备写操作
如果要从串口发送10个字节的数据，可采用如下方式：
```C
int ret;
char write_buf[10];

ret = aos_write(fd_uart, write_buf, 10);
```
返回值等于10则数据发送成功.

## 1.6 设备IO操作
设备IO操作通过aos_ioctl函数实现，例如GPIO中，如果要输出高低电平，可采用如下方式：
```C
/* output high */
ret = aos_ioctl(fd_gpio, IOCTL_GPIO_OUTPUT_HIGHT, 0);

/* output low */
ret = aos_ioctl(fd_gpio, IOCTL_GPIO_OUTPUT_LOW, 0);
```
第二个参数代表所要进行的IO操作，各个设备支持的IO操作见kernel/vfs/include/device/目录下对应的设备头文件。如vfs_gpio.h中：
```C
/* cmd for ioctl */
#define IOCTL_GPIO_OUTPUT_HIGHT  1  /* output hight */
#define IOCTL_GPIO_OUTPUT_LOW    2  /* output low */
#define IOCTL_GPIO_OUTPUT_TOGGLE 3  /* toggle output */
```
## 1.7 设备关闭操作
```C
ret = aos_close(fd_uart);
```
返回值为0则关闭成功，设备关闭后要再次使用需重新打开.
## 1.8 其他
rtc设备数据格式已经定义好，见include/hal/soc/rtc.h，建议按照如下方式进行读取和设置：
```C
rtc_time_t rtc_time_get;
rtc_time_t rtc_time_set;

/* get rtc */
ret = aos_read(fd_rtc, &rtc_time_get, sizeof(rtc_time_get));

/* set rtc */
ret = aos_write(fd_rtc, &rtc_time_set, sizeof(rtc_time_set));
```


# 2文件系统
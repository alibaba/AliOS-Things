EN| [中文](AliOS-Things-VFS-Porting-Guide.zh)

VFS provides a unified user interface for various files (including device files and system files).

# Content

- [1 Device driver](#1设备驱动)
- [2 file system](#2文件系统)

------

# 1 Device driver

## 1.1 Porting of device driver to HAL

1）Open kernel/vfs/include/device/vfs_device.h.

```c
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

2）Header files for all devices supported by us are included. If the device you used is included, you can continue. Otherwise, you can contact us. For supported devices, you can open include/hal/soc/ to find their corresponding HAL header file (for example, uart device can open uart.h), and you can implement all the declared functions in that file. Porting methods can refer to AliOS Things HAL Porting Guide.

## 1.2 Register drivers

1）Find out registration functions in kernel/vfs/include/vfs_register.h.

```c
int aos_register_driver(const char *path, file_ops_t *fops, void *arg)
```

The first parameter "path" is device path. For example, uart device can be defined as const char* uart_path = /dev/uart. Device path must start with "/", and total length should not exceed 4096..

The second parameter "fops" is device driver struct, which is defined in corresponding header file in kernel/vfs/include/device/, and can be used directly. For uart, its corresponding fops is located in kernel/vfs/include/device/vfs_uart.h. You can use this struct referring to vfs_device.h.

The third parameter "arg" is pointer to device configuration information. Configuration information struct of adc is defined in include/hal/soc/uart.h in HAL as uart_dev_t. You can define uart configuration information struct, such as uart_dev_t uart_dev_test, initialize it based on specific device, and call aos_register_driver function to register.

```c
ret = aos_register_driver（uart_path, &uart_ops, &uart_dev_test)
```

That the return value is 0 means registration has been successful.

If there are multiple devices in the system, each device needs to be registered. For example, if there are two uart, they should be uart1 and uart2 respectively.

```c
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

Set device ops as uart_ops uniformly, and other two parameters can be specified respectively. Parameter "port" in device configuration information should be consistent with that in HAL class, in order to facilitate the parsing of drivers.

## 1.3 Open the device

```c
int fd_uart;
fd_uart = aos_open(uart_path,0);
```

The second parameter has not been used currently, and you can set it as 0. If return value is greater than AOS_CONFIG_VFS_FD_OFFSET, open operation is successful.

## 1.4 Read operation

You can read 10 bytes of data from the serial port by :

```c
int ret;
char read_buf[10];

ret = aos_read(fd_uart, read_buf, 10);
```

If return value is 10, you have successfully read the data.

## 1.5 Write operation

You can send 10 bytes of data to the serial port by :

```c
int ret;
char write_buf[10];

ret = aos_write(fd_uart, write_buf, 10);
```

If return value is 10, you have successfully sent the data.

## 1.6 IO operation

IO operation can be implemented through aos_ioctl. For example, in GPIO, you can output high and low level by :

```c
/* output high */
ret = aos_ioctl(fd_gpio, IOCTL_GPIO_OUTPUT_HIGHT, 0);

/* output low */
ret = aos_ioctl(fd_gpio, IOCTL_GPIO_OUTPUT_LOW, 0);
```

The second parameter is the IO operation you want. IO operation supported in various devices can refer to their corresponding header file in kernel/vfs/include/device/. For example, in vfs_gpio.h：

```c
/* cmd for ioctl */
#define IOCTL_GPIO_OUTPUT_HIGHT  1  /* output hight */
#define IOCTL_GPIO_OUTPUT_LOW    2  /* output low */
#define IOCTL_GPIO_OUTPUT_TOGGLE 3  /* toggle output */
```

## 1.7 Close the device

```c
ret = aos_close(fd_uart);
```

That the return value is 0 means the success of close. You need to reopen it if you need it again. 

## 1.8 Others

Data format for rtc device has been defined. You can refer to include/hal/soc/rtc.h, and we recommend you to read and set it as following : 

```c
rtc_time_t rtc_time_get;
rtc_time_t rtc_time_set;

/* get rtc */
ret = aos_read(fd_rtc, &rtc_time_get, sizeof(rtc_time_get));

/* set rtc */
ret = aos_write(fd_rtc, &rtc_time_set, sizeof(rtc_time_set));
```

# 2 File system
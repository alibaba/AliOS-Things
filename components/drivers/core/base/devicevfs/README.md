# 介绍
字符驱动核心框架
主要负责连接设备驱动和驱动核心框架，提供简化的API接口给驱动调用，对驱动隐藏驱动核心框架实现细节

# 特性
兼容三种VFS访问模式：
1. 驱动和使用驱动的应用位于用户态同一进程中 - 应用通过直接函数调用方式访问驱动服务
2. 驱动和使用驱动的应用位于用户态不同进程中 - 应用通过RPC方式访问驱动服务
3. 驱动位于内核，使用驱动的应用位于用户态   - 应用通过SYSCALL方式访问驱动服务
以上三种模式下，应用层是无感的，都是调用open/close/read/write/ioctl的接口进行设备节点的操作

对于RPC方式对外提供的服务使用“即用即走”的设计思想，open函数会触发设备驱动启动RPC服务，close函数触发驱动服务关闭RPC服务。不使用驱动服务的时候RPC服务不启动，以此来降低驱动对系统资源的占用。

驱动自动加载：设计了驱动启动级别，驱动程序所需要的初始化函数只需要用特定的宏定义进行声明到合适的启动级别，则系统(内核或APP)会自动读驱动进行加载

# 目录
```sh
main_driver/
├── aos.mk              #AliOS Things makefile文件
├── Config.in           #AliOS Things menuconfig配置文件
├── README.md
├── device_vfs_core.c   #字符设备核心驱动
├── u_driver_hub.c      #自动加载驱动功能
├── u_driver_main.c     #RPC主服务功能

```

# 依赖
- 操作系统系统基础组件


### 2. 声明驱动启动加载级别
驱动框架中会对驱动架构核心层及驱动子系统初始化进行集中统一管理，驱动程序中需要在代码中用预定义的宏定义对驱动初始化函数进行声明，驱动框架在启动的时候就会自动加载通过这个宏定义规定的驱动初始化函数。
以目前的设计会将驱动分为以下9个级别，其名称参考“类型名”栏位，加载顺序数字越低，加载时间越早。
编译驱动的lds文件中需要有对应section的描述。

|-|加载顺序|类型名|声明API|说明|模块列表|
|---|---|---|---|---|---|
|1|CORE_DRIVER|CORE_DRIVER_ENTRY|驱动框架核心模块|u_cdev, u_driver, u_bus|
|2|BUS_DRIVER|BUS_DRIVER_ENTRY/BUS_DRIVER_FULL_ENTRY|总线驱动模块|platform|
|3|EARLY_DRIVER|EARLY_DRIVER_ENTRY/EARLY_DRIVER_FULL_ENTRY|前置驱动模块|devicevfs|
|4|VFS_DRIVER|VFS_DRIVER_ENTRY/VFS_DRIVER_FULL_ENTRY|驱动子系统(提供VFS接口)|u_adc, u_dac, u_flash, u_gpio, u_uart, u_spi, u_i2c, u_pwm, u_timer, u_qspi, wifi|
|5|LEVEL0_DRIVER|LEVEL0_DRIVER_ENTRY/LEVEL0_DRIVER_FULL_ENTRY|级别0硬件驱动|-|
|6|LEVEL1_DRIVER|LEVEL1_DRIVER_ENTRY/LEVEL1_DRIVER_FULL_ENTRY|级别0硬件驱动|-|
|7|LEVEL2_DRIVER|LEVEL2_DRIVER_ENTRY/LEVEL2_DRIVER_FULL_ENTRY|级别0硬件驱动|-|
|8|LEVEL3_DRIVER|LEVEL3_DRIVER_ENTRY/LEVEL3_DRIVER_FULL_ENTRY|级别0硬件驱动|-|
|9|POST_DRIVER|POST_DRIVER_ENTRY/POST_DRIVER_FULL_ENTRY|驱动后初始化阶段|vfs_gpio_test, vfs_pwm_test, vfs_adc_test, vfs_uart_test, vfs_flash_test|

编译完成后，可以通过指令realelf -S <images's elf>查看这些section的size大小，增加新的驱动后对应section的size会增大

驱动lds文件section声明：
* 位置：./vendor/haas/board/<BOARD>/driver.ld.S
```
    .core_driver_entry (.):
    {
        __core_driver_start__ = .;
        KEEP(*(.core_driver_entry))
        __core_driver_end__ = .;
    } > RAM

    .bus_driver_entry (.):
    {
        __bus_driver_start__ = .;
        KEEP(*(*.bus_driver_entry))
        __bus_driver_end__ = .;
    } > RAM

    .early_driver_entry (.):
    {
        __early_driver_start__ = .;
        KEEP(*(*.early_driver_entry))
        __early_driver_end__ = .;
    } > RAM

    .vfs_driver_entry (.):
    {
        __vfs_driver_start__ = .;
        KEEP(*(*.vfs_driver_entry))
        __vfs_driver_end__ = .;
    } > RAM

    .level0_driver_entry (.):
    {
        __level0_driver_start__ = .;
        KEEP(*(*.level0_driver_entry))
        __level0_driver_end__ = .;
    } > RAM

    .level1_driver_entry (.):
    {
        __level1_driver_start__ = .;
        KEEP(*(*.level1_driver_entry))
        __level1_driver_end__ = .;
    } > RAM

    .level2_driver_entry (.):
    {
        __level2_driver_start__ = .;
        KEEP(*(*.level2_driver_entry))
        __level2_driver_end__ = .;
    } > RAM

    .level3_driver_entry (.):
    {
        __level3_driver_start__ = .;
        KEEP(*(*.level3_driver_entry))
        __level3_driver_end__ = .;
    } > RAM

    .post_driver_entry (.):
    {
        __post_driver_start__ = .;
        KEEP(*(*.post_driver_entry))
        __post_driver_end__ = .;
    } > RAM

    __kernel_driver_mode__ = 0;
```

### 3. 在目标应用中选择此驱动的编译选项
如果组件需要编译到kernel中，则在projects/alios_kernel/Config.in中选择此驱动的编译选项
如果组件需要编译到main_driver中，则在projects/main_driver/Config.in中选择此驱动的编译选项
如需通过VFS方式访问设备驱动，还需要选择此驱动对应的核心驱动，如v_gpio_drv对应的是u_gpio驱动

### 4. 功能验证


# 使用
对于驱动的使用者，可以参考components/drivers/test/xxxx/xxxx_test.c中访问设备驱动的方式来进行设备的操作，open/read/write/ioctl/close等功能和HAL层API的驱动接口定义的功能可以一一对应

## 编译指令
- 无

# 参考
- 无

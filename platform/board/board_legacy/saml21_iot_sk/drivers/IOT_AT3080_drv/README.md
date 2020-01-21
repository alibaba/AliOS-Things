# EMHOST SDK参考手册

Emhost SDK是运行在微控制器上，通过串口AT指令和MXCHIP出品的AT指令模组通讯的软件中间件，通过Emhost SDK和基于它开发的示例程序，开发者可以很方便地在自己的MCU上实现模组提供的通讯功能，连接到云服务快速实现应用功能。

EMHOST包含以下几个核心功能：

* 网络通讯和云服务应用框架
* AT指令处理系统
* 串口驱动程序

以及一些方便开发者参考的示例代码和文档

* 评估板设备驱动程序
* 典型应用示例程序

## 代码结构

1. 云通讯应用层协议，应用框架（后期持续增加应用）
   * AliCloud_SDS: 阿里云SDS服务应用框架
1. 评估版设备驱动（选用）
   * drv_button: 使用GPIO口实现的按键功能，可识别短按和长按
   * drv_color_led: 使用PWM来驱动的RGB三色LED
   * drv_sht20: 温湿度传感器驱动
   * drv_VGM128064：oled显示屏驱动
1. 支持软件包
   * emhost: AT指令处理系统，将模组支持的AT指令封装成API接口调用
   * jsmn: JSON数据包的构建和解析，用于和云服务进行交互
   * mx_utils: 一些常用的工具
   * hal: MCU外设驱动标准接口实现
     * mx_hal_atomic.c 进出临界区API，防止主程序和中断处理函数同时操作变量
     * mx_i2c.c: i2c接口驱动，现用于传感器和lcd
     * mx_serial.c: 串口驱动，要实现AT指令处理就必须实现的底层驱动
     * mx_stdio.c: 串口打印输出，用于调试信息的输出
     * mx_tick.c: 系统滴答基数，1毫秒间隔，用于超时时间处理
   * mx_common.h: 系统常量，宏定义等
   * mx_debug.h: 系统调试宏定义
   * mx_toolchain.h: 工具链相关的宏定义

代码的大致的调用关系如下：

```r
                application
                |          |
            drv_xxx     protocol(AliCloud_SDS)
                |          |              |
            mx_hal       emhost          jsmn
                         |    |
                  mx_serial   mx_tick
        ===========================================          
                  MCU Peripheral Lib
```

## API使用流程
应用程序可以直接调用emhost提供的API来通过AT指令控制模组，也可以调用基于emhost实现的云服务应用框架的API快速实现应用程序功能。
一般的使用流程是：

1. 首先调用初始化函数进行串口初始化，运行环境初始化
2. 然后调用API发送控制指令，等待模组返回命令处理结果后，返回API函数运行结果
3. 如果需要接收模组主动传回的数据和状态，需要周期地调用xxx_runloop函数，在函数中会对收到的数据进行处理，产生事件回调函数传递给上层。
   * 如果需要实现低功耗的应用，可以在有接收到串口数据后再调用runloop
   * 在进入休眠之前也需要调用一次runloop，因为有些runloop有将暂存数据发送的功能

==**需要注意的是，在emhost的事件处理回调函数中，绝对不能调用会直接发送AT指令的API函数，否则可能会引起系统锁死。**==

###通过emhost接口

emhost的接口函数在emh_api.h接口中定义

1. 使用`emh_init`函数进行初始化
2. 周期调用`emh_runloop`函数接收传入的事件和数据
3. 调用`emh_xxxx`API实现对模组的控制
4. emhost的事件回调函数在emh_event.h中定义

###通过AliCloud_SDS（阿里云SDS服务服务应用框架）

AliCloud_SDS的接口函数在alicloud_sds.h中定义，该应用框架通过调用emhost接口来实现功能

1. 使用`alisds_init`函数进行初始化，输入在云端注册的相关参数
2. 使用`alisds_attr_init`接口，初始化每一个SDS属性，设置属性的名称，类型和读写回调函数
2. 周期调用`alisds_runloop`函数接收传入的事件，发送准备好的数据
   * 当SDS服务需要读取设备数据时，会出发对应属性的读回调
   * 当SDS服务需要写入设备数据时，会出发对应属性的写回调
3. 如果需要主动向SDS发送数据，应调用`alisds_attr_indicate_by_handle`，它会触发`alisds_runloop`中的读属性回调
4. `alisds_event_handler`会向应用层报告SDS服务的工作状态

## 移植指南
底层硬件的驱动都在mx_hal目录下实现。

如果需要移植emhost SDK，通过AT指令控制MXCHIP模组，需要移植系统时钟`mx_tick`和串口交互`mx_serial`，严格按照SDK中定义的接口实现相应的功能。本SDK没有对其他的硬件接口做统一的定义，开发者可以根据自己使用的处理器和上层应用自由地开发。

注：mx_i2c仅用于评估板上的温湿度传感器和显示屏，如果不需要用，可以不用实现。评估版上的其他外设是直接调用MCU的驱动接口实现的，如果需要使用，请自行在不同平台上移植。这里不再赘述。

### 系统时钟的移植
系统时钟在mx_tick.c中实现，需要实现的函数如下：

* `mx_hal_ms_ticker_init`：系统时钟初始化
* `mx_hal_ms_ticker_read`：读取系统时钟，精度1毫秒

在示例中，使用Cortex-M内核的systick来实现系统时钟，具有一定的通用性。

### 串口通讯的移植
系统时钟在mx_serial.c中实现，串口接收功能的实现需要注意一定要采用中断接收，在中断处理函数中将接收到的数据存放在一块初始化的缓冲区中（参考mx_ringbuffer.h）,串口数据读取的API应读取缓冲区中的数据，而不是直接从串口外设读取，读取函数有超时时间，如果固定时间内没能从缓冲区中读取足够的数据，也会返回。
*关于串口接收缓冲区的使用，可以参考示例中的相关代码。*

串口通讯需要实现的函数如下：

* `mx_hal_serial_init`：串口初始化
* `mx_hal_serial_putc`：串口发送一个字节的数据
* `mx_hal_serial_getc`：串口从缓冲区读取一个字节的数据
* `mx_hal_serial_readable`：串口缓冲区中是否有数据
* `mx_hal_serial_flush`：清空串口接收缓冲区中的数据

### 调试信息的输出
系统通过printf将调式信息打印到串口终端上，mx_stdio.c中实现了调试串口的初始化，并且将系统的printf重定向到调试串口。










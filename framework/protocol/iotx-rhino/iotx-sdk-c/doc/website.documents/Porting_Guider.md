**以下描述如何将华东2节点设备端V2.0+版本C-SDK移植到目标硬件平台.**

## 详细技术文档请访问[官方WiKi](https://github.com/aliyun/iotkit-embedded/wiki)

#### IoT套件后续会逐渐增加已适配的平台, 如果您使用的平台未被适配, 请访问[官方Github](https://github.com/aliyun/iotkit-embedded/issues), 给我们提出Issue

## V2.0+设备端C-SDK概述

        +---------------------------+         +---------------------------+
        |                           |         |                           | =>  构建完成后产生:
        |  IoT SDK Example Program  |         | sample/mqtt|coap|ota/*.c  |
        |                           |         |                           |     output/release/bin/*-example
        +---------------------------+         +---------------------------+
        |                           |         |                           | =>  SDK提供功能的API, 都在这里声明
        |  IoT SDK Interface Layer  |         | src/sdk-impl/iot_export.h | =>  构建完成后产生:
        |                           |         |                           |
        |     IOT_XXX_YYY() APIs    |         |  Has all APIs' prototype  |     output/release/include/
        |                           |         |                           |         iot-sdk/iot_export.h
        |                           |         |                           |         iot-sdk/exports/*.h
        +---------------------------+         +---------------------------+
        |                           |         |                           | =>  SDK提供功能的API, 都在这里实现
        |                           |         | src/utils: utilities      | =>  构建完成后产生:
        |                           |  +--->  | src/log: logging          |
        |                           |         | src/tls: security         |
        |  IoT SDK Core Implements  |         | src/guider: authenticate  |     output/release/lib/
        |  : =>                     |  <---+  | src/system: device mgmt   |         libiot_sdk.a
        |  : You SHOULD NOT Focus   |         | src/mqtt: MQTT client     |
        |  : on this unless         |         | src/coap: CoAP client     |
        |  : you're debugging bugs  |         | src/http: HTTP client     |
        |                           |         | src/shadow: device shadow |
        |                           |         | src/ota: OTA channel      |
        |                           |         |                           |
        +---------------------------+         +---------------------------+
        |                           |         |                           | =>  SDK仅含有示例代码, 移植时需二次开发
        |  Hardware Abstract Layer  |         | src/sdk-impl/iot_import.h | =>  构建完成后产生:
        |                           |         | : =>                      |
        |     HAL_XXX_YYY() APIs    |         | : HAL_*() declarations    |     output/release/lib/
        |                           |         |                           |         libiot_platform.a
        |  : You MUST Implement     |         | src/platform/*/*/*.c      |     output/release/include/
        |  : this part for your     |         | : =>                      |         iot-sdk/iot_import.h
        |  : target device first    |         | : HAL_*() example impls   |         iot-sdk/imports/*.h
        +---------------------------+         +---------------------------+

2.0版本相对1.0.1版本在结构性方面, 升级了编译系统, 支持后续功能模块的灵活迭代和裁剪, 但是在代码架构方面, 和1.0.1版本保持恒定, 也是分为三层的, 如上图

* 最底层称为"硬件平台抽象层", 也简称`HAL层`, 对应上图的 `Hardware Abstract Layer`
    - 这里是抽象不同的嵌入式目标板上操作系统对我们SDK的支撑函数
    - 举例来说, 包括网络收发, TLS/DTLS通道建立和读写, 内存申请是否, 互斥量加锁解锁等
    - **注意: 在任何跨平台移植时, 实现这部分是需要完成的第一步工作**
    - **注意: 阿里的SDK里, 并不含有多平台的HAL层实现, 但我们提供了Linux桌面OS(Ubuntu16.04)上的参考实现, 移植时可以作为参考**

* 中间层称为"SDK内核实现层", 对应上图的 `IoT SDK Core Implements`
    - 这里是IoT套件C-SDK的核心实现部分, 它基于HAL层接口完成了MQTT/CoAP通道等的功能封装
    - 举例来说, 包括MQTT的连接建立, 报文收发, CoAP的连接建立, 报文收发, OTA的固件状态查询, OTA的固件下载等
    - **注意: 如果HAL层实现的好, 这一层在跨平台移植时, 理想情况不需要做任何修改**

* 最上层称为"SDK接口声明层", 对应上图的 `IoT SDK Interface Layer`
    - 这里没有实现, 只有一系列C函数的原型声明, 也就是SDK跨平台移植完成之后, 可以用于编写业务逻辑, 和阿里云服务器通信的API
    - 举例来说, 怎么去使用这些API做业务逻辑, 我们在`sample`目录提供了丰富的示例程序, 并且只要填入了设备信息, 就可以在Linux主机上运行体验

以下按照从下到上的顺序, 逐个对每个层次做更加详细的说明

## 硬件平台抽象层(HAL层)

* 所有HAL层函数的声明都在 `src/sdk-impl/iot_import.h` 这个头文件中列出
* 各功能点引入的HAL层接口依赖在`src/sdk-impl/imports/iot_import_*.h`中列出
* 这些`imports`目录下的子文件, 都被`src/sdk-impl/iot_import.h`包含
* 在V2.0+版本的编译系统中, 这个部分会被编译成`output/release/lib/libiot_platform.a`

以下可以列出所有跨平台移植时需要实现的HAL层接口

    src/sdk-impl$ grep -ro "HAL_[_A-Za-z0-9]*" *|cut -d':' -f2|sort -u|cat -n

     1  HAL_DTLSSession_create
     2  HAL_DTLSSession_free
     3  HAL_DTLSSession_read
     4  HAL_DTLSSession_write
     5  HAL_Free
     6  HAL_GetModuleID
     7  HAL_GetPartnerID
     8  HAL_Malloc
     9  HAL_MutexCreate
    10  HAL_MutexDestroy
    11  HAL_MutexLock
    12  HAL_MutexUnlock
    13  HAL_Printf
    14  HAL_Random
    15  HAL_SleepMs
    16  HAL_Snprintf
    17  HAL_Srandom
    18  HAL_SSL_Destroy
    19  HAL_SSL_Establish
    20  HAL_SSL_Read
    21  HAL_SSL_Write
    22  HAL_TCP_Destroy
    23  HAL_TCP_Establish
    24  HAL_TCP_Read
    25  HAL_TCP_Write
    26  HAL_UDP_close
    27  HAL_UDP_create
    28  HAL_UDP_read
    29  HAL_UDP_readTimeout
    30  HAL_UDP_write
    31  HAL_UptimeMs
    32  HAL_Vsnprintf

对这些函数做实现的时候, 可以参考`src/platform`下已经写好的示例, 这些示例在`Ubuntu16.04`主机和`Win7`主机上被完善的编写和测试过

    src/platform$ tree
    .
    +-- iot.mk
    +-- os
    |   +-- linux
    |   |   +-- HAL_OS_linux.c
    |   |   +-- HAL_TCP_linux.c
    |   |   +-- HAL_UDP_linux.c
    |   +-- ubuntu -> linux
    |   +-- win7
    |       +-- HAL_OS_win7.c
    |       +-- HAL_TCP_win7.c
    +-- ssl
        +-- mbedtls
        |   +-- HAL_DTLS_mbedtls.c
        |   +-- HAL_TLS_mbedtls.c
        +-- openssl
            +-- HAL_TLS_openssl.c

以下是这些函数的一个说明表格, 更多详细信息, 请查阅代码中的注释, 或关注[官方WiKi](https://github.com/aliyun/iotkit-embedded/wiki)

| 函数名                   | 说明                                                                    |
|--------------------------|-------------------------------------------------------------------------|
| HAL_DTLSSession_create   | 初始化DTLS资源并建立一个DTLS会话, 用于CoAP功能                          |
| HAL_DTLSSession_free     | 销毁一个DTLS会话并释放DTLS资源, 用于CoAP功能                            |
| HAL_DTLSSession_read     | 从DTLS会话中读数据, 用于CoAP功能                                        |
| HAL_DTLSSession_write    | 向DTLS会话中写数据, 用于CoAP功能                                        |
| HAL_Free                 | 释放一片堆上内存                                                        |
| HAL_GetModuleID          | 用于紧密合作伙伴, 一般客户只需要在此可实现为空函数                      |
| HAL_GetPartnerID         | 用于紧密合作伙伴, 一般客户只需要在此可实现为空函数                      |
| HAL_Malloc               | 申请一片堆上内存                                                        |
| HAL_MutexCreate          | 创建一个互斥量, 用于同步控制, 目前SDK仅支持单线程应用, 可实现为空函数   |
| HAL_MutexDestroy         | 销毁一个互斥量, 用于同步控制, 目前SDK仅支持单线程应用, 可实现为空函数   |
| HAL_MutexLock            | 加锁一个互斥量, 用于同步控制, 目前SDK仅支持单线程应用, 可实现为空函数   |
| HAL_MutexUnlock          | 解锁一个互斥量, 用于同步控制, 目前SDK仅支持单线程应用, 可实现为空函数   |
| HAL_Printf               | 打印函数, 用于向串口或其它标准输出打印日志或调试信息                    |
| HAL_Random               | 随机数函数, 接受一个无符号数作为范围, 返回0到该数值范围内的随机无符号数 |
| HAL_SleepMs              | 睡眠函数, 使当前执行线程睡眠指定的毫秒数                                |
| HAL_Snprintf             | 打印函数, 向内存缓冲区格式化构建一个字符串, 参考C99标准库函数`snprintf` |
| HAL_Srandom              | 随机数播种函数, 使`HAL_Random`的返回值每个执行序列各不相同, 类似`srand` |
| HAL_SSL_Destroy          | 销毁一个TLS连接, 用于MQTT功能, HTTPS功能                                |
| HAL_SSL_Establish        | 建立一个TLS连接, 用于MQTT功能, HTTPS功能                                |
| HAL_SSL_Read             | 从一个TLS连接中读数据, 用于MQTT功能, HTTPS功能                          |
| HAL_SSL_Write            | 向一个TLS连接中写数据, 用于MQTT功能, HTTPS功能                          |
| HAL_TCP_Destroy          | 销毁一个TLS连接, 用于MQTT功能, HTTPS功能                                |
| HAL_TCP_Establish        | 建立一个TCP连接, 包含了域名解析的动作和TCP连接的建立                    |
| HAL_TCP_Read             | 在指定时间内, 从TCP连接读取流数据, 并返回读到的字节数                   |
| HAL_TCP_Write            | 在指定时间内, 向TCP连接发送流数据, 并返回发送的字节数                   |
| HAL_UDP_close            | 关闭一个UDP socket                                                      |
| HAL_UDP_create           | 创建一个UDP socket                                                      |
| HAL_UDP_read             | 阻塞的从一个UDP socket中读取数据包, 并返回读到的字节数                  |
| HAL_UDP_readTimeout      | 在指定时间内, 从一个UDP socket中读取数据包, 返回读到的字节数            |
| HAL_UDP_write            | 阻塞的向一个UDP socket中发送数据包, 并返回发送的字节数                  |
| HAL_UptimeMs             | 时钟函数, 获取本设备从加电以来到目前时间点已经过去的毫秒数              |
| HAL_Vsnprintf            | 字符串打印函数, 将`va_list`类型的变量, 打印到指定目标字符串             |

在这些HAL接口中

**必选实现**

| 函数名                   | 说明                                                                    |
|--------------------------|-------------------------------------------------------------------------|
| HAL_Malloc               | 申请一片堆上内存                                                        |
| HAL_Free                 | 释放一片堆上内存                                                        |
| HAL_SleepMs              | 睡眠函数, 使当前执行线程睡眠指定的毫秒数                                |
| HAL_Snprintf             | 打印函数, 向内存缓冲区格式化构建一个字符串, 参考C99标准库函数`snprintf` |
| HAL_Printf               | 打印函数, 用于向串口或其它标准输出打印日志或调试信息                    |
| HAL_Vsnprintf            | 字符串打印函数, 将`va_list`类型的变量, 打印到指定目标字符串             |
| HAL_UptimeMs             | 时钟函数, 获取本设备从加电以来到目前时间点已经过去的毫秒数              |


**可实现为空**

| 函数名                   | 说明                                                                    |
|--------------------------|-------------------------------------------------------------------------|
| HAL_GetPartnerID         | 用于紧密合作伙伴, 一般客户只需要在此可实现为空函数                      |
| HAL_GetModuleID          | 用于紧密合作伙伴, 一般客户只需要在此可实现为空函数                      |
| HAL_MutexCreate          | 创建一个互斥量, 用于同步控制, 目前SDK仅支持单线程应用, 可实现为空函数   |
| HAL_MutexDestroy         | 销毁一个互斥量, 用于同步控制, 目前SDK仅支持单线程应用, 可实现为空函数   |
| HAL_MutexLock            | 加锁一个互斥量, 用于同步控制, 目前SDK仅支持单线程应用, 可实现为空函数   |
| HAL_MutexUnlock          | 解锁一个互斥量, 用于同步控制, 目前SDK仅支持单线程应用, 可实现为空函数   |

**没有MQTT时可实现为空**

| 函数名                   | 说明                                                                    |
|--------------------------|-------------------------------------------------------------------------|
| HAL_SSL_Destroy          | 销毁一个TLS连接, 用于MQTT功能, HTTPS功能                                |
| HAL_SSL_Establish        | 建立一个TLS连接, 用于MQTT功能, HTTPS功能                                |
| HAL_SSL_Read             | 从一个TLS连接中读数据, 用于MQTT功能, HTTPS功能                          |
| HAL_SSL_Write            | 向一个TLS连接中写数据, 用于MQTT功能, HTTPS功能                          |
| HAL_TCP_Destroy          | 销毁一个TLS连接, 用于MQTT功能, HTTPS功能                                |
| HAL_TCP_Establish        | 建立一个TCP连接, 包含了域名解析的动作和TCP连接的建立                    |
| HAL_TCP_Read             | 在指定时间内, 从TCP连接读取流数据, 并返回读到的字节数                   |
| HAL_TCP_Write            | 在指定时间内, 向TCP连接发送流数据, 并返回发送的字节数                   |
| HAL_Random               | 随机数函数, 接受一个无符号数作为范围, 返回0到该数值范围内的随机无符号数 |
| HAL_Srandom              | 随机数播种函数, 使`HAL_Random`的返回值每个执行序列各不相同, 类似`srand` |

**没有CoAP时可实现为空**

| 函数名                   | 说明                                                                    |
|--------------------------|-------------------------------------------------------------------------|
| HAL_DTLSSession_create   | 初始化DTLS资源并建立一个DTLS会话, 用于CoAP功能                          |
| HAL_DTLSSession_free     | 销毁一个DTLS会话并释放DTLS资源, 用于CoAP功能                            |
| HAL_DTLSSession_read     | 从DTLS连接中读数据, 用于CoAP功能                                        |
| HAL_DTLSSession_write    | 向DTLS连接中写数据, 用于CoAP功能                                        |

**没有ID2时可实现为空**

| 函数名                   | 说明                                                                    |
|--------------------------|-------------------------------------------------------------------------|
| HAL_UDP_close            | 关闭一个UDP socket                                                      |
| HAL_UDP_create           | 创建一个UDP socket                                                      |
| HAL_UDP_read             | 阻塞的从一个UDP socket中读取数据包, 并返回读到的字节数                  |
| HAL_UDP_readTimeout      | 在指定时间内, 从一个UDP socket中读取数据包, 返回读到的字节数            |
| HAL_UDP_write            | 阻塞的向一个UDP socket中发送数据包, 并返回发送的字节数                  |

## SDK内核实现层

* 所有被提供的函数的声明都在 `src/sdk-impl/iot_export.h` 这个头文件中列出
* 各功能点提供的接口在`src/sdk-impl/exports/iot_export_*.h`中列出
* 这些`exports`目录下的子文件, 都被`src/sdk-impl/iot_export.h`包含
* 在V2.0+版本的编译系统中, 这个部分会被编译成`output/release/lib/libiot_sdk.a`

## SDK接口声明层 + 例程

请参考[快速接入页面](https://help.aliyun.com/document_detail/30530.html)和[官方SDK首页](https://github.com/aliyun/iotkit-embedded)

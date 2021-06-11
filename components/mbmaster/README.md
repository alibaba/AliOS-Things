@page mbmaster mbmaster

**[更正文档](https://gitee.com/alios-things/mbmaster/edit/master/README.md)** &emsp;&emsp;&emsp;&emsp; **[贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)**

# 概述

AliOS Things Modbus是标准的串行通信协议驱动，提供RTU模式能力，暂不支持ASCII/TCP模式。开发可以通过组件API，在设备端快速对接支持Modbus协议器件的进行读写。

组件支持以下功能：

- 支持基于RS232/485串口的RTU Modbus
- 提供完整的读写寄存器接口（共8个api），一一对应支持如下功能码：

  1. 0x01: 读线圈寄存器 mbmaster_read_coils
  2. 0x02: 读离散输入寄存器 mbmaster_read_discrete_inputs
  3. 0x03: 读保持寄存器 mbmaster_read_holding_registers
  4. 0x04: 读输入寄存器 mbmaster_read_input_registers
  5. 0x05: 写单个线圈寄存器 mbmaster_write_single_coil
  6. 0x06: 写单个保持寄存器 mbmaster_write_single_register
  7. 0x0f: 写多个线圈寄存器 mbmaster_write_multiple_coils
  8. 0x10: 写多个保持寄存器 mbmaster_write_multiple_registers

## 版权信息

> Apache license v2.0

## 目录结构

```tree
src
├── adu
│   ├── mbcrc.c                            # crc16校验位计算
│   ├── mbcrc.h                            # crc16校验位计算头文件
│   ├── rtu.c                              # 编译数据
│   └── rtu.h                              # 编译数据头文件
├── api
│   ├── main_process.c                     # 选择加解密类型
│   ├── main_process.h                     # 选择加解密型头文件
│   ├── mbm.c                              # 驱动初始化
│   └── mbmaster_api.c                     # 驱动对外读写接口
├── auxiliary
│   ├── log.c                              # 打印信息相关
│   └── other.c                            # 锁相关
├── include
│   ├── adu.h                              # 发送数据校验和编译相关头文件
│   ├── auxiliary.h                        # 打印及锁相关头文件
│   ├── mbmaster_default_config.h          # modbus配置头文件，包括模式使能
│   └── mbmaster.h                         # 对外配置和接口头文件
├── pdu
│   ├── pdu.c                              # 加解密类型接口
│   └── pdu.h                              # 加解密类型接口头文件
├── physical
│   ├── serial.c                           # 依赖的串口驱动
│   └── serial.h                           # 依赖的串口头文件
├── example
│   └── mbmaster_example.c                 # modbus示例代码
└── package.yaml                           # 编译配置文件
```

## 依赖组件

- uart
- vfs
- ulog
- posix

# 常用配置

无

# API说明

**常用API**
- mbmaster_read_coils
- mbmaster_read_discrete_inputs
- mbmaster_read_holding_registers
- mbmaster_read_input_registers

- mbmaster_write_single_coil
- mbmaster_write_single_register
- mbmaster_write_multiple_coils
- mbmaster_write_multiple_registers

参考[完整API手册](https://g.alicdn.com/alios-things-3.3/doc/group__aos__mbmaster.html)

# 使用示例

组件使用示例相关的代码下载、编译和固件烧录均依赖AliOS Things配套的开发工具 **alios-studio** ，所以首先需要参考[《aos-studio使用说明之搭建开发环境》](https://g.alicdn.com/alios-things-3.3/doc/setup_env.html)，下载安装 **alios-studio** 。
待开发环境搭建完成后，可以按照以下步骤进行示例的测试。

## 步骤1 创建或打开工程

**打开已有工程**

如果用于测试的案例工程已存在，可参考[《aos-studio使用说明之打开工程》](https://g.alicdn.com/alios-things-3.3/doc/open_project.html)打开已有工程。

**创建新的工程**

组件的示例代码可以通过编译链接到AliOS Things的任意案例（solution）来运行，这里选择helloworld_demo案例。helloworld_demo案例相关的源代码下载可参考[《aos-studio使用说明之创建工程》](https://g.alicdn.com/alios-things-3.3/doc/create_project.html)。

## 步骤2 添加组件

案例下载完成后，需要在helloworld_demo组件的package.yaml中添加对组件的依赖：

```yaml
depends:
  - mbmaster: master # helloworld_demo中引入modbus组件
  - ulog: master
```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell
aos install mbmaster
```

上述命令执行成功后，组件源码则被下载到了./components/mbmaster路径中。

## 步骤4 添加示例

在mbmaster组件的package.yaml中添加[example示例代码](https://gitee.com/alios-things/mbmaster/tree/master/example)：

```yaml
source_file:
  - "src/*.c"
  - "example/mbmaster_example.c" # add mbmaster_example.c
```

## 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

## 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

## 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

## 步骤8 测试示例

**CLI命令行输入：**

```sh
mbmaster_example
```

> 关键日志：

```sh
mbmaster test                                        # 测试开始
write single register ok                             # 写单个保持寄存器成功
read holding register simulator1: xx,simulator2: xx  # 读保持寄存器成功
```

# FAQ

暂无

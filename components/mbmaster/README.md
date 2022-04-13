@page modbus modbus


# 概述

AliOS Things Modbus是标准的串行通信协议驱动，提供RTU模式能力，暂不支持ASCII/TCP模式。开发可以通过组件API，在设备端快速对接支持Modbus协议器件的进行读写。

组件支持以下功能：

- 支持基于RS232/485串口的RTU Modbus
- 提供完整的读写寄存器接口（共8个api），一一对应支持如下功能码：

  0x01: 读线圈寄存器 mbmaster_read_coils
  0x02: 读离散输入寄存器 mbmaster_read_discrete_inputs
  0x03: 读保持寄存器 mbmaster_read_holding_registers
  0x04: 读输入寄存器 mbmaster_read_input_registers
  0x05: 写单个线圈寄存器 mbmaster_write_single_coil
  0x06: 写单个保持寄存器 mbmaster_write_single_register
  0x0f: 写多个线圈寄存器 mbmaster_write_multiple_coils
  0x10: 写多个保持寄存器 mbmaster_write_multiple_registers

## 版权信息

> Apache license v2.0

## 目录结构

```javascript
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
│   └── modbus_example.c                   # modbus示例代码
└── package.yaml                           # 编译配置文件
```


## 依赖组件

- uart
- vfs
- ulog
- posix

# 常用配置


# API说明

@ref mbmaster_read_coils
@ref mbmaster_read_discrete_inputs
@ref mbmaster_read_holding_registers
@ref mbmaster_read_input_registers


@ref mbmaster_write_single_coil
@ref mbmaster_write_single_register
@ref mbmaster_write_multiple_coils
@ref mbmaster_write_multiple_registers

# 使用示例

示例代码参考example/modbus_example.c，以运行helloworld_demo为例，具体步骤如下：

## 添加示例代码

> modbus组件的package.yaml中添加example示例代码

```javascript
source_file:
  - "src/*.c"
  - "example/modbus_example.c" # add modbus_example.c
```

## app中添加modbus组件

> helloworld_demo组件的package.yaml中添加

```javascript
depends:
  - haas100: master
  - mbmaster: master # helloworld_demo中引入modbus组件
  - ulog: master
```


## 编译


```sh
cd solutions/helloworld_demo && aos make
```

## 烧录固件

> 参考具体板子的快速开始文档。

## Modbus示例测试

# FAQ

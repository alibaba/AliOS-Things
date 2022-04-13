@page i2c i2c

# 概述
> I2C VFS驱动子系统，该驱动的目的是给应用或组件提供通过VFS形式（open/close/ioctl）访问I2C控制器驱动对上层提供服务的接口。
> 该组件初始化过程中，会根据hardware/chip/<chip_name>/package.yaml中定义的CONFIG_I2C_NUM来依此创建如下名称的设备节点：

* /dev/i2c0
* /dev/i2c1
* ...
* /dev/i2c[CONFIG_I2C_NUM - 1]

> I2C VFS驱动子系统内部会将open/close/ioctl请求转换成对应硬件的HAL层接口调用，通过HAL层接口控制主控芯片对I2C控制器的参数（从设备地址、CLK等）的设定或发起I2C 读或写操作请求。

## 版权信息
> Apache license v2.0

## 目录结构
```sh
│── src
│   └── i2c_dev.c      # I2C VFS驱动程序源代码
├── example            # I2C VFS驱动程序使用案例
│   └── i2c_example.c  # 通过I2C VFS接口进行I2C通信的案例文件
├── include            # I2C驱动子系统对外提供头文件目录
│   ├── aos
│   │   └── hal
│   │       └── i2c.h  # AOS HAL API头文件声明，本文件中的API是给厂商对接HAL API的原型声明
│   └── vfsdev
│       └── i2c_dev.h  # I2C VFS驱动子系统对应用/组件提供的接口声明文件
└── package.yaml       # I2C VFS驱动子系统的组件配置文件

```

## 依赖组件
* base     # 最底层核心驱动模型、驱动自动加载机制以及Device VFS core
* vfs      # VFS API抽象库
* epoll    # 增强型多路复用IO接口 - epoll机制所在库

# 常用配置
> 本组件可以通过CONFIG_I2C_NUM配置对上层提供设备节点的数量，CONFIG_I2C_NUM代表芯片内部的I2C控制器的数量。
> CONFIG_I2C_NUM默认是在hardware/chip/<chip_name>/package.yaml中的**def_config**节点进行配置。
> 如果没有定义CONFIG_I2C_NUM，则代码中会定义默认对外输出的设备节点数量。
> 设备节点数量: 默认4个, 如需修改，在hardware/chip/<chip_name>/package.yaml中修改CONFIG_I2C_NUM配置
```sh
def_config:
  CONFIG_I2C_NUM: 2
```

# API说明
@ref i2c_device_api

# 使用示例
> 示例代码参考example/i2c_example.c，以运行helloworld_demo为例，具体步骤如下：

## 添加示例代码
> i2c组件的package.yaml中添加example
```sh
source_file:

I2C device driver
#i2c VFS driver
  - src/i2c_dev.c ? <CONFIG_U_I2C_DEV>
#i2c VFS driver example
- example/i2c_example.c ? <CONFIG_U_I2C_DEV>
```

## 添加I2C组件
> 如果芯片内部含有I2C控制器，则芯片厂在操作系统对接的时候已经将I2C组件添加在了芯片级别配置文件“hardware/chip/<chip_name>/package.yaml”中，此配置主要包含“设置组件依赖关系”及“设置芯片内部所含I2C控制器数量”两个配置。
```sh
# 设置组件依赖关系
depends:
  - i2c: dev_aos

# 设置芯片内部所含I2C控制器数量
def_config:
  CONFIG_I2C_NUM: 2
```

## 编译
> 编译helloworld_demo中默认定义的单板
```sh
cd solutions/helloworld_demo && aos make
```

> 如果需要编译非helloworld_demo中默认定义的单板（比如编译haaseduk1可运行的helloworld_demo），需要使用如下指令：
```sh
cd solutions/helloworld_demo && aos make helloworld_demo@haaseduk1 -c config && aos make
```

## 烧录固件
> 参考具体板子的快速开始文档。

## I2C示例测试
> CLI命令行输入：
```sh
# 向挂载在I2C通道1上面的SI7006温湿度传感器（从设备地址）发送一个0xF5的指令，控制其量测温度
i2c_write 1 64 245 1
```

## 关键日志
> CLI日志：
```sh
i2c comp write test success!
```

> CLI命令行输入：
```sh
# 向挂载在I2C通道1上面的SI7006温湿度传感器（从设备地址）发送读2个字节数据的指令
i2c_read 1 64 2
```

## 关键日志
> CLI日志：
```sh
i2c comp read test success!
```

# 注意事项
> I2C是总线类型的bus，进行i2c组件读写测试，必须确保i2c总线上面有外挂i2c 从设备，并且i2c控制器id、i2c从设备地址、写入数据及长度信息都正确才能得到正确的执行结果；以上任一信息出错，执行i2c_read/i2c_write 都可能得到错误的执行结果。

# FAQ
> I2C总线协议的介绍可参考视频学习：https://www.bilibili.com/video/BV1zb4y197h6/


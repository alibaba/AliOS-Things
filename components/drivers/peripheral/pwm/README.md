@page pwm pwm

# 概述
> PWM VFS驱动子系统，该驱动的目的是给应用或组件提供通过VFS形式（open/close/ioctl）访问PWM控制器驱动对上层提供服务的接口。
> 该组件初始化过程中，会根据hardware/chip/<chip_name>/package.yaml中定义的CONFIG_PWM_NUM来依此创建如下名称的设备节点：

* /dev/pwm0
* /dev/pwm1
* ...
* /dev/pwm[CONFIG_PWM_NUM - 1]

> PWM VFS驱动子系统内部会将open/close/ioctl请求转换成对应硬件的HAL层接口调用，通过HAL层接口控制主控芯片的PWM控制器的参数（频率、占空比等）的设定或发起PWM输出波形请求。

## 版权信息
> Apache license v2.0

## 目录结构
```sh
│── src
│   └── pwm_dev.c      # PWM VFS驱动程序源代码
├── example            # PWM VFS驱动程序使用案例
│   └── pwm_example.c  # 通过PWM VFS接口进行PWM通信的案例文件
├── include            # PWM驱动子系统对外提供头文件目录
│   ├── aos
│   │   └── hal
│   │       └── pwm.h  # AOS HAL API头文件声明，本文件中的API是给厂商对接HAL API的原型声明
│   └── vfsdev
│       └── pwm_dev.h  # PWM VFS驱动子系统对应用/组件提供的接口声明文件
└── package.yaml       # PWM VFS驱动子系统的组件配置文件

```

## 依赖组件
* base     # 最底层核心驱动模型、驱动自动加载机制以及Device VFS core
* vfs      # VFS API抽象库

# 常用配置
> 本组件可以通过CONFIG_PWM_NUM配置对上层提供设备节点的数量，CONFIG_PWM_NUM代表芯片内部的PWM控制器的数量。
> CONFIG_PWM_NUM默认是在hardware/chip/<chip_name>/package.yaml中的**def_config**节点进行配置。
> 如果没有定义CONFIG_PWM_NUM，则代码中会定义默认对外输出的设备节点数量。
> 设备节点数量: 默认4个, 如需修改，在hardware/chip/<chip_name>/package.yaml中修改CONFIG_PWM_NUM配置
```sh
def_config:
  CONFIG_PWM_NUM: 2
```

# API说明
@ref pwm_device_api

# 使用示例
> 示例代码参考example/pwm_example.c，以运行helloworld_demo为例，具体步骤如下：

## 添加示例代码
> pwm组件的package.yaml中添加example
```sh
source_file:

PWM device driver
#PWM VFS driver
  - src/pwm_dev.c ? <CONFIG_U_PWM_DEV>
#PWM VFS driver example
- example/pwm_example.c ? <CONFIG_U_PWM_DEV>
```

## 添加PWM组件
> 如果芯片内部含有PWM控制器，则芯片厂在操作系统对接的时候已经将PWM组件添加在了芯片级别配置文件“hardware/chip/<chip_name>/package.yaml”中，此配置主要包含“设置组件依赖关系”及“设置芯片内部所含PWM控制器数量”两个配置。
```sh
# 设置组件依赖关系
depends:
  - pwm: dev_aos

# 设置芯片内部所含PWM控制器数量
def_config:
  CONFIG_PWM_NUM: 2
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

## PWM示例测试
> CLI命令行输入：
```sh
pwm_output <channel id> <freqency> <period, in unit of s>
# 向PWM通道0输出频率为100，占空比为0.5(pwm_example.c中固定参数)的波形，持续时间3秒
pwm_output 0 100 3
```

## 关键日志
> CLI日志：
```sh
pwm comp output test success!
```

# 注意事项
> PWM是总线类型的bus，进行pwm组件输出测试，测试结果可以通过示波器进行量测或通过外拐有声光效果的元器件（比如LED，蜂鸣器）观察不同的声光效果。

# FAQ
> 无


@page adc adc

# 概述
ADC（Analog-to-Digital Converter），即模拟数字转换器，用于将模拟形式的连续信号转换为数字形式的离散信号的一类设备。典型的ADC设备将模拟信号转换为表示一定比例电压值的数字信号。
该组件是ADC VFS驱动子系统，给应用或组件提供通过VFS形式（open/close/ioctl）访问ADC控制器驱动对上层提供服务的接口。该组件初始化过程中，会根据hardware/chip/<chip_name>/package.yaml中定义的CONFIG_ADC_NUM来依此创建如下名称的设备节点：
* /dev/adc0
* /dev/adc1
* ...
* /dev/adc[CONFIG_ADC_NUM - 1]

组件支持以下功能：
- 从ADC设备读取数据

## 版权信息
> Apache license v2.0

## 目录结构
```sh
adc/
|-- example
|   |-- adc_example.c   #ADC示例代码
|-- include
|   |-- aos
|   |   |-- hal
|   |       |-- adc.h   #ADC HAL API申明
|   |-- vfsdev
|       |-- adc_dev.h   #ADC设备API申明
|-- package.yaml        #编译和配置文件
|-- README.md           #使用文档
|-- src
    |-- adc_dev.c      #ADC设备实现代码
```

## 依赖组件
* base     # 最底层核心驱动模型、驱动自动加载机制以及Device VFS core
* vfs      # VFS API抽象库

# 常用配置
本组件可以通过CONFIG_ADC_NUM配置对上层提供设备节点的数量，CONFIG_ADC_NUM代表芯片内部的ADC控制器的数量。
CONFIG_ADC_NUM默认是在hardware/chip/<chip_name>/package.yaml中的**def_config**节点进行配置。
如果没有定义CONFIG_ADC_NUM，则代码中会定义默认对外输出的设备节点数量。
> 设备节点数量: 默认4个, 如需修改，在hardware/chip/<chip_name>/package.yaml中修改CONFIG_ADC_NUM配置
```sh
def_config:
  CONFIG_ADC_NUM: 2
```

# API说明
@ref adc_device_api

# 使用示例
示例代码参考example/adc_example.c，以运行helloworld_demo为例，具体步骤如下：

## 添加示例代码
> adc组件的package.yaml中添加example
```sh
source_file:
  原有其他文件
  - example/adc_example.c # add adc_example.c
```

## 添加adc组件
> helloworld_demo组件的package.yaml中添加
```sh
depends:
  - adc: master # helloworld_demo中引入adc组件
```

## 编译
```sh
cd solutions/helloworld_demo && aos make
```
其中具体单板还需要先配置环境：
```sh
aos make helloworld_demo@haas100 -c config
```

## 烧录固件
> 参考具体板子的快速开始文档。

## KV示例测试
> CLI命令行输入：
```sh
adc_example
```

## 关键日志
> CLI日志：
```sh
vfs adc test success, ret:0
```

# 注意事项
无

# FAQ
无

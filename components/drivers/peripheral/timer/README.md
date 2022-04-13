@page timer timer

# 概述
Timer，即通用硬件定时器。
该组件是Timer VFS驱动子系统，给应用或组件提供通过VFS形式（open/close/ioctl）访问硬件Timer驱动对上层提供服务的接口。该组件初始化过程中，会根据hardware/chip/<chip_name>/package.yaml中定义的CONFIG_TIMER_NUM来依此创建如下名称的设备节点：
* /dev/timer0
* /dev/timer1
* ...
* /dev/timer[CONFIG_TIMER_NUM - 1]

组件支持以下功能：
- 设置定时器运行周期
- 设置中断回调函数
- 启动/停止定时器

## 版权信息
> Apache license v2.0

## 目录结构
```sh
.
├── example
│   └── timer_example.c  #Timer示例代码
├── include
│   ├── aos
│   │   └── hal
│   │       └── timer.h   #Timer HAL API申明
│   └── vfsdev
│       └── timer_dev.h   #Timer设备API申明
├── package.yaml          #编译和配置文件
├── README.md             #使用文档
└── src
    └── timer_dev.c       #Timer设备实现代码
```

## 依赖组件
* base     # 最底层核心驱动模型、驱动自动加载机制以及Device VFS core
* vfs      # VFS API抽象库

# 常用配置
本组件可以通过CONFIG_TIMER_NUM配置对上层提供设备节点的数量，CONFIG_TIMER_NUM代表芯片内部的TIMER控制器的数量。
CONFIG_TIMER_NUM默认是在hardware/chip/<chip_name>/package.yaml中的**def_config**节点进行配置。
如果没有定义CONFIG_TIMER_NUM，则代码中会定义默认对外输出的设备节点数量。
> 设备节点数量: 默认4个, 如需修改，在hardware/chip/<chip_name>/package.yaml中修改CONFIG_TIMER_NUM配置
```sh
def_config:
  CONFIG_TIMER_NUM: 1
```

# API说明
@ref timer_device_api

# 使用示例
示例代码参考example/timer_example.c，以运行helloworld_demo为例，具体步骤如下：

## 添加示例代码
> timer组件的package.yaml中添加example
```sh
source_file:
  原有其他文件
  - example/timer_example.c # add timer_example.c
```

## 添加timer组件
> helloworld_demo组件的package.yaml中添加
```sh
depends:
  - timer: master # helloworld_demo中引入timer组件
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

## Timer示例测试
> CLI命令行输入：
```sh
timer_example id
```
其中id为0-CONFIG_TIMER_NUM之间的数值。

## 关键日志
> CLI日志：
```sh
vfs timer test success, ret:0
```

# 注意事项
无

# FAQ
无

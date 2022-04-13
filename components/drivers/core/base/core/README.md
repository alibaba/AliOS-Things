# 介绍
驱动核心框架: 基于Bus-Device-Driver的核心设备驱动模型

# 特性
面向对象的设计理念：核心设备驱动模型采用Bus-Device-Driver分离的架构，用C语言结合面向对象的设计理念进行设计和实现。

# 目录
```sh
core/
├── aos.mk          #AliOS Things makefile文件
├── Config.in       #AliOS Things menuconfig配置文件
├── README.md
├── u_bus.c         #总线抽象层
├── u_bus.c         #暂未使用
├── u_device.c      #设备抽象层
├── u_driver.c      #驱动抽象层
├── u_ld.c          #驱动初始化模块
└── u_interrupt.c   #暂未使用
```

# 依赖
- 操作系统系统基础组件

# 使用
厂商无需关注此模块的实现细节
对外提供ubusdump调试指令dump系统中的总线/设备/驱动信息及其连接关系
AOS_COMP_UDRIVER会被devicevfs模块选择使用

## 编译指令
无需单独编译

# 参考
无

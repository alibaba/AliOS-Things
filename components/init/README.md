@page init init

# 概述
init是AliOS Things的基础模块初始化接口。其主要实现了**aos_components_init**接口。内部包含了Kernel中间件相关的基础组件的初始化。**aos_components_init**默认在各个app中作为基础接口在进入application_start前被调用。
该组件实现了以下功能：
- 基础中间件的初始化
- cli以及基础模块cli命令的注册：cli_service_init
- **Welcome to AliOS Things** 串口欢迎打印字样

其初始化的模块包括：
- VFS： vfs_init
- uagent: uagent_init
- cli： cli_service_init
- driver框架: u_driver_entry
- NFTL nand适配层： nftl_init
- fatfs: fatfs_register
- ulog： ulog_init
- kv: kv_init
- sal: sal_device_init
- mal: mal_device_init
- yloop: aos_loop_init
- ota: ota_service_init
- sensor框架： sensor_init
- pwrmgmt低功耗： pwrmgmt_init
- debug： aos_debug_init
- und网络诊断： und_init

其中上述组件模块只有被编译链接进相关的app时，对应的宏才会生效。如，只有vfs这个模块被编译链接，AOS_COMP_VFS宏才会生效，vfs_init才会被执行，如：
```C
#ifdef AOS_COMP_VFS
    vfs_init();
#endif
```

## 版权信息
> Apache license v2.0

## 目录结构
```sh
├── aos_init.c     # aos_components_init实现
├── include
│   └── aos
│       └── init.h # 对外头文件
├── package.yaml   # 编译配置文件
└── README.md
```

## 依赖组件
* rhino
* drivers

# 常用配置
init模块本身没有配置，相关的只有aos_component_init模块初始化内各个模块的开关宏；
而这些开关宏是通过yaml内引入组件自动打开的，不需要手动修改相关配置。

# API说明
@ref init_aos_api

# 使用示例
以运行helloworld_demo为例，具体步骤如下：

## 添加init组件
> helloworld_demo组件的package.yaml中添加
```sh
depends:
  - init: master # helloworld_demo中引入init组件
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

## 关键日志
> aos欢迎日志：
```sh
             Welcome to AliOS Things           
```

# FAQ

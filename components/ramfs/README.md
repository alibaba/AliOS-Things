@page ramfs ramfs

# 概述
ramfs是AliOS Things提供的基于RAM不依赖flash驱动的可读可写的文件系统。和ROMFS的区别在于ROMFS为只读，RAMFS可读写。
主要特性包括：
- 不依赖flash设备和驱动
- 可读写创建文件等

## 版权信息
> Apache license v2.0

## 目录结构
```sh
└── src
    ├── ramfs_adapt.c # ramfs和内核的对接层
    ├── ramfs.c       # ramfs核心代码
    └── ramfs_vfs.c   # ramfs和vfs对接层
├── package.yaml      # 编译配置文件
├── example           # 示例代码
├── include           # 对外头文件
│   └── ramfs.h
└── internal          # 内部头文件
```

## 依赖组件
只依赖内核模块

# 常用配置
无

# API说明
@ref ramfs_aos_api
注：ramfs本身只提供注册去注册接口；其他文件操作接口统一使用vfs接口/posix接口。##  ##

# 使用示例
使用示例参考**example/ramfs_example.c**，

以运行helloworld_demo为例，添加运行组件的具体步骤如下：

## 添加示例代码
> ramfs组件的package.yaml中添加example
```sh
source_file:
  - example/*.c
```
## 添加ramfs组件
> helloworld_demo组件的package.yaml中添加
```sh
depends:
  - ramfs: dev_aos # helloworld_demo中引入ramfs组件
```

## 编译
```sh
aos make helloworld_demo@haas100 -c config
aos make
```
其他单板修改haas100即可。

## 烧录固件
> 参考具体板子的快速开始文档。

## ramfs示例测试
> CLI命令行输入：
```sh
ramfs_example
```

## 关键日志
> CLI日志：
```sh
ramfs: fd test success!
ramfs comp test success!
```

# 注意事项
ramfs_register是ramfs的初始化接口，用户可自定义根路径，默认为"/RAMFS"；
ramfs需要对接vfs使用，对外统一使用标准的open/read/write/close接口；

# FAQ



@page littlefs

# 概述
littlefs是一个高度完整的嵌入式文件系统，可以用在自身不带坏块处理、磨损平衡等功能的内存芯片上，如裸的NAND Flash芯片；同时littlefs也充分考虑了异常掉电情况下的数据保护。
组件支持以下功能
- 掉电安全
- 磨损平衡
- 体积小
- 坏快处理

## 版权信息
> Apache license v2.0

## 目录结构
```sh
littlefs
├── src
│   ├── littlefs-v220           #littlefs v220版本源码
│   └── littlefs_vfs.c          #little VFS虚拟文件操作接口的定义与实现
├── include
│   └── littlefs.h              #littlefs 接口
├── internal
│   └── lfs_conf.h              # littlefs 配置文件
├── example
│    └── littlefs_example.c     # 示例代码
├── package.yaml                # 编译配置文件
└─── README.md                  #中文版README
```
## 依赖组件
- rhino nftl

# 常用配置

def_config:
  AOS_COMP_LITTLEFS: 1
  LFS_STATIC_OBJECT: 1
  LITTLEFS_FORMAT: 1

# 使用示例
示例代码参考example/littlefs_example.c，以运行helloworld_demo为例，具体步骤如下：

## 添加示例代码
> littlefs组件的package.yaml中在source_file部分添加example
```sh
source_file:
  - "src/*.c"
  - "example/littlefs_example.c" # add fatfs_example.c
```

## 添加组件
> helloworld_demo组件的package.yaml中添加
```sh
depends:
  - littlefs: master # helloworld_demo中引入littlefs组件
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

## 示例测试
> CLI命令行输入：
```sh
littlefs_example
```

## 关键日志
> CLI日志：
```sh
littlefs component example start!
aos_open fd1=xxx
aos_write num=xxx
aos_open fd2=xxx
aos_lseek position=xxx
aos_read: haas100 littlefs demo
littlefs component example end !
```




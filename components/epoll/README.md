@page epoll epoll

# 概述
epoll实现了linux兼容的高效的IO多路复用机制epoll.

支持以下几种场景：
- socket fd事件通知；
- VFS fd事件通知；
- socket与VFS fd混合事件通知。
-
## 版权信息
> Apache license v2.0

## 目录结构
```sh
.
├── example                     # 示例程序
│   ├── epoll_example.c
│   └── test_device.c
├── include
│   ├── epoll.h                 # 对外头文件
│   ├── poll.h                  # 系统中无poll功能时poll的数据结构定义,常用与驱动开发
│   └── private
│       ├── epoll_inner.h       # epoll内部数据结构
│       └── rbtree_wrapper.h    # 红黑树wrapper层
├── package.yaml                # 编译文件
├── README.md                   # 说明文档
└── src
    ├── device.c                # epoll设备节点实现
    ├── epoll.c                 # epoll接口实现
    └── rbtree_wrapper.c        # 红黑树适配层
```

## 依赖组件
* osal_aos
* vfs

# 常用配置
系统中相关配置已有默认值，如需修改配置，统一在yaml中**def_config**节点修改，具体如下：
> 配置是否支持LWIP:
```sh
def_config:
  CONFIG_EPOLL_LWIP: 1
```
> 配置是否支持标准epoll接口:
```sh
def_config:
  CONFIG_WITH_MUSL: 0 # 0 支持标准epoll接口，非零说明在musl库中支持。
```
# API说明
@ref epoll_aos_api

epoll接口定义与linux的epoll兼容，可参考linux相关文档:
https://man7.org/linux/man-pages/man7/epoll.7.html

# 使用示例
示例代码参考example/epoll_example.c，以运行helloworld_demo为例，具体步骤如下：

## 添加示例代码
> epoll组件的package.yaml中添加example
```sh
source_file:
  - "src/*.c"
  - "example/*.c" # add example source
```

## app中添加epoll组件
> helloworld_demo组件的package.yaml中添加
```sh
depends:
  - epoll: master # helloworld_demo中引入epoll组件
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

## 运行
> 烧入固件后，输入cli命令：
```sh
epoll_example
```
可看到如下打印：
```sh
epoll_example
epoll wait timeout:ret = 0
epoll_wait,ret = 1
epoll_wait,get event, fd = 513, event = 1
 read fd =513, data = 2
```

# 注意
无

# FAQ
无

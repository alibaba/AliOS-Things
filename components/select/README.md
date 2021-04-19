@page select select

# 概述
select组件实现了IO多路复用机制select和poll。

组件支持以下功能：
- select
- poll

支持以下几种场景：
- socket fd事件通知；
- VFS fd事件通知；
- socket与VFS fd混合通知。

## 版权信息
> Apache license v2.0

## 目录结构
```sh
.
├── example        #示例程序
│   ├── select_example.c
│   └── test_device.c
├── include
│   └── select.h   # 对外头文件
├── package.yaml   # 编译文件
├── poll
│   └── poll.c     # aos_poll接口实现
├── README.md      # 说明文档
└── select
    └── select.c   # aos_select接口实现

```

## 依赖组件
* osal_aos
* vfs

# 常用配置
系统中相关配置已有默认值，如需修改配置，统一在yaml中**def_config**节点修改，具体如下：

配置fd_set中fd最大值:
```sh
def_config:
  FD_SETSIZE: 64  # 配置fd_set大小。注意必须大于VFS组件中的 VFS_CONFIG_FD_OFFSET 数值！
```
# API说明

aos_select/aos_poll功能定义与posix标准的select/poll兼容，可参考linux相关接口文档:

[select](https://man7.org/linux/man-pages/man2/select.2.html)
[poll](https://man7.org/linux/man-pages/man2/poll.2.html)


# 使用示例
示例代码参考example/select_example.c，以运行helloworld_demo为例，具体步骤如下。

## 添加示例代码
select组件的package.yaml中添加example：
```sh
source_file:
  - "src/*.c"
  - "example/*.c" # add example source
```

## app中添加select组件
helloworld_demo组件的package.yaml中添加：
```sh
depends:
  - select: dev_aos # helloworld_demo中引入select组件
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
参考具体板子的快速开始文档。

## 运行
烧入固件后，输入cli命令：
```sh
select_example
```
可看到如下打印：
```sh
select_example
+++++++select example+++++++++
aos_select timeout:ret = 0
aos_select ret = 1
 read fd =49,data = 1
+++++++poll example+++++++++
aos_poll timeout, ret = 0
aos_poll ret = 1
 read fd =49,data = 1
```
# 注意事项
无

# FAQ
无

@page select select

[更正文档](https://gitee.com/alios-things/select/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

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
```tree
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
```yaml
def_config:
  FD_SETSIZE: 64  # 配置fd_set大小。注意必须大于VFS组件中的 VFS_CONFIG_FD_OFFSET 数值！
```
# API说明

aos_select/aos_poll功能定义与posix标准的select/poll兼容，可参考linux相关接口文档:

- 参考 [select](https://man7.org/linux/man-pages/man2/select.2.html)
- 参考 [poll](https://man7.org/linux/man-pages/man2/poll.2.html)


# 使用示例
组件使用示例相关的代码下载、编译和固件烧录均依赖AliOS Things配套的开发工具 **alios-studio** ，所以首先需要参考[《aos-studio使用说明之搭建开发环境》](https://g.alicdn.com/alios-things-3.3/doc/setup_env.html)，下载安装 **alios-studio** 。
待开发环境搭建完成后，可以按照以下步骤进行示例的测试。

## 步骤1 创建或打开工程

**打开已有工程**

如果用于测试的案例工程已存在，可参考[《aos-studio使用说明之打开工程》](https://g.alicdn.com/alios-things-3.3/doc/open_project.html)打开已有工程。

**创建新的工程**

组件的示例代码可以通过编译链接到AliOS Things的任意案例（solution）来运行，这里选择helloworld_demo案例。helloworld_demo案例相关的源代码下载可参考[《aos-studio使用说明之创建工程》](https://g.alicdn.com/alios-things-3.3/doc/create_project.html)。

## 步骤2 添加组件

案例下载完成后，需要在helloworld_demo组件的package.yaml中添加对组件的依赖：
```sh
depends:
  - select: master # helloworld_demo中引入select组件
```
## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install select

```

上述命令执行成功后，组件源码则被下载到了./components/select路径中。

## 步骤4 添加示例

在select组件的package.yaml中添加example：
```yaml
source_file:
  - "src/*.c"
  - "example/*.c" # add example source
```

## 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

## 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

## 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

## 步骤8 测试示例

在串口终端输入cli命令：
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

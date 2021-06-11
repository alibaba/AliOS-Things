@page littlefs

[更正文档](https://gitee.com/alios-things/littlefs/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

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
```tree
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

```yaml
def_config:
  AOS_COMP_LITTLEFS: 1
  LFS_STATIC_OBJECT: 1
  LITTLEFS_FORMAT: 1
```

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

```yaml

depends:
  - littlefs: master # helloworld_demo中引入littlefs组件

```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install littlefs

```

上述命令执行成功后，组件源码则被下载到了./components/littlefs路径中。

## 步骤4 添加示例

在littlefs组件的package.yaml中添加[example示例代码](https://gitee.com/alios-things/littlefs/tree/master/example)：

```yaml
source_file:
  - "src/*.c"
  - "example/littlefs_example.c" # add fatfs_example.c
```

## 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

## 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

## 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

## 步骤8 测试示例
> CLI命令行输入：

```shell
littlefs_example
```

**关键日志**
> CLI日志：

```shell
littlefs component example start!
aos_open fd1=xxx
aos_write num=xxx
aos_open fd2=xxx
aos_lseek position=xxx
aos_read: haas100 littlefs demo
littlefs component example end !
```




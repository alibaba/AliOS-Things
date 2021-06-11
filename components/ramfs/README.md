@page ramfs ramfs

# 概述
ramfs是AliOS Things提供的基于RAM不依赖flash驱动的可读可写的文件系统。和ROMFS的区别在于ROMFS为只读，RAMFS可读写。
主要特性包括：
- 不依赖flash设备和驱动
- 可读写创建文件等
- 掉电不保存

## 版权信息
> Apache license v2.0

## 目录结构
```tree
├── src
│   ├── ramfs_adapt.c # ramfs和内核的对接层
│   ├── ramfs.c       # ramfs核心代码
│   └── ramfs_vfs.c   # ramfs和vfs对接层
├── package.yaml      # 编译配置文件
├── example           # 示例代码
├── include           # 对外头文件
│   └── ramfs.h
└── internal          # 内部头文件
```

## 依赖组件
* osal_aos

# 常用配置
无

# API说明
- 参考 [ramfs_aos_api](https://g.alicdn.com/alios-things-3.3/doc/group__ramfs__aos__api.html) 
注：ramfs本身只提供注册去注册接口；其他文件操作接口统一使用vfs接口/posix接口。

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
  - ramfs: master # helloworld_demo中引入ramfs组件
```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install ramfs

```

上述命令执行成功后，组件源码则被下载到了./components/ramfs路径中。

## 步骤4 添加示例

> ramfs组件的package.yaml中添加example
```yaml
source_file:
  - example/*.c
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
```sh
ramfs_example
```

**关键日志**
> CLI日志：
```sh
ramfs: fd test success!
ramfs comp test success!
```

# 注意事项
ramfs_register是ramfs的初始化接口，用户可自定义根路径，默认为"/RAMFS"；
ramfs需要对接vfs使用，对外统一使用标准的open/read/write/close接口；

# FAQ



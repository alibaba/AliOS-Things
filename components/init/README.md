@page init init

[更正文档](https://gitee.com/alios-things/init/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

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
```tree
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

- 参考 [init_aos_api](https://g.alicdn.com/alios-things-3.3/doc/group__init__aos__api.html)

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
  - init: master # helloworld_demo中引入init组件

```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install init

```

上述命令执行成功后，组件源码则被下载到了./components/init路径中。

## 步骤4 添加示例

init组件不需要添加example示例代码，忽略此步骤。


## 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

## 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

## 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

## 步骤8 测试示例

```shell

复位开发版硬件，可以看到下述关键日志的打印，说明init组件已经成功运行。

```

**关键日志**
> aos欢迎日志：
```shell
             Welcome to AliOS Things           
```

# FAQ

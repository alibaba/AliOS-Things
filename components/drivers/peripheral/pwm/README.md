@page pwm pwm

[更正文档](https://gitee.com/alios-things/adc/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://help.aliyun.com/document_detail/302301.html)

# 概述
PWM（ Pulse Width Modulation），即脉冲宽度调制，是利用微处理器的数字输出来对模拟电路进行控制的一种技术。
该组件是PWM VFS驱动子系统，给应用或组件提供通过VFS形式（open/close/ioctl）访问PWM控制器驱动对上层提供服务的接口。该组件初始化过程中，会根据hardware/chip/<chip_name>/package.yaml中定义的CONFIG_PWM_NUM来依此创建如下名称的设备节点：

* /dev/pwm0
* /dev/pwm1
* ...
* /dev/pwm[CONFIG_PWM_NUM - 1]

组件支持以下功能：
- 输出不同频率和占空比的方波

## 版权信息
> Apache license v2.0

## 目录结构
```tree
pwm/
├── README.md
├── example
│   ├── aos_pwm_example.c
│   └── pwm_example.c
├── include
│   ├── aos
│   │   ├── hal
│   │   │   └── pwm.h
│   │   ├── pwm.h
│   │   ├── pwm_core.h
│   │   └── pwm_csi.h
│   └── vfsdev
│       └── pwm_dev.h
├── package.yaml
└── src
    ├── pwm.c
    ├── pwm_csi.c
    └── pwm_dev.c

```

## 依赖组件
* base     # 最底层核心驱动模型、驱动自动加载机制以及Device VFS core
* vfs      # VFS API抽象库

# 常用配置
本组件可以通过CONFIG_PWM_NUM配置对上层提供设备节点的数量，CONFIG_PWM_NUM代表芯片内部的PWM控制器的数量。
CONFIG_PWM_NUM默认是在hardware/chip/<chip_name>/package.yaml中的**def_config**节点进行配置。
如果没有定义CONFIG_PWM_NUM，则代码中会定义默认对外输出的设备节点数量。

> 设备节点数量: 默认4个, 如需修改，在hardware/chip/<chip_name>/package.yaml中修改CONFIG_PWM_NUM配置
```yaml
def_config:
  CONFIG_PWM_NUM: 2
```

# API说明
应用可以选择使用PWM VFS API或者原生的AOS PWM API。

- PWM VFS API 参考 [adc_device_api](https://g.alicdn.com/alios-things-3.3/doc/group__adc__device__api.html)
- AOS PWM API 参考 [aos_adc_app_api](https://g.alicdn.com/alios-things-3.3/doc/group__aos__adc__app.html)

芯片驱动对接

- PWM驱动操作API 参考 [aos_adc_driver_api](https://g.alicdn.com/alios-things-3.3/doc/group__aos__adc__driver.html)

# 使用示例

组件使用示例相关的代码下载、编译和固件烧录均依赖AliOS Things配套的开发工具，所以首先需要参考[《AliOS Things集成开发环境使用说明之搭建开发环境》](https://help.aliyun.com/document_detail/302378.html)，下载安装。
待开发环境搭建完成后，可以按照以下步骤进行示例的测试。

## 步骤1 创建或打开工程

**打开已有工程**

如果用于测试的案例工程已存在，可参考[《AliOS Things集成开发环境使用说明之打开工程》](https://help.aliyun.com/document_detail/302381.html)打开已有工程。

**创建新的工程**

组件的示例代码可以通过编译链接到AliOS Things的任意案例（solution）来运行，这里选择helloworld_demo案例。helloworld_demo案例相关的源代码下载可参考[《AliOS Things集成开发环境使用说明之创建工程》](https://help.aliyun.com/document_detail/302379.html)。

## 步骤2 添加组件

案例下载完成后，需要在helloworld_demo组件的package.yaml中添加对组件的依赖：

```yaml

depends:
  - pwm: master          # helloworld_demo中引入pwm组件

```

## 步骤3 下载组件

在已安装了  的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install pwm

```

上述命令执行成功后，组件源码则被下载到了./components/drivers/peripheral/pwm路径中。

## 步骤4 添加示例

在pwm组件的package.yaml中添加[example示例代码](https://gitee.com/alios-things/adc/tree/master/example)：

```yaml
source_file:
  - "src/*.c"
  - "example/pwm_example.c"
  - "example/aos_pwm_example.c"
```

## 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《AliOS Things集成开发环境使用说明之编译固件》](https://help.aliyun.com/document_detail/302384.html)。

## 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《AliOS Things集成开发环境使用说明之烧录固件》](https://help.aliyun.com/document_detail/302383.html)来烧录固件。

## 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《AliOS Things集成开发环境使用说明之查看日志》](https://help.aliyun.com/document_detail/302382.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

## 步骤8 测试示例

**CLI命令行输入：**
```shell

pwm_example # 执行pwm vfs api 示例

```

> 关键日志：
```shell

pwm comp output test success!

```

**CLI命令行输入：**
```shell

aos_pwm_example # 执行aos pwm api 示例

```

> 关键日志：
```shell

aos_pwm_test success
```

# 注意事项
无

# FAQ
无

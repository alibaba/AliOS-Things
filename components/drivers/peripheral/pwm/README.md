@page pwm pwm

**[更正文档](https://gitee.com/alios-things/pwm/edit/master/README.md)** &emsp;&emsp;&emsp;&emsp; **[贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)**

# 概述
> PWM VFS驱动子系统，该驱动的目的是给应用或组件提供通过VFS形式（open/close/ioctl）访问PWM控制器驱动对上层提供服务的接口。
> 该组件初始化过程中，会根据hardware/chip/<chip_name>/package.yaml中定义的CONFIG_PWM_NUM来依此创建如下名称的设备节点：

* /dev/pwm0
* /dev/pwm1
* ...
* /dev/pwm[CONFIG_PWM_NUM - 1]

> PWM VFS驱动子系统内部会将open/close/ioctl请求转换成对应硬件的HAL层接口调用，通过HAL层接口控制主控芯片的PWM控制器的参数（频率、占空比等）的设定或发起PWM输出波形请求。

## 版权信息
> Apache license v2.0

## 目录结构
```tree
│── src
│   └── pwm_dev.c      # PWM VFS驱动程序源代码
├── example            # PWM VFS驱动程序使用案例
│   └── pwm_example.c  # 通过PWM VFS接口进行PWM通信的案例文件
├── include            # PWM驱动子系统对外提供头文件目录
│   ├── aos
│   │   └── hal
│   │       └── pwm.h  # AOS HAL API头文件声明，本文件中的API是给厂商对接HAL API的原型声明
│   └── vfsdev
│       └── pwm_dev.h  # PWM VFS驱动子系统对应用/组件提供的接口声明文件
└── package.yaml       # PWM VFS驱动子系统的组件配置文件

```

## 依赖组件
* base     # 最底层核心驱动模型、驱动自动加载机制以及Device VFS core
* vfs      # VFS API抽象库

# 常用配置
> 本组件可以通过CONFIG_PWM_NUM配置对上层提供设备节点的数量，CONFIG_PWM_NUM代表芯片内部的PWM控制器的数量。
> CONFIG_PWM_NUM默认是在hardware/chip/<chip_name>/package.yaml中的**define**节点进行配置。
> 如果没有定义CONFIG_PWM_NUM，则代码中会定义默认对外输出的设备节点数量。
> 设备节点数量: 默认4个, 如需修改，在hardware/chip/<chip_name>/package.yaml中修改CONFIG_PWM_NUM配置
```sh
define:
  CONFIG_PWM_NUM: 2
```

# API说明
## 打开设备节点
```c
int open(const char *pathname, int flags);
```

|args                                    |description|
|:-----                                  |:----|
|pathname                                |PWM控制器外设VFS路径|
|flags                                   |目前固定为0值|

兼容POSIX标准的open接口。其中参数*pathname*为*/dev/pwm*加pwm控制器序号，例如*/dev/pwm0*。

## 关闭设备节点
```c
int close(int fd);
```
兼容POSIX标准的close接口。

## PWM 参数设置
### PWM频率设定
在开启PWM控制器输出PWM波形之前需先设定PWM频率参数
```c
int ioctl(int fd, IOC_PWM_FREQ, unsigned long arg);

//arg为目标PWM的频率
// 假如设置PWM波形频率为100
ioctl (fd, IOC_PWM_FREQ, 100);
```

### PWM占空比设定
在开启PWM控制器输出PWM波形之前需先设定PWM占空比参数
```c
int ioctl(int fd, IOC_PWM_DUTY_CYCLE, unsigned long arg);

//arg为指向PWM占空比的指针
// 假如设置PWM波形占空比为0.5
double duty_cycle = 0.5;
ioctl (fd, IOC_PWM_DUTY_CYCLE, &duty_cycle);
```

### 控制PWM开始/停止输出波形

```c
int ioctl(int fd, int cmd, unsigned long arg);
//arg传入0即可
```
其中cmd取值及其意义说明如下：

|cmd            |功能         |
|:-----         |:----        |
|IOC_PWM_ON     |开始输出PWM波形|
|IOC_PWM_OFF    |停止输出PWM波形|

# 使用示例

组件使用示例相关的代码下载、编译和固件烧录均依赖AliOS Things配套的开发工具 **alios-studio** ，所以首先需要参考[《aos-studio使用说明之搭建开发环境》](https://g.alicdn.com/alios-things-3.3/doc/setup_env.html)，下载安装 **alios-studio** 。
待开发环境搭建完成后，可以按照以下步骤进行示例的测试。

## 步骤1 创建或打开工程

**打开已有工程**

如果用于测试的案例工程已存在，可参考[《aos-studio使用说明之打开工程》](https://g.alicdn.com/alios-things-3.3/doc/open_project.html)打开已有工程。

**创建新的工程**

组件的示例代码可以通过编译链接到AliOS Things的任意案例（solution）来运行，这里选择helloworld_demo案例。helloworld_demo案例相关的源代码下载可参考[《aos-studio使用说明之创建工程》](https://g.alicdn.com/alios-things-3.3/doc/create_project.html)。

## 步骤2 添加组件
> 如果芯片内部含有PWM控制器，则芯片厂在操作系统对接的时候已经将PWM组件添加在了芯片级别配置文件“hardware/chip/<chip_name>/package.yaml”中，此配置主要包含“设置组件依赖关系”及“设置芯片内部所含PWM控制器数量”两个配置。
```yaml
# 设置组件依赖关系
depends:
  - pwm: master

# 设置芯片内部所含PWM控制器数量
define:
  CONFIG_PWM_NUM: 2
```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install pwm

```

上述命令执行成功后，组件源码则被下载到了./components/drivers/peripheral/pwm路径中。

## 步骤4 添加示例

在i2c组件的package.yaml中添加[example示例代码](https://gitee.com/alios-things/pwm/tree/master/example)：

```yaml
source_file:

PWM device driver
#PWM VFS driver
  - src/pwm_dev.c ? <CONFIG_U_PWM_DEV>
#PWM VFS driver example
- example/pwm_example.c ? <CONFIG_U_PWM_DEV>
```

## 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

## 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

## 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

## 步骤8 测试示例

**CLI命令行输入：**
```shell
pwm_output <channel id> <freqency> <period, in unit of s>
# 如向PWM通道0输出频率为100，占空比为0.5(pwm_example.c中固定参数)的波形，持续时间3秒
pwm_output 0 100 3
```

> 关键日志
```shell
pwm comp output test success!
```

# 注意事项
> PWM是总线类型的bus，进行pwm组件输出测试，测试结果可以通过示波器进行量测或通过外拐有声光效果的元器件（比如LED，蜂鸣器）观察不同的声光效果。

# FAQ
> 无


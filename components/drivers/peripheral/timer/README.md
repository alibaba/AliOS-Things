@page timer timer

**[更正文档](https://gitee.com/alios-things/timer/edit/master/README.md)** &emsp;&emsp;&emsp;&emsp; **[贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)**

# 概述
Timer，即通用硬件定时器。
该组件是Timer VFS驱动子系统，给应用或组件提供通过VFS形式（open/close/ioctl）访问硬件Timer驱动对上层提供服务的接口。该组件初始化过程中，会根据hardware/chip/<chip_name>/package.yaml中定义的CONFIG_TIMER_NUM来依此创建如下名称的设备节点：
* /dev/timer0
* /dev/timer1
* ...
* /dev/timer[CONFIG_TIMER_NUM - 1]

组件支持以下功能：
- 设置定时器运行周期
- 设置中断回调函数
- 启动/停止定时器

## 版权信息
> Apache license v2.0

## 目录结构
```tree
.
├── example
│   └── timer_example.c  #Timer示例代码
├── include
│   ├── aos
│   │   └── hal
│   │       └── timer.h   #Timer HAL API申明
│   └── vfsdev
│       └── timer_dev.h   #Timer设备API申明
├── package.yaml          #编译和配置文件
├── README.md             #使用文档
└── src
    └── timer_dev.c       #Timer设备实现代码
```

## 依赖组件
* base     # 最底层核心驱动模型、驱动自动加载机制以及Device VFS core
* vfs      # VFS API抽象库

# 常用配置
本组件可以通过CONFIG_TIMER_NUM配置对上层提供设备节点的数量，CONFIG_TIMER_NUM代表芯片内部的硬件定时器控制器的数量。
CONFIG_TIMER_NUM默认是在hardware/chip/<chip_name>/package.yaml中的**define**节点进行配置。
如果没有定义CONFIG_TIMER_NUM，则代码中会定义默认对外输出的设备节点数量。
> 设备节点数量: 默认4个, 如需修改，在hardware/chip/<chip_name>/package.yaml中修改CONFIG_TIMER_NUM配置
```yaml
define:
  CONFIG_TIMER_NUM: 1
```

# API说明
## 打开设备节点
```c
int open(const char *pathname, int flags);
```

|args                                    |description|
|:-----                                  |:----|
|pathname                                |硬件定时器外设VFS路径|
|flags                                   |目前固定为0值|

兼容POSIX标准的open接口。其中参数*pathname*为*/dev/timer*加硬件定时器外设id，例如*/dev/timer0*。

## 关闭设备节点
```c
int close(int fd);
```
兼容POSIX标准的close接口。

## 设置定时器参数
在需要设定硬件定时器的定时及超时处理函数的时候呼叫此API
```c
int ioctl(int fd, IOC_TIMER_IRQP_SET, unsigned long arg);
// arg为指向timer_alarm_t结构体的指针
// 如需设定周期为500ms的周期性定时器，并在硬件定时器到期后呼叫timer_handler(argument_pointer),代码如下：
timer_alarm_t ralarm;
alarm.arg = argument_pointer; // 呼叫定时器到期处理函数的时候传入的参数
alarm.cb = timer_handler;     // 定时器到期处理函数指针
alarm.period = 500000;        // 单位:ms
alarm.auto_reload = true;     // 开启自动重新加载硬件定时器功能
ioctl (fd, IOC_TIMER_IRQP_SET, &ralarm);
```

## 获取定时器当前设定参数
在需要获取硬件定时器的当前的定时参数的时候呼叫此API
```c
int ioctl(int fd, IOC_TIMER_IRQP_GET, unsigned long arg);

// 示例代码
timer_alarm_t ralarm;
ioctl (fd, IOC_TIMER_IRQP_GET, &alarm);
```

## 开启/停止定时器

```c
int ioctl(int fd, IOC_TIMER_CONTROL, unsigned long arg);

// arg为IO_TIMER_START代表开启定时器, 示例代码：
ioctl (fd, IOC_TIMER_CONTROL, IO_TIMER_START)
// arg为IO_TIMER_STOP代表停止定时器
ioctl (fd, IOC_TIMER_CONTROL, IO_TIMER_STOP)
```

## 控制定时器是否自动重新加载
```c
int ioctl(int fd, IOC_TIMER_RELOAD, unsigned long arg);

// arg为true代表开启自动重载定时器功能, 示例代码：
ioctl (fd, IOC_TIMER_RELOAD, true)
// arg为false代表停止自动重载定时器功能, 示例代码：
ioctl (fd, IOC_TIMER_RELOAD, false)
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
  - timer: master # helloworld_demo中引入timer组件
```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install timer

```

上述命令执行成功后，组件源码则被下载到了./components/drivers/peripheral/timer路径中。

## 步骤4 添加示例

在timer组件的package.yaml中添加[example示例代码](https://gitee.com/alios-things/timer/tree/master/example)：
```yaml
source_file:
  原有其他文件
  - example/timer_example.c # add timer_example.c
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
timer_example id
```
其中id取值范围：[0, CONFIG_TIMER_NUM)

> 关键日志
```shell
vfs timer test success, ret:0
```

# 注意事项
无

# FAQ
无

@page gpio GPIO

[更正文档](https://gitee.com/alios-things/gpio/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

# 概述
GPIO设备VFS驱动子系统。

## 版权说明
> Apache license v2.0

## 目录结构
```tree
├── src
│   └── gpio_dev.c          # HAL to VFS适配
├── include
│   └── aos
│       ├── hal
│       │   └── gpio.h      # HAL API
│       └── vfsdev
│           └── gpio_dev.h  # VFS API
├── package.yaml            # 编译配置文件
└── example
    └── gpio_example.c      # 示例代码（默认禁用，可在package.yaml中使能）
```

## 依赖组件
* base
* vfs

# 常用配置
无

# API说明

## 打开
```c
int open(const char *pathname, int flags);
```
|args                                    |description|
|:-----                                  |:----|
|pathname                                |固定为`"/dev/gpio"`|
|flags                                   |目前固定为0值|
兼容POSIX标准的open接口。

## 关闭
```c
int close(int fd);
```
兼容POSIX标准的close接口。

## 配置输入属性并获取输入电平
```c
typedef struct gpio_io_config {
    unsigned int id : 8;        // GPIO id
    unsigned int config : 16;   // GPIO config
    unsigned int data : 1;      // output data
} gpio_io_config_t;

int ioctl(int fd, IOC_GPIO_GET, gpio_io_config_t *config);
```
参数config中的id成员变量表示GPIO pin ID。
参数config中的config成员变量表示输入属性：
|符号                               |含义|
|:-----                             |:----|
|GPIO_IO_INPUT \| GPIO_IO_INPUT_PU  |输入上拉|
|GPIO_IO_INPUT \| GPIO_IO_INPUT_PD  |输入下拉|
|GPIO_IO_INPUT \| GPIO_IO_INPUT_HI  |输入高阻|
参数config中的data成员变量无意义。
返回值等于0表示输入为低电平，大于0表示输入为高电平，小于0表示出错。

## 配置输出属性并设置输出电平
```c
int ioctl(int fd, IOC_GPIO_SET, gpio_io_config_t *config);
```
参数config中的id成员变量表示GPIO pin ID。
参数config中的config成员变量表示输出属性：
|符号                                   |含义|
|:-----                                 |:----|
|GPIO_IO_OUTPUT \| GPIO_IO_OUTPUT_PP    |输出推挽|
|GPIO_IO_OUTPUT \| GPIO_IO_OUTPUT_ODNP  |输出开漏无上拉|
|GPIO_IO_OUTPUT \| GPIO_IO_OUTPUT_ODPU  |输出开漏有上拉|
|GPIO_IO_TOGGLE                         |翻转输出电平|
参数config中的data成员变量表示输出电平，0表示低电平，1表示高电平；config为GPIO_IO_TOGGLE时data无意义。
返回值等于0表示成功，小于0表示出错。

## 配置中断
```c
typedef int (*io_irq_handler_t)(void *arg);

typedef struct gpio_irq_config {
    unsigned int id : 8;        // GPIO id
    unsigned int config : 16;   // IRQ config
    unsigned char *arg;         // IRQ callback argument
    io_irq_handler_t cb;        // IRQ callback
} gpio_irq_config_t;

int ioctl(int fd, IOC_GPIO_SET_IRQ, gpio_irq_config_t *config);
```
参数config中的id成员变量表示GPIO pin ID。
参数config中的config成员变量表示中断属性：
|符号                                       |含义|
|:-----                                     |:----|
|GPIO_IRQ_ENABLE \| GPIO_IRQ_EDGE_RISING    |使能上升沿触发中断|
|GPIO_IRQ_ENABLE \| GPIO_IRQ_EDGE_FALLING   |使能下降沿触发中断|
|GPIO_IRQ_ENABLE \| GPIO_IRQ_EDGE_BOTH      |使能上升下降沿触发中断|
|GPIO_IRQ_DISABLE                           |禁用中断|
参数config中的cb成员变量表示中断回调函数。
参数config中的arg成员变量表示传递给中断回调函数的参数。
返回值等于0表示成功，小于0表示出错。

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
  - gpio: master          # helloworld_demo中引入gpio组件

```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install gpio

```

上述命令执行成功后，组件源码则被下载到了./components/drivers/peripheral路径中。

## 步骤4 添加示例
gpio组件的package.yaml中添加example
```yaml
source_file:
  - "src/*.c"
  - "example/gpio_example.c"
```
## 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

## 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

## 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

## 步骤8 测试示例
CLI命令行输入：
```
gpiot <pin ID>
```

**关键日志**
示例测试日志显示：
```
open gpio success, fd:<fd>
gpio read <pin ID> return <ioctl IOC_GPIO_GET返回值>
gpio write <pin ID> return <ioctl IOC_GPIO_SET（配置输出）返回值>
gpio toggle <pin ID> return <ioctl IOC_GPIO_SET（翻转输出）返回值>
```

# 注意事项
无

# FAQ
1. API中的pin ID如何填写？
> 一般来说GPIO是分组的。以HaaS100为例，它的GPIO pin是8个一组。如果某GPIO pin名称为GPIO_2_6，则pin ID应为2 \* 8 + 6 = 22。其它pin ID的计算方法依此类推。

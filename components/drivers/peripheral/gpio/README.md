@page gpio GPIO

# 概述
GPIO设备VFS驱动子系统。

## 版权说明
> Apache license v2.0

## 目录结构
```
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
示例代码参考example/gpio_example.c，以运行helloworld_demo为例，具体步骤如下：

## 添加示例代码
gpio组件的package.yaml中添加example
```
source_file:
  - "src/*.c"
  - "example/gpio_example.c"
```

## 添加组件
helloworld_demo组件默认已依赖gpio组件。

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

## 示例测试
CLI命令行输入：
```
gpiot <pin ID>
```

## 关键日志
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

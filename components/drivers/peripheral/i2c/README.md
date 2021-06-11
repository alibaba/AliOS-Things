@page i2c i2c

**[更正文档](https://gitee.com/alios-things/i2c/edit/master/README.md)** &emsp;&emsp;&emsp;&emsp; **[贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)**

# 概述
> I2C VFS驱动子系统，该驱动的目的是给应用或组件提供通过VFS形式（open/close/ioctl）访问I2C控制器驱动对上层提供服务的接口。
> 该组件初始化过程中，会根据hardware/chip/<chip_name>/package.yaml中定义的CONFIG_I2C_NUM来依此创建如下名称的设备节点：

* /dev/i2c0
* /dev/i2c1
* ...
* /dev/i2c[CONFIG_I2C_NUM - 1]

> I2C VFS驱动子系统内部会将open/close/ioctl请求转换成对应硬件的HAL层接口调用，通过HAL层接口控制主控芯片对I2C控制器的参数（从设备地址、CLK等）的设定或发起I2C 读或写操作请求。

## 版权信息
> Apache license v2.0

## 目录结构
```tree
│── src
│   └── i2c_dev.c      # I2C VFS驱动程序源代码
├── example            # I2C VFS驱动程序使用案例
│   └── i2c_example.c  # 通过I2C VFS接口进行I2C通信的案例文件
├── include            # I2C驱动子系统对外提供头文件目录
│   ├── aos
│   │   └── hal
│   │       └── i2c.h  # AOS HAL API头文件声明，本文件中的API是给厂商对接HAL API的原型声明
│   └── vfsdev
│       └── i2c_dev.h  # I2C VFS驱动子系统对应用/组件提供的接口声明文件
└── package.yaml       # I2C VFS驱动子系统的组件配置文件

```

## 依赖组件
* base     # 最底层核心驱动模型、驱动自动加载机制以及Device VFS core
* vfs      # VFS API抽象库
* epoll    # 增强型多路复用IO接口 - epoll机制所在库

# 常用配置
> 本组件可以通过CONFIG_I2C_NUM配置对上层提供设备节点的数量，CONFIG_I2C_NUM代表芯片内部的I2C控制器的数量。
> CONFIG_I2C_NUM默认是在hardware/chip/<chip_name>/package.yaml中的**define**节点进行配置。
> 如果没有定义CONFIG_I2C_NUM，则代码中会定义默认对外输出的设备节点数量。
> 设备节点数量: 默认4个, 如需修改，在hardware/chip/<chip_name>/package.yaml中修改CONFIG_I2C_NUM配置
```yaml
define:
  CONFIG_I2C_NUM: 2
```

# API说明
## 打开设备节点
```c
int open(const char *pathname, int flags);
```

|args                                    |description|
|:-----                                  |:----|
|pathname                                |I2C控制器外设VFS路径|
|flags                                   |目前固定为0值|

兼容POSIX标准的open接口。其中参数*pathname*为*/dev/i2c*加i2c控制器序号，例如*/dev/i2c0*。

## 关闭设备节点
```c
int close(int fd);
```
兼容POSIX标准的close接口。

## I2C 参数设置
### I2C 数据传输频率
arg为指向io_i2c_control_u的指针。

```c
int ioctl(int fd, IOC_I2C_SET_FREQ, unsigned long arg);

// 假如和从设备通信clock频率为100k, 则用如下代码对其进行设置
io_i2c_control_u c;
c.freq = 100000; // set i2c clock frequence to 100k
ioctl (fd, IOC_I2C_SET_FREQ, &c);

```

### I2C 地址模式、角色及从设备地址设置
arg为指向io_i2c_control_u的指针。

```c
int ioctl(int fd, IOC_I2C_SET_CONFIG, unsigned long arg);

// 假如从设备地址为0x40, 地址宽度为7-bit, 则使用如下代码对其进行设置
io_i2c_control_u c;
c.c.addr = 0x40;     // I2C从设备地址设置为0x40
c.c.addr_width = 0;  // I2C从设备地址宽度为7-bit
c.c.role = 1;        // i2c master模式
ioctl (fd, IOC_I2C_SET_CONFIG, &c);

```

### I2C 数据收发

arg为指向io_i2c_data_t的指针。

```c
int ioctl(int fd, int cmd, unsigned long arg);

```
其中cmd取值及其意义说明如下：

|cmd                  |功能                                       |
|:-----               |:----                                     |
|IOC_I2C_MASTER_RX    |控制I2C控制器从外部I2C从设备读取数据           |
|IOC_I2C_MASTER_TX    |控制I2C控制器向外部I2C从设备发送数据           |
|IOC_I2C_MEM_RX       |控制I2C控制器从外部I2C存储类型的从设备读取数据   |
|IOC_I2C_MEM_TX       |控制I2C控制器向外部I2C存储类型从设备发送数据     |

io_i2c_data_t 结构体变量说明如下:

|变量|说明                                         |
|:-----     |:----                                |
|addr       | 此次读写操作对应的i2c送设备地址          |
|length     | 此次读写操作要读写数据的长度             |
|data       | 写操作：指向要发送数据的首地址；读操作：指向接收数据存放内存块首地址 |
|maddr      | 此次内存操作要读写的memory地址，只有在ioctl cmd id为IOC_I2C_MEM_TX/IOC_I2C_MEM_RX时有效 |
|mlength    | 此次内存操作要读写的memory的长度，只有在ioctl cmd id为IOC_I2C_MEM_TX/IOC_I2C_MEM_RX时有效 |
|timeout    | 此次操作的超时时间，单位：ms |

# 使用示例
组件使用示例相关的代码下载、编译和固件烧录均依赖AliOS Things配套的开发工具 **alios-studio** ，所以首先需要参考[《aos-studio使用说明之搭建开发环境》](https://g.alicdn.com/alios-things-3.3/doc/setup_env.html)，下载安装 **alios-studio** 。
待开发环境搭建完成后，可以按照以下步骤进行示例的测试。

## 步骤1 创建或打开工程

**打开已有工程**

如果用于测试的案例工程已存在，可参考[《aos-studio使用说明之打开工程》](https://g.alicdn.com/alios-things-3.3/doc/open_project.html)打开已有工程。

**创建新的工程**

组件的示例代码可以通过编译链接到AliOS Things的任意案例（solution）来运行，这里选择helloworld_demo案例。helloworld_demo案例相关的源代码下载可参考[《aos-studio使用说明之创建工程》](https://g.alicdn.com/alios-things-3.3/doc/create_project.html)。

## 步骤2 添加组件
> 如果芯片内部含有I2C控制器，则芯片厂在操作系统对接的时候已经将I2C组件添加在了芯片级别配置文件“hardware/chip/<chip_name>/package.yaml”中，此配置主要包含“设置组件依赖关系”及“设置芯片内部所含I2C控制器数量”两个配置。
```yaml
# 设置组件依赖关系
depends:
  - i2c: master

# 设置芯片内部所含I2C控制器数量
define:
  CONFIG_I2C_NUM: 2
```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install i2c

```

上述命令执行成功后，组件源码则被下载到了./components/drivers/peripheral/i2c路径中。

## 步骤4 添加示例

在i2c组件的package.yaml中添加[example示例代码](https://gitee.com/alios-things/i2c/tree/master/example)：

```yaml
source_file:

I2C device driver
#i2c VFS driver
  - src/i2c_dev.c ? <CONFIG_U_I2C_DEV>
#i2c VFS driver example
- example/i2c_example.c ? <CONFIG_U_I2C_DEV>
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
# 向挂载在I2C通道1上面的SI7006温湿度传感器（从设备地址）发送一个0xF5的指令，控制其量测温度
i2c_write 1 64 245 1
```

> 关键日志
```shell
i2c comp write test success!
```

**CLI命令行输入：**
```shell
# 向挂载在I2C通道1上面的SI7006温湿度传感器（从设备地址）发送读2个字节数据的指令
i2c_read 1 64 2
```

> 关键日志
```shell
i2c comp read test success!
```

# 注意事项
> I2C是总线类型的bus，进行i2c组件读写测试，必须确保i2c总线上面有外挂i2c 从设备，并且i2c控制器id、i2c从设备地址、写入数据及长度信息都正确才能得到正确的执行结果；以上任一信息出错，执行i2c_read/i2c_write 都可能得到错误的执行结果。

# FAQ
> I2C总线协议的介绍可参考视频学习：https://www.bilibili.com/video/BV1zb4y197h6/


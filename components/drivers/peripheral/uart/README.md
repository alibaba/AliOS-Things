@page uart UART

[更正文档](https://gitee.com/alios-things/uart/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

# 概述
UART设备VFS驱动子系统。

## 版权说明
> Apache license v2.0

## 目录结构
```tree
├── src
│   └── uart_dev.c          # HAL to VFS适配
├── include
│   └── aos
│       ├── hal
│       │   └── uart.h      # HAL API
│       └── vfsdev
│           └── uart_dev.h  # VFS API
├── package.yaml            # 编译配置文件
└── example
    └── uart_example.c      # 示例代码（默认禁用，可在package.yaml中使能）
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
|pathname                                |UART设备VFS路径|
|flags                                   |目前固定为0值|
兼容POSIX标准的open接口。其中参数*pathname*为*/dev/ttyUART*加UART序号,例如:
```c
fd = open("/dev/ttyUART0", 0);
```

## 关闭
```c
int close(int fd);
```
兼容POSIX标准的close接口。

## 接收数据
```c
ssize_t read(int fd, void *buf, size_t count);
```
兼容POSIX标准的read接口。接收缓冲区为空时返回0。

## 发送数据
```c
ssize_t write(int fd, const void *buf, size_t count);
```
兼容POSIX标准的write接口。发送缓冲区已满时返回0。

## 查询IO事件
```c
int poll(struct pollfd *fds, nfds_t nfds, int timeout);
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
```
兼容POSIX标准的poll/select接口。

## 设置属性
```c
int ioctl(int fd, IOC_UART_SET_CFLAG, unsigned long flags);
```
参数flags包含波特率、数据位数、校验方式等属性，各属性之间是逻辑或关系。
### 波特率
用*B*加波特率数值表示，例如*B115200*。
### 数据位数
|符号                           |含义|
|:-----                         |:----|
|CS5                            |数据长度5位|
|CS6                            |数据长度6位|
|CS7                            |数据长度7位|
|CS8                            |数据长度8位|

### 校验方式
|符号                           |含义|
|:-----                         |:----|
|0                              |无校验|
|PARENB                         |偶校验|
|PARENB \| PARODD               |奇校验|

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
  - uart: master          # helloworld_demo中引入uart组件

```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install uart

```

上述命令执行成功后，组件源码则被下载到了./components/drivers/peripheral/uart路径中。

## 步骤4 添加示例
uart组件的package.yaml中添加example
```yaml
source_file:
  - "src/*.c"
  - "example/uart_example.c"
```

## 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

## 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

## 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

## 步骤8 测试示例

UART接收测试，CLI命令行输入：
```sh
uartr <UART ID> <测试持续时间（ms）>
```
UART发送测试，CLI命令行输入：
```sh
uartw <UART ID> <待发送字符串>
```
UART回显测试，CLI命令行输入：
```sh
uarte <UART ID> <测试持续时间（ms）>
```

**关键日志**
UART接收测试日志显示：
```sh
opening device:/dev/ttyUART<UART ID>
set baudrate to 1500000
set baudrate done
<显示接收到的字符串>
closing /dev/ttyUART<UART ID>
/dev/ttyUART<UART ID> closed
```
UART发送测试日志显示：
```sh
opening device:/dev/ttyUART<UART ID>
set baudrate to 1500000
set baudrate done
writing <待发送字符串>
write return <write接口返回值>
closing /dev/ttyUART<UART ID>
/dev/ttyUART<UART ID> closed
```
UART回显测试（发送已接收到的数据）日志显示：
```sh
opening device:/dev/ttyUART<UART ID>
set baudrate to 1500000
set baudrate done
closing /dev/ttyUART<UART ID>
/dev/ttyUART<UART ID> closed
```

# 注意事项
无

# FAQ
无

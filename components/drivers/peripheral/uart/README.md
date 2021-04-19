@page uart UART

# 概述
UART设备VFS驱动子系统。

## 版权说明
> Apache license v2.0

## 目录结构
```
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
示例代码参考example/uart_example.c，以运行helloworld_demo为例，具体步骤如下：

## 添加示例代码
uart组件的package.yaml中添加example
```
source_file:
  - "src/*.c"
  - "example/uart_example.c"
```

## 添加组件
helloworld_demo组件默认已依赖uart组件。

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
UART接收测试，CLI命令行输入：
```
uartr <UART ID> <测试持续时间（ms）>
```
UART发送测试，CLI命令行输入：
```
uartw <UART ID> <待发送字符串>
```
UART回显测试，CLI命令行输入：
```
uarte <UART ID> <测试持续时间（ms）>
```

## 关键日志
UART接收测试日志显示：
```
opening device:/dev/ttyUART<UART ID>
set baudrate to 1500000
set baudrate done
<显示接收到的字符串>
closing /dev/ttyUART<UART ID>
/dev/ttyUART<UART ID> closed
```
UART发送测试日志显示：
```
opening device:/dev/ttyUART<UART ID>
set baudrate to 1500000
set baudrate done
writing <待发送字符串>
write return <write接口返回值>
closing /dev/ttyUART<UART ID>
/dev/ttyUART<UART ID> closed
```
UART回显测试（发送已接收到的数据）日志显示：
```
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

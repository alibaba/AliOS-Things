@page posix posix

# 概述
POSIX (Portable Operating System Interface) 是IEEE组织为了维护应用在不同操作系统之间的兼容性而制定的标准。 主要包括API，Shell和Utility等一整套应用环境。主要应用于Unix/Linux操作系统之间， 也有其他的操作系统为了兼容Unix/Linux的应用生态而支持POSIX标准， 如Zephyr, VxWorks, QNX, Fuchsia等。
组件支持以下功能：
- pthread 线程及其属性管理
- pthread 条件变量及其属性管理
- pthread 互斥及其属性管理
- 文件系统类接口
- 环境变量管理
- POSIX信号量
- POSIX定时器
- POSIX消息队列

## 版权信息
> Apache license v2.0

## 目录结构
```sh
|-- example
|   |-- pthread_example.c #示例代码
|-- include
|   |-- dirent.h         #文件系统类接口申明
|   |-- enviro.h         #环境变量接口申明
|   |-- mqueue.h         #POSIX消息队列接口申明
|   |-- posix
|   |   |-- timer.h      #POSIX定时器接口申明
|   |-- pthread.h        #pthread接口申明
|   |-- semaphore.h      #POSIX信号量接口申明
|   |-- signal.h         #signal结构体申明
|   |-- sys
|       |-- prctl.h      #prctl接口申明
|-- package.yaml         #编译及配置文件
|-- README.md            #组件说明文档
|-- src
    |-- dirent.c         #文件系统接口实现
    |-- enviro.c         #环境变量接口实现
    |-- internal
    |   |-- pthread.h    #pthread内部定义
    |   |-- sched.h      #调度功能内部定义
    |-- mqueue.c         #POSIX消息队列实现
    |-- prctl.c          #prctl实现
    |-- pthread_attr.c   #pthread线程属性实现
    |-- pthread.c        #pthread线程实现
    |-- pthread_cond.c   #pthread条件变量及其属性的实现
    |-- pthread_mutex.c  #pthread互斥锁及其属性的实现
    |-- pthread_tsd.c    #pthread互斥锁及其属性的实现
    |-- sched.c          #调度类接口实现
    |-- semaphore.c      #POSIX信号量的实现
    |-- timer.c          #POSIX定时器及时间函数的实现
```

## 依赖组件
* osal_aos

# 常用配置
无

# API说明
本组件实现POSIX标准的API，API的使用方式遵循POSIX.1-2017标准的定义，与Unix/Linux上POSIX API使用方式一致。具体使用方法可以参考POSIX标准的官方文档[POSIX.1-2017标准](https://pubs.opengroup.org/onlinepubs/9699919799/idx/functions.html)。

# 使用示例
pthread的使用也可以参考示例代码example/pthread_example.c，以运行helloworld_demo为例，具体步骤如下：

## 添加示例代码
> posix组件的package.yaml中添加example
```sh
source_file:
  - "src/*.c"
  - "example/pthread_example.c"
```

## 添加posix组件
> helloworld_demo组件的package.yaml中添加
```sh
depends:
  - posix: master # helloworld_demo中引入posix组件
```

## 编译
```sh
cd solutions/helloworld_demo && aos make
```
其中具体单板还需要先配置环境：
```sh
aos make helloworld_demo@haas100 -c config
```

## 烧录固件
> 参考具体板子的快速开始文档。

## posix示例测试
> CLI命令行输入：
```sh
pthread_example
```

## 关键日志
> CLI日志：
```sh
new thread:0x34038460, arg:10
new thread hold the lock.
Main thread hold the lock.
retval:0x34003aa8, &ret_value:0x34003aa8
New thread:0x34038460 exited with vaule: 100
The count is 11
pthread_example test success!
```

# 注意事项
1. POSIX API的使用方式虽然与POSIX标准一致，但相同API的子功能项可能是不支持的，此时注意判断API的异常处理（返回值和errno），如API有子功能项不支持，会设置errno为ENOTSUP并返回-1，或者直接返回ENOTSUP，具体需要查看不同API的标准定义。

# FAQ
无

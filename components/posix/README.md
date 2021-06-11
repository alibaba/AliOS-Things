@page posix posix

[更正文档](https://gitee.com/alios-things/posix/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

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
```tree
posix
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
  - posix: master          # helloworld_demo中引入posix组件

```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install posix

```

上述命令执行成功后，组件源码则被下载到了./components/posix路径中。

## 步骤4 添加示例

在posix组件的package.yaml中添加[example示例代码](https://gitee.com/alios-things/posix/tree/master/example)：

```yaml
source_file:
  - "src/*.c"
  - "example/pthread_example.c"
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

pthread_example # 执行pthread示例

```

> 关键日志：
```shell

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

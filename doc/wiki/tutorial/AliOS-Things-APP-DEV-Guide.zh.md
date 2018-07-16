[EN](AliOS-Things-APP-DEV-Guide) | 中文

# 第一个应用

## 概述
本文将描述如何基于AliOS Things源码进行应用开发，涉及的内容包括：软硬件环境搭建、如何创建第一个应用程序、AliOS Things重要开发组件的使用等。

## 软硬件环境准备
AliOS Things支持多种硬件平台。开发应用的硬件环境包括开发板、串口、调试器、烧录器等。
AliOS Things支持命令行工具和IDE两种开发方式。AliOS Things开发环境的搭建请参照：[AliOS Things Development Environment Setup](AliOS-Things-Environment-Setup)

## 应用开发步骤
基于AliOS Things可以很方便地进行应用开发。命令行工具主要适用于使用Linux的开发者。IDE环境则适合在Windows下使用。
### 命令行工具进行应用开发
命令行工具下的应用的开发步骤主要包括工程目录的创建、工程Makefile编写、源码编写、工程编译、程序烧录、调试等步骤。
#### 创建工程目录
AliOS Things的应用工程一般放在“example”目录下，用户也可以根据需要在其他目录下创建应用工程的目录。
#### 添加Makefile
Makefile用于指定应用的名称、使用到的源文件、依赖的组件、全局符号等。下面是helloworld.mk样例文件的内容：
```
NAME := helloworld  ## 指定应用名称
$(NAME)_SOURCES := helloworld.c  ## 指定使用的源文件
$(NAME)_COMPONENTS += cli  ## 指定依赖的组件，本例使用cli组件
$(NAME)_DEFINES += LOCAL_MACRO ## 定义局部符号
GLOBAL_DEFINES += GLOBAL_MACRO ## 定义全局符号
```
#### 添加源码
所有的源码文件放置在应用工程目录下，开发者可以根据自行组织源码文件/目录。  
AliOS Things的应用程序入口为`application_start`, 如
```c
#include <aos/aos.h>

static void app_delayed_action(void *arg)
{
    printf("%s:%d %s\r\n", __func__, __LINE__, aos_task_name());
    aos_post_delayed_action(5000, app_delayed_action, NULL);
}

int application_start(int argc, char *argv[])
{
    aos_post_delayed_action(1000, app_delayed_action, NULL);
    aos_loop_run();
    return 0;
}
```

#### 编译运行

请参考[快速开始](Quick-Start.zh#run)。  
当然，你可以使用gdb，valgrind等工具调试上述应用。

### 在AliOS Studio中进行应用开发
AliOS Things提供了AliOS Things Studio集成开发环境，基于AliOS Things Studio进行应用开发非常方便、快捷。AliOS Things Studio提供了可供导入的应用模版，用户可以基于导入的模版进行应用开发。AliOS Things Sutdio也支持编译、烧录、调试等
#### 创建应用项目
关于如何在AliOS Things Studio中创建应用，请参考[使用AliOS Things Studio创建应用](AliOS-Things-Studio#41-%E6%96%B0%E5%BB%BA%E9%A1%B9%E7%9B%AE)。创建完项目后，用户可以在AliOS Things Studio中添加、编辑应用代码。
#### 编译、烧录和调试
AliOS Things Studio下的编译、烧录和调试步骤，可以参照：[IDE开发环境](AliOS-Things-Studio)。

## 总结
本文描述了基于AliOS Things的应用模型，介绍了软硬件开发环境的搭建、应用开发的基本步骤。以helloworld为例，展示了如何基于AliOS Things进行应用开发。本文最后，还介绍了AliOS Things提供的丰富组件和接口，以及如何利用这个组件进行应用开发。
想了解AliOS Things更详细的信息，请访问[AliOS Things Github主页](https://github.com/alibaba/AliOS-Things)。

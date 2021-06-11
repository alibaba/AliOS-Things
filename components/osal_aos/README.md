@page aos_kernel 内核编程

[更正文档](https://gitee.com/alios-things/osal_aos/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

# 概述

AliOS Things是一款支持单处理器上运行多个任务的实时操作系统。操作系统内核只包含用来控制系统资源和处理器对资源的使用的基础功能，来支持系统服务和上层应用的构建和开发。
AliOS Things操作系统内核特性如下：

   - 可抢占式/协作式任务调度
   - 多任务管理
   - 软件定时器
   - 任务间通信机制包括信号量、互斥量、队列、事件
   - 内存管理
   - 时间功能
   - 随机数
   - 系统信息
   - 系统初始化
## 版权信息
> Apache license v2.0

## 目录结构
```tree
├── include
│   ├── aos
│       └── compiler.h	  # 编译相关的宏定义文件
│       └── errno.h	    	# 兼容posix的错误号定义及相关接口的声明文件
│       └── kernel.h	    # 操作系统内核接口声明文件
│       └── list.h	    	# 链表接口声明文件
│       └── osal_debug.h	# 错误检测与处理相关的声明文件
│       └── rhino.h	      # 扩展AOS API，目前仅用于POSIX对接
├── common.c 系统通用接口实现
├── rhino.c 操作系统内核接口实现
├── package.yaml    # 编译配置文件
└── example			
    └── event_example.c   # 事件使用示例代码
    └── mem_example.c   # 内存管理功能使用示例代码
    └── mutex_example.c   # 互斥量使用示例代码
    └── queue_example.c   # 消息队列使用示例代码
    └── sem_example.c   # 信号量使用示例代码
    └── task_example.c   # 任务功能使用示例代码
    └── timer_example.c   # 定时器功能使用示例代码
```


## 依赖组件

- rhino

# 任务管理
## 概述
任务可以认为是一段独享CPU的运行程序，而应用是完成特定功能的多个任务的集合。任务管理就是为多任务环境中的每个任务分配一个上下文（context）（上下文（context）是指当任务被调度执行的所必不可少的一组数据，包括前任务的CPU指令地址（PC指针），当前任务的栈空间，当前任务的CPU寄存器状态等），在任务相继执行过程中，将切出任务的信息保存在任务上下文中，将切入任务的上下文信息恢复，使其得以执行。为维护任务上下文、状态、栈等相关信息，操作系统内核为每个任务定义了一组数据结构，即任务控制块（Task Control Block），来存放这些信息。
任务调度负责将处理器资源分配给关键任务，让其优先运行。所以系统中的每个任务需要根据关键程度分配不同的优先级，那些执行关键操作的任务被赋予高优先级，而一些非关键性任务赋予低优先级。当系统发生调度时，高优先级任务可以抢占低优先级任务的处理器资源得到调度执行。系统在无任务可调度时，就运行空闲任务，其优先级最低。
任务被创建时，需要为任务指定执行体入口地址、栈大小、优先级等信息，创建过程中内核为任务分配任务控制块（TCB）。任务栈空间可以在任务创建时由用户程序指定，也可以由内核根据用户指定大小来动态分配。操作系统内核提供基于软件的栈溢出检测方法，用户可根据需要配置或关闭该功能。
### 任务状态
任务状态是反映当前系统中任务所处的状况，操作系统内核需要维护所有任务的当前状态。AliOS Things为了充分描述任务在系统中所处的状态以及引发状态迁移的条件差异，将任务状态分为就绪状态、挂起状态、休眠状态、阻塞状态、运行状态和删除状态。当任务通过aos_task_create()创建时，任务处于挂起状态，当任务通过aos_task_del()删除时，任务处于删除状态，具体的转化过程如下图：

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01LHAt211Dqmoi53SlY_!!6000000000268-2-tps-684-451.png" style="max-width:800px;"/>
</div>

（1）阻塞状态是指因等待资源而处于等待状态，如调用aos_mutex_lock()获取互斥量时互斥量已经被锁定、调用aos_queue_recv()获取队列数据时队列为空、调用aos_sem_wait()等待信号量时信号量计数为0、调用aos_evnet_get()获取事件时，事件还未发生；
（2）挂起状态是因任务被其他或自身调用挂起函数aos_task_suspend()后，将无条件地停止运行，被挂起的任务只能通过其他任务调用恢复函数aos_task_resume()使其恢复到就绪状态；
（3）休眠状态是因任务在调用任务休眠函数aos_msleep()后，进入一种延迟执行的状态，直到休眠时间到达任务被重新调度恢复运行。
（4）删除状态是因任务运行完成调用任务退出函数aos_task_exit()或被调用任务删除函数aos_task_del()时被设置的一种状态。
（5）就绪状态是在任务被创建或任务解除阻塞或延迟到期时，任务被置为就绪状态。只有当任务处于就绪状态时，才能被系统调度进入运行状态。
（6）运行状态是获取处理器执行权的就绪任务所处的状态，单处理器系统，任意时刻只有一个任务可以运行。
AliOS Things允许任务处于组合状态，如阻塞挂起状态：任务在阻塞状态下，被其他任务挂起，则进入阻塞挂起状态。该状态下，若任务被恢复则保持阻塞状态；若任务解除阻塞则保持挂起状态。
用户可通过tasklist命令查看任务状态，任务状态描述符号和含义如下表： 

| **状态符号** | **描述** |
| --- | --- |
| RDY | 任务已在就绪队列或已被调度运行，处于就绪状态或运行状态 |
| PEND | 任务因等待资源或事件的发生而处于阻塞状态 |
| SUS | 任务因被其他或自身调用挂起函数aos_task_suspend()后所处的挂起状态 |
| SLP | 任务处于休眠状态 |
| PEND_SUS | 任务在阻塞状态下，被其他任务挂起，处于阻塞挂起状态 |
| SLP_SUS | 任务在休眠状态下，被其他任务挂起，处于休眠挂起状态 |
| DELETED | 任务处于删除状态 |

### 任务调度
任务调度是为多任务环境中的就绪任务分配处理器资源。AliOS Things操作系统内核支持两种调度策略：

- 基于优先级的抢占式调度

该调度策略下，每个任务优先级都维护了一个FIFO模式的就绪队列（ready queue），里面包含了当前所有可运行的任务列表，此列表中的任务都处于就绪状态，当处理器可用时，最高优先级的就绪队列的第一个任务得到处理器被执行。当有一个高优先级任务进入就绪队列，正在运行的低优先级任务会立即被唤出，将处理器执行权交给高优先级任务。此种调度机制存在一个潜在问题，如果存在多个优先级相同的任务，其中一个任务强占处理器，则其他同等优先级的任务将无法被执行。时间片轮转调度可以避免这个问题。

- 基于时间片的轮转调度

时间片轮转调度使用时间片控制每个任务的执行时间，同等优先级的任务依次获得处理器被调度执行，每个任务可以运行的时间片是固定的，当任务的时间片用完后，该任务被放在对应优先级就绪队列的队尾，然后调度就绪队列第一个位置上的任务来执行。
### 优先级分配
AliOS Things操作系统内核允许的任务优先级分配范围默认为0～62，值越大表示优先级越低，其中空闲任务的优先级是61，即系统最低优先级。
### 任务栈保护
任务栈保护采用栈统计的方法在任务切换时，对当前任务栈的使用情况进行统计，当检测到任务栈使用量超过了预先分配的空间，则抛出异常。
### 空闲任务
空闲任务是一个无限循环函数，在进入循环前或循环体内允许用户配置钩子函数。
### 任务管理函数
AliOS Things操作系统内核提供了任务创建、任务删除、任务延迟、获取任务名称等任务相关的服务函数，提供给应用程序调用，具体描述如下：

| **函数名** | **描述** |
| --- | --- |
| aos_task_create() | 任务创建函数（推荐） |
| aos_task_new() | 任务创建函数（兼容3.1） |
| aos_task_new_ext() | 任务创建函数（兼容3.1） |
| aos_task_exit() | 任务退出函数 |
| aos_task_delete() | 任务删除函数 |
| aos_task_resume() | 任务恢复函数 |
| aos_task_suspend() | 任务挂起函数 |
| aos_task_yield() | 任务让出CPU函数 |
| aos_task_self() | 获取当前任务的具柄 |
| aos_task_name_get() | 获取任务名称 |

## 常用配置
> 任务优先级最大值: 默认62, 最大不能超过256，如需修改，在yaml中修改RHINO_CONFIG_PRI_MAX配置

```yaml
def_config:
  RHINO_CONFIG_PRI_MAX: 127
```


> 任务栈溢出检测: 默认关闭, 如需修改，在yaml中修改RHINO_CONFIG_TASK_STACK_OVF_CHECK配置

```yaml
def_config:
  RHINO_CONFIG_TASK_STACK_OVF_CHECK: 1
```


> 空闲任务栈大小: 默认100Bytes, 如需修改，在yaml中修改RHINO_CONFIG_IDLE_TASK_STACK_SIZE配置

```yaml
def_config:
  RHINO_CONFIG_IDLE_TASK_STACK_SIZE: 1024
```


> 时间片轮转调度策略: 默认使能, 如需修改，在yaml中修改RHINO_CONFIG_SCHED_RR配置

```yaml
def_config:
  RHINO_CONFIG_SCHED_RR: 0
```


> 完全公平调度策略: 默认关闭, 如需修改，在yaml中修改RHINO_CONFIG_SCHED_CFS配置

```yaml
def_config:
  RHINO_CONFIG_SCHED_CFS: 1
```
## API说明

- 参考 [aos_kernel_task](https://g.alicdn.com/alios-things-3.3/doc/group__aos__kernel__task.html)

## 使用示例

示例代码参考[example/task_example.c](https://gitee.com/alios-things/osal_aos/blob/master/example/task_example.c)，该示例使用任务管理函数来控制任务的执行状态，具体场景为任务2因等待某个信号量进入阻塞状态，而此时被任务1将其挂起，则任务2仍然是处于阻塞状态，如果在此过程中等到信号量，则任务2会解除阻塞进入挂起状态；如果未等到信号量，则任务2恢复状态后仍然处于阻塞状态。
示例说明如下：

1. 在t0时刻，任务task1、task2是通过aos_task_create()函数调用被创建，之后task1进入就绪状态，而task2处于挂起状态。
1. Task1得到运行后，在t1时刻调用aos_task_resume()将task2恢复，task2进入就绪状态，之后task1通过调用aos_msleep()进入休眠状态，task2因为task1休眠而获得CPU执行权，task2运行后因等待信号量进入阻塞状态。
1. Task1在t2时刻因延迟到期得到运行，并调用aos_task_suspend()将task2挂起，task2此时的状态为阻塞挂起。之后task1通过调用aos_msleep()进入休眠状态。
1. Task2在t3时刻因延迟到期得到运行，并调用aos_task_resume()将task2恢复，此时task2的状态为阻塞状态。之后task1通过调用aos_msleep()进入休眠状态。
1. Task1在t4时刻因延迟到期得到运行，并调用aos_sem_signal()释放信号量，这时task2因等到信号量而进入就绪状态。待到task1再次进入休眠转改后task2得到运行，进入运行状态。

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01WaKfUM1fXNGrOho4q_!!6000000004016-2-tps-859-291.png" style="max-width:800px;"/>
</div>

该示例可配置到helloworld_demo案例中运行，相关代码的下载、编译和固件烧录均依赖AliOS Things配套的开发工具 **alios-studio** ，所以首先需要参考[《aos-studio使用说明之搭建开发环境》](https://g.alicdn.com/alios-things-3.3/doc/setup_env.html)，下载安装 **alios-studio** 。
待开发环境搭建完成后，可以按照以下步骤进行示例的测试。

### 步骤1 创建或打开工程

**打开已有工程**

如果用于测试的案例工程已存在，可参考[《aos-studio使用说明之打开工程》](https://g.alicdn.com/alios-things-3.3/doc/open_project.html)打开已有工程。

**创建新的工程**

组件的示例代码可以通过编译链接到AliOS Things的任意案例（solution）来运行，这里选择helloworld_demo案例。helloworld_demo案例相关的源代码下载可参考[《aos-studio使用说明之创建工程》](https://g.alicdn.com/alios-things-3.3/doc/create_project.html)。

### 步骤2 添加组件

> 案例下载完成后，需要在helloworld_demo的package.yaml中添加

```yaml
depends:
  - osal_aos: master # helloworld_demo中引入osal_aos组件
```

### 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install osal_aos

```

上述命令执行成功后，组件源码则被下载到了./components/osal_aos路径中。

### 步骤4 添加示例

> 在osal_aos组件的package.yaml中添加[example示例代码](https://gitee.com/alios-things/osal_aos/tree/master/example)：

```yaml
depends:
  - rhino: master
  - cli: master # 添加cli依赖
source_file:
  - "*.c"
  - "example/task_example.c" # 添加 task_example.c
```

### 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

### 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

### 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

### 步骤8 测试示例

> CLI命令行输入：

```shell
task_example
```

> 关键日志：

```shell
[aos_task_example]task1 is running!
[aos_task_example]task1 resume task2!
[aos_task_example]task1 start to sleep and release CPU!
[aos_task_example]task2 is running!
[aos_task_example]task1 suspend task2!
[aos_task_example]task1 start to sleep and release CPU!
[aos_task_example]task1 resume task2 again!
[aos_task_example]task1 start to sleep and release CPU!
[aos_task_example]task1 signal a semphone!
[aos_task_example]task1 start to sleep and release CPU!
[aos_task_example]task2 get semphone and is running!
```


## 注意事项

- 时间片轮转调度也是基于优先级策略的，如果有一个高优先级任务就绪了，无论当前任务的时间片是否用完，处理器都会立即去执行高优先级任务，当被打断的任务恢复执行时，它将继续执行剩下的时间片。
- tasklist查看运行任务的状态标识符为RDY，内核并没有为运行状态定义特定的标识符。

## FAQ

- Q1： 如何避免栈溢出？

答：任务栈必须满足应用程序对函数嵌套调用所需的空间，用户可以通过tasklist查看栈的使用情况，根据需              要调整栈大小，这样可以在防止栈溢出的同时，也可避免栈空间分配过大造成浪费。

- Q2： aos_task_yield()和aos_task_suspend()都能够使任务退出运行状态，使用上有什么差异吗？

答：aos_task_yield()仅仅是让出CPU，它仍然处于就绪状态，因为该操作只是将任务放置到就绪队列的队              尾，若无更高优先级任务在就绪队列中，它就可以被调度执行。而aos_task_suspend()是将任务挂起停              止运行，任务会从就绪队列中去除，只有当任务被其他任务调用aos_task_resume()才能恢复。

# 软件定时器
## 概述
AliOS Things操作系统内核使用tick作为时间片轮转调度以及延迟操作的时间度量单位，tick是实现定时触发功能的基础。tick计数发生在每次时钟中断处理的过程，时钟中断是定时产生的，系统在默认情况下为1ms触发一次，即一个tick代表1ms，用户可根据应用需要调整该时间。
软件定时器是用来在指定时间或者触发一次或多次某个功能函数的调用。这种由定时器来执行的函数叫做定时器回调函数，定时器回调函数以及触发时间由应用程序来设定。当定时器的触发时间到来，则定时器的回调函数会被执行。定时器支持两种工作模式：单次模式和周期性模式。定时器在创建时，用户可通过option参数来制定该定时器的工作模式是单次还是周期性的，若option设置了AOS_TIMER_REPEAT则为周期性的，否则为单次。
### 单次模式
定时器会从应用程序设置的初始时间开始，以tick为计时单位进行倒计时，当计数值减为0时调用回调函数执行。回调函数执行完毕，则定时器停止。
### 周期性模式
周期定时器在时间到期执行完回调函数后，重新开始计时，直到下次时间到期，再次执行回调函数，然后一直循环下去。
### 定时器任务
为了在定时器创建、删除、启动、停止以及参数并更的过程中，避免锁操作，系统采用一个定时器任务来处理这些操作。当应用程序调用定时器管理接口时，将接口调用命令发送到一个消息队列，定时器任务从消息队列中读取消息并处理该命令。
定时器任务除了处理命令外，还对当前已运行的定时器进行实时调度。所有正在运行的定时器会被挂接到g_timer_head链表，定时器任务循环的从g_timer_head链表中取出时间最近一次的定时器，通过当前tick计数和该定时器的超时tick数来判断定时时间是否到，如果该定时器触发时间已到则立即执行其处理函数，否则代表最近一次的定时器时间尚未到来，则在此时差，继续从命令buffer中收取消息，直到时间差到来后，再立即执行定时器处理函数。
上述流程在g_timer_head中有待处理定时器时才会进入，如果没有待处理定时器，在定时器任务将只会进入定时器管理循环中。
### 定时器管理函数
| **函数名** | **描述** |
| --- | --- |
| aos_timer_create() | 定时器创建函数（推荐） |
| aos_timer_new() | 定时器创建函数（兼容3.1） |
| aos_timer_new_ext() | 定时器创建函数 |
| aos_timer_free() | 定时器删除函数 |
| aos_timer_start() | 定时器启动函数 |
| aos_timer_stop() | 定时器停止函数 |
| aos_timer_change() | 定时器初始时长和周期间隔参数变更函数 |
| aos_timer_change_once() | 定时器初始时长参数变更函数 |
| aos_timer_gettime() | 获取定时器时间参数函数 |

## 常用配置
> 软件定时器功能: 默认使能，如需修改，在yaml中修改RHINO_CONFIG_TIMER配置

```yaml
def_config:
  RHINO_CONFIG_TIMER: 0
```


软件定时器任务栈: 默认200Bytes，如需修改，在yaml中修改RHINO_CONFIG_TIMER_TASK_STACK_SIZE配置
```yaml
def_config:
  RHINO_CONFIG_TIMER_TASK_STACK_SIZE: 512
```


软件定时器任务优先级: 默认5，如需修改，在yaml中修改RHINO_CONFIG_TIMER_TASK_PRI配置
```yaml
def_config:
  RHINO_CONFIG_TIMER_TASK_PRI: 5
```


定时器消息队列消息数: 默认20，如需修改，在yaml中修改RHINO_CONFIG_TIMER_MSG_NUM配置
```yaml
def_config:
  RHINO_CONFIG_TIMER_MSG_NUM: 24
```


每秒tick数: 默认100，如需修改，在yaml中修改RHINO_CONFIG_TICKS_PER_SECOND配置
```yaml
def_config:
  RHINO_CONFIG_TICKS_PER_SECOND: 1000
```


时间片: 默认50个tick，如需修改，在yaml中修改RHINO_CONFIG_TIME_SLICE_DEFAULT配置
```yaml
def_config:
  RHINO_CONFIG_TIME_SLICE_DEFAULT: 100
```
## API说明
- 参考 [aos_kernel_timer](https://g.alicdn.com/alios-things-3.3/doc/group__aos__kernel__timer.html)
## 使用示例

- 示例代码参考[example/timer_example.c](https://gitee.com/alios-things/osal_aos/blob/master/example/timer_example.c)，该示例使用定时器管理函数来控制定时器的执行，具体场景为创建一个周期性定时器，定时调用回调函数执行，停止定时器该变定时器的时间参数，则定时器按照修改后的时间间隔定时调用回调函数执行。
- 示例说明如下：
1. t0时刻，测试任务调用aos_timer_create()创建一个周期性的定时器，周期间隔为1秒，回调函数为timer1_func。然后测试任务调用aos_sleep()进入休眠状态。
1. t1时刻，相对t0过去1秒，定时器到期，回调函数timer1_func被执行。该过程重复10次。
1. tn时刻，测试任务休眠到期，调用aos_timer_stop()停止定时器。然后调用aos_timer_change()接口修改周期间隔为2秒。
1. tn+1时刻，相对tn过去2秒，定时器到期，回调函数timer1_func被执行。该过程重复5次。

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01eCw5Hz1JIKiZdxYMI_!!6000000001005-2-tps-689-192.png"  style="max-width:800px;" />
</div>

- 该示例可配置到helloworld_demo中运行，相关代码的下载、编译和固件烧录均依赖AliOS Things配套的开发工具 **alios-studio** ，所以首先需要参考[《aos-studio使用说明之搭建开发环境》](https://g.alicdn.com/alios-things-3.3/doc/setup_env.html)，下载安装 **alios-studio** 。
待开发环境搭建完成后，可以按照以下步骤进行示例的测试。

### 步骤1 创建或打开工程

**打开已有工程**

如果用于测试的案例工程已存在，可参考[《aos-studio使用说明之打开工程》](https://g.alicdn.com/alios-things-3.3/doc/open_project.html)打开已有工程。

**创建新的工程**

组件的示例代码可以通过编译链接到AliOS Things的任意案例（solution）来运行，这里选择helloworld_demo案例。helloworld_demo案例相关的源代码下载可参考[《aos-studio使用说明之创建工程》](https://g.alicdn.com/alios-things-3.3/doc/create_project.html)。

### 步骤2 添加组件
> helloworld_demo组件的package.yaml中添加

```yaml
depends:
  - osal_aos: master # helloworld_demo中引入osal_aos组件
```

### 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install osal_aos

```

上述命令执行成功后，组件源码则被下载到了./components/osal_aos路径中。

### 步骤4 添加示例
> osal_aos组件的package.yaml中添加[example示例代码](https://gitee.com/alios-things/osal_aos/tree/master/example)：

```yaml
depends:
  - rhino: master
  - cli: master # 添加cli依赖
source_file:
  - "*.c"
  - "example/timer_example.c" # 添加 timer_example.c
```

### 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

### 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

### 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

### 步骤8 测试示例
> CLI命令行输入：

```shell
timer_example
```

> 关键日志：

```shell
[aos_timer_example]timer expires 10 times
[aos_timer_example]timer expires 5 times
```
## 注意事项

- tick是内核调度和延迟操作的基础，所以RHINO_CONFIG_TICKS_PER_SECOND参数必须配置，而定时器是基于tick实现的软件定时器功能，可以通过配置RHINO_CONFIG_TIMER参数来使能或关闭该功能。
## FAQ

- Q1： 定时器回调函数的执行上下文是在哪里？

答：定时器回调函数是在定时器任务中执行的，所以定时器回调函数的执行上下文为定时器任务，当回调函              数所需栈空间超过定时器任务栈大小时，用户需要通过配置                                                                       RHINO_CONFIG_TIMER_TASK_STACK_SIZE参数来增加栈大小。
# 内存管理
## 概述
AliOS Things操作系统内核通过内存管理实现应用程序可以调用标准C库的malloc和free接口来动态的分配和释放内存。
### 内存管理函数
| **函数名** | **描述** |
| --- | --- |
| aos_malloc() | 从系统heap分配内存给用户 |
| aos_zalloc() | 从系统heap分配内存给用户，并且将分配的内存初始化为0 |
| aos_calloc() | 从系统heap分配内存给用户，并且将分配的内存初始化为0 |
| aos_realloc() | 重新调整之前调用 `aos_malloc(aos_calloc、aos_zalloc)`所分配的内存块的大小 |
| aos_free() | 内存释放函数 |

### 内存分配算法
AliOS Things内存管理采用类buddy伙伴算法，以及blk快速小内存申请算法相结合的策略。

- buddy算法：

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01N1qqmG1md3UkY5gRB_!!6000000004976-2-tps-841-393.png"  style="max-width:800px;" />
</div>

Buddy算法申请的内存最小为8字节对齐
可申请的最大长度除了受堆空间总大小限制外还可通过编译宏配置
通过32 bit的位图结构来挂载对应的空闲链表bit n代表内存块对应的type，其下面挂载了长度范围 (2^(n- 1 + off) ~ 2^(n + off) - 1)的空闲块，其中off值主要是考虑利用了原本较小长度块的低bit。
内存申请时，优先从对应type的空闲链表中申请；如果没有空闲，则从 type+1 的位置申请内存，并将多余的空闲内存块做拆分并重新放入对应的空闲链表位置；每个内存块维测信息，包括如魔术字、多级申请调用栈、是否空闲等信息，便于内存踩踏、泄漏、重复释放等问题的定位。
内存释放时，考虑前后的内存块是否也空闲，如果是，则进行内存块合并，并插入相应type的空闲链表位置。
**优点：**
能够满足绝大多数场景的内存申请需求；
可以申请最大范围内任意长度的内存；
内存碎片整理已经相当出色；
有充分的维测信息，便于问题定位。
**缺点：**
内存维测信息较多，有一定的空间浪费；
内存不停地拆分组合，一定程度上影响效率；
长期运行过多的小内存会出现碎片，会隔断大内存空间。


- blk小内存快速申请：

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01mHjJZ11viOeeBd76S_!!6000000006206-2-tps-835-477.png"  style="max-width:800px;" />
</div>

blk小内存算法申请的内存最小为4字节对齐
小于RHINO_CONFIG_MM_BLK_SIZE大小的内存块会优先从blk算法中申请不够的再从通过buddy申请
内存申请时，优先从对应type的空闲链表中申请；如果没有空闲，则先考虑从空闲资源池拉取一段空闲资源补充；如果空闲资源不足，则从 type+1 的位置申请内存（以此类推）申请过程不涉及拆分；
内存释放时，直接挂在到对应type的空闲链表内，释放不涉及合并。
**优点：**
申请效率高；
避免过多小内存碎片。
**缺点：**
内存维测信息少，出现问题较难定位；
申请的内存大小有限制，一般为1K以下。
## 常用配置
内存的常用配置都在各个单板的k_config.h中，如果不需修改，则会使用k_default_config.h里面的默认配置。当然由于其本身是宏定义，也可在board对应的yaml中定义。
> 打开关闭内存模块功能

```yaml
def_config:
  RHINO_CONFIG_MM_TLF: 1
```


打开关闭buddy内存模块的维测功能
```yaml
def_config:
  RHINO_CONFIG_MM_DEBUG: 1
```


内存维测打开时，记录内存申请时的多少级调用栈
```yaml
def_config:
  RHINO_CONFIG_MM_TRACE_LVL: 8
```


Buddy算法最大支持申请的内存大小bit位
实际大小换算为 1ul << RHINO_CONFIG_MM_MAXMSIZEBIT
```yaml
def_config:
  RHINO_CONFIG_MM_MAXMSIZEBIT: 28
```


打开关闭固定长度小内存块快速申请
```yaml
def_config:
  RHINO_CONFIG_MM_BLK: 1
```


固定长度小内存块总空间大小（byte）
```yaml
def_config:
  RHINO_CONFIG_MM_TLF_BLK_SIZE: 8192
```


设定从blk小内存申请的阈值（byte）即 
```yaml
def_config:
  RHINO_CONFIG_MM_BLK_SIZE: 256
```


注: blk目前最大支持1K以下的小内存块如果需要调整可修改内部配置 MM_BLK_SLICE_BIT一般无需修改
## API说明

- 参考 [aos_kernel_memory](https://g.alicdn.com/alios-things-3.3/doc/group__aos__kernel__memory.html)

## 使用示例
该示例可配置到helloworld_demo中运行，相关代码的下载、编译和固件烧录均依赖AliOS Things配套的开发工具 **alios-studio** ，所以首先需要参考[《aos-studio使用说明之搭建开发环境》](https://g.alicdn.com/alios-things-3.3/doc/setup_env.html)，下载安装 **alios-studio** 。
待开发环境搭建完成后，可以按照以下步骤进行示例的测试。

### 步骤1 创建或打开工程

**打开已有工程**

如果用于测试的案例工程已存在，可参考[《aos-studio使用说明之打开工程》](https://g.alicdn.com/alios-things-3.3/doc/open_project.html)打开已有工程。

**创建新的工程**

组件的示例代码可以通过编译链接到AliOS Things的任意案例（solution）来运行，这里选择helloworld_demo案例。helloworld_demo案例相关的源代码下载可参考[《aos-studio使用说明之创建工程》](https://g.alicdn.com/alios-things-3.3/doc/create_project.html)。

### 步骤2 添加组件

> helloworld_demo组件的package.yaml中添加

```c
depends:
  - osal_aos: master # helloworld_demo中引入osal_aos组件
```

### 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install osal_aos

```

上述命令执行成功后，组件源码则被下载到了./components/osal_aos路径中。

### 步骤4 添加示例

> osal_aos组件的package.yaml中添加[example示例代码](https://gitee.com/alios-things/osal_aos/tree/master/example)：

```yaml
depends:
  - rhino: master
  - cli: master # 添加cli依赖
source_file:
  - "*.c"
  - "example/mem_example.c" # 添加 mem_example.c
```

### 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

### 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

### 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

### 步骤8 测试示例
> CLI命令行输入：

```shell
mem_example
```

> 关键日志：

```shell
[aos_mem_example]aos_malloc success!
[aos_mem_example]aos_zalloc success!
[aos_mem_example]aos_calloc success!
[aos_mem_example]aos_realloc success!
```
## 注意事项

- 实际内存申请释放应统一通过malloc/free接口，其内部已经对接了aos_malloc/aos_free接口
- 内存申请使用的堆统一由g_mm_region数据结构初始化（传入堆起点和长度），并在aos_init内核初始化接口中默认完成了内存模块的初始化
- AliOS Things针对内存模块提供了完善的内存解析工具，用来分析内存踩踏、内存泄漏、重复释放等问题。具体可参考：[https://blog.csdn.net/HaaSTech/article/details/112246611](https://blog.csdn.net/HaaSTech/article/details/112246611)
## FAQ

- Q1： 内存模块的维测功能如何打开？

答：通过打开宏RHINO_CONFIG_MM_DEBUG即可打开内存管理模块的维测信息；其包含内存头魔术字、内存申请的调用栈以及当前内存空闲情况等等。
其中blk算法不包含维测信息。

- Q2：如何查看当前内存的使用情况？

答：通过在cli中输入 dumpsys_mm_info ,将所有内存块的使用情况，以及整体的占用空闲情况输出。
# 信号量
## 概述
信号量是多任务系统中实现任务间同步，并且协调多任务对共享资源访问的一种互斥机制。信号量允许有多个使用者，所以采用计数值来表示可用的资源数，当请求一个信号量时，该计数值减1，若此时计数值大于等于0，表示当前有可用的信号量，则任务获得信号量，可以访问资源，若此时计数值为负数，则任务进入阻塞状态，释放处理器资源。当获取信号量的任务执行完操作，释放信号量时，则将当前计数值加1，如果当前存在等待该资源的任务，则任务被唤醒而获得该信号量。
### 超时时间
信号量获取可设置超时时间，如果任务在超时时间到期后仍未等待信号量，则任务解除阻塞进入就绪状态。
### 多任务同步
多个任务可以等待同一个信号量，若信号量可用或信号量被释放，通常情况下，系统会将阻塞在该信号量上优先级最高的任务置于就绪状态，提供了面向所有阻塞任务的信号量释放机制，可以将阻塞在该信号量上的所有任务都置为就绪状态。
### 信号量功能函数
| **函数名** | **描述** |
| --- | --- |
| aos_sem_create() | 信号量创建函数（推荐），需指定计数值 |
| aos_sem_new() | 信号量创建函数（兼容3.1），需指定计数值 |
| aos_sem_free() | 信号量删除函数 |
| aos_sem_wait() | 信号量获取函数，可以指定超时时间 |
| aos_sem_signal() | 信号量释放函数，只唤醒阻塞在此信号量上的最高优先级任务 |
| aos_sem_signal_all() | 信号量释放函数，唤醒阻塞在此信号量上的所有任务 |
| aos_sem_is_valid() | 判断信号量具柄是否合法函数 |

## 常用配置
> 软件定时器功能: 默认使能，如需修改，在yaml中修改RHINO_CONFIG_SEM配置

```yaml
def_config:
  RHINO_CONFIG_SEM: 0
```


## API说明
- 参考 [aos_kernel_sem](https://g.alicdn.com/alios-things-3.3/doc/group__aos__kernel__sem.html)
## 使用示例

- 示例代码参考[example/sem_example.c](https://gitee.com/alios-things/osal_aos/blob/master/example/sem_example.c)，该示例使用信号量实现多任务同步，具体场景为创建一个高优先级任务A，一个低优先级任务B，任务A和任务B同时等待同一信号量，此时测试任务T调用aos_sem_signal()释放信号量，任务A首先获得信号量，任务A操作完成后释放一次信号量，此时任务B获取信号量得到运行。
- 示例说明如下：
1. t0时刻，任务T调用aos_sem_create()创建一信号量，初始计数值为0。任务T然后调用aos_task_create()创建任务A和任务B，任务A优先级为30，任务B优先级为31。任务A和任务B运行后因等待信号量而阻塞。
1. t1时刻，任务T调用aos_sem_signal()释放信号量，任务A获得信号量。
1. t2时刻，任务A调用aos_sem_signal()释放信号量，任务B获得信号量。

<div align=left display=flex>
    <img src="hhttps://img.alicdn.com/imgextra/i3/O1CN01NW351a1hXUbVEKizH_!!6000000004287-2-tps-516-261.png"  style="max-width:800px;" />
</div>

- 该示例可配置到helloworld_demo中运行，相关代码的下载、编译和固件烧录均依赖AliOS Things配套的开发工具 **alios-studio** ，所以首先需要参考[《aos-studio使用说明之搭建开发环境》](https://g.alicdn.com/alios-things-3.3/doc/setup_env.html)，下载安装 **alios-studio** 。
待开发环境搭建完成后，可以按照以下步骤进行示例的测试。

### 步骤1 创建或打开工程

**打开已有工程**

如果用于测试的案例工程已存在，可参考[《aos-studio使用说明之打开工程》](https://g.alicdn.com/alios-things-3.3/doc/open_project.html)打开已有工程。

**创建新的工程**

组件的示例代码可以通过编译链接到AliOS Things的任意案例（solution）来运行，这里选择helloworld_demo案例。helloworld_demo案例相关的源代码下载可参考[《aos-studio使用说明之创建工程》](https://g.alicdn.com/alios-things-3.3/doc/create_project.html)。

### 步骤2 添加组件

案例下载完成后，需要在helloworld_demo组件的package.yaml中添加对组件的依赖：

```yaml
depends:
  - osal_aos: master # helloworld_demo中引入osal_aos组件
```
### 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install osal_aos

```

上述命令执行成功后，组件源码则被下载到了./components/osal_aos路径中。

### 步骤4 添加示例

> 在osal_aos组件的package.yaml中添加[example示例代码](https://gitee.com/alios-things/osal_aos/tree/master/example)：

```yaml
depends:
  - rhino: master
  - cli: master # 添加cli依赖
source_file:
  - "*.c"
  - "example/sem_example.c" # 添加 sem_example.c
```
### 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

### 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

### 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

### 步骤8 测试示例
> CLI命令行输入：

```shell
sem_example
```
> 关键日志：

```sh
[aos_sem_example][0x34035840]field1=1449208480, field2=1449208480, field3=1449208480, field4=1449208480
[aos_sem_example][0x34035d10]field1=670588782, field2=670588782, field3=670588782, field4=670588782
[aos_sem_example][0x34035840]field1=1648917010, field2=1648917010, field3=1648917010, field4=1648917010
[aos_sem_example][0x34035d10]field1=1949034929, field2=1949034929, field3=1949034929, field4=1949034929
[aos_sem_example][0x34035840]field1=672861528, field2=672861528, field3=672861528, field4=672861528
[aos_sem_example][0x34035d10]field1=820049120, field2=820049120, field3=820049120, field4=820049120
```
## 注意事项

- 中断禁止信号量获取检测

信号量的获取接口在中断上下文调用很容易发生死锁问题。当被打断的上下文和打断的中断上下文要获取同一个信号量时，会发生互相等待的情况。有些内核将这种判断处理交由上层软件进行判断和使用，本内核会在take信号量检测，如果是中断上下文，则直接返回失败。

- 占用信号量非等待、永远等待、延时使用区别

应用程序在获取信号量时，需要按照实际的需求，来安排信号量获取策略。aos_sem_take传入延时timeout为0，获取不到信号量会立即报失败；timeout为AOS_WAIT_FOREVER时，会永远在此等待，直到获取到信号量，可能会造成该任务无法继续运行；其他值标识最大延迟的时间上限，达到上限时，即使未获取到信号量，任务也会被唤醒，并返回错误。
## FAQ

- Q1： 调用aos_sem_task()接口无限期的等待信号量，timeout参数怎么设置？

     答：将timeout赋值为AOS_WAIT_FOREVER。
# 互斥量
## 概述
互斥量的获取是完全互斥的，即同一时刻，互斥量只能被一个任务获取。而信号量按照起始的计数值的配置，可以存在多个任务获取同一信号量的情况，直到计数值减为0，则后续任务无法再获取信号量，当信号量的计数初值设置为1，同样有互斥的效果。但信号量无法避免优先级反转问题。
### 优先级反转
优先级反转是一种不希望发生的任务调度状态，该状态下，一个高优先级任务间接的被一个低优先级任务所抢占，使得两个任务的相对优先级反转了。当高、中、低三个优先级任务同时访问使用信号量互斥资源时，可能会引起优先级反转。当高优先级的任务需要的信号量被低优先级任务占用时，处理器资源会调度给低优先级任务。此时如果低优先级需要获取的另一个信号量被中优先级的pend任务所占用，那么低优先级的任务则需要等待中优先级的任务事件到来，并释放信号量，则就出现了 高、中优先级的任务并不是等待一个信号量，但是中优先级任务先运行的现象。如下图：

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01zD7nbO1kzNZGRDjRR_!!6000000004754-2-tps-410-240.png"  style="max-width:800px;" />
</div>

任务H具有高优先级，任务M具有中等优先级，任务L具有低优先级，在t0时刻任务H还未运行，任务M因等待信号量2而阻塞，低优先级任务L得到调度占用信号量1；t1时刻，任务H运行并抢占任务L，任务L占用信号量还未释放；t2时刻，任务H阻塞于信号量1，任务L得以继续运行；t3时刻，任务L等待信号量2而阻塞，t4时刻，信号量2被释放，任务M得到运行，此时任务H虽然具有高优先级，但需先等待任务M释放信号量2让任务L运行，并且任务L释放信号量1后才能运行，这种情况即出现了优先级反转。
### 优先级继承
互斥量可以解决优先级反转问题，高优先级的任务获取互斥量时，如果该互斥量被某低优先级的任务占用， 会动态提升该低优先级任务的优先级等于高优先级，并且将该优先级值依次传递给该低优先级任务依赖的互斥量关联的任务，以此递归下去。当某任务释放互斥量时，会查找该任务的基础优先级，以及获取到的互斥量所阻塞的最高优先级的任务的优先级，取两者中高的优先级来重新设定此任务的优先级。总的原则就是，高优先级任务被互斥量阻塞时，会将占用该互斥量的低优先级任务临时提高；互斥量释放时，相应任务的优先级需要恢复。如下图：

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN0131uJpc1veH5gWvL3g_!!6000000006197-2-tps-481-250.png"  style="max-width:800px;" />
</div>

任务H具有高优先级，任务M具有中等优先级，任务L具有低优先级，在t0时刻任务H还未运行，任务M获取互斥量2而阻塞，低优先级任务L得到调度获得互斥量1；t1时刻taskH抢占taskL，但因无法获得互斥量1而阻塞，此时taskL的优先级被提升至与taskH一样高，并继续运行；t2时刻taskL因无法获得互斥量2而阻塞，t3时刻互斥量2被释放，taskL因比taskM优先级高获得互斥量2得到运行；在t4时刻，taskL释放互斥量1，并将优先级恢复到之前状态，taskH因获得互斥量1得到运行，该机制消除了优先级反转的发生。
### 超时时间
互斥量获取可设置超时时间，如果任务在超时时间到期后仍未获得互斥量，则任务解除阻塞进入就绪状态。
### 互斥量功能函数
| **函数名** | **描述** |
| --- | --- |
| aos_mutex_create() | 互斥量创建函数（推荐） |
| aos_mutex_new() | 互斥量创建函数（兼容3.1） |
| aos_mutex_free() | 互斥量删除函数 |
| aos_mutex_lock() | 互斥量获取函数 |
| aos_mutex_unlock() | 互斥量释放函数 |
| aos_mutex_is_valid() | 判断互斥量具柄是否合法函数 |

## 常用配置
> 互斥量优先级继承: 默认关闭，如需修改，在yaml中修改RHINO_CONFIG_MUTEX_INHERIT配置

```yaml
def_config:
  RHINO_CONFIG_MUTEX_INHERIT: 1
```


## API说明
- 参考 [aos_kernel_mutex](https://g.alicdn.com/alios-things-3.3/doc/group__aos__kernel__mutex.html)
## 使用示例

- 示例代码参考[example/mutex_example.c](https://gitee.com/alios-things/osal_aos/blob/master/example/mutex_example.c)，该示例使用互斥量实现共享资源的互斥访问，具体场景为创建任务A和认为B，以及一互斥量。任务A和任务B使用互斥量同时访问共享数据区，访问共享数据区时使用互斥量做保护。
- 示例说明如下：
1. t0时刻，任务T调用aos_mutex_create()创建一互斥量。任务T然后调用aos_task_create()创建任务A和任务B。任务A得到运行，并获取互斥量对数据区record_status进行读写操作。
1. t1时刻，任务A因时间片耗尽，让出CPU，任务B得到运行。
1. t2时刻，任务B因无法获得互斥量，进入阻塞状态，任务A得到运行。
1. t3时刻，任务A对数据区record_status的操作完成，释放互斥量，任务B获得互斥量开始对数据区record_status进行读写操作。

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01UKTCDa1kGx9OVO9Yi_!!6000000004657-2-tps-409-251.png"  style="max-width:800px;"/>
</div>

- 该示例可配置到helloworld_demo中运行，相关代码的下载、编译和固件烧录均依赖AliOS Things配套的开发工具 **alios-studio** ，所以首先需要参考[《aos-studio使用说明之搭建开发环境》](https://g.alicdn.com/alios-things-3.3/doc/setup_env.html)，下载安装 **alios-studio** 。
待开发环境搭建完成后，可以按照以下步骤进行示例的测试。

### 步骤1 创建或打开工程

**打开已有工程**

如果用于测试的案例工程已存在，可参考[《aos-studio使用说明之打开工程》](https://g.alicdn.com/alios-things-3.3/doc/open_project.html)打开已有工程。

**创建新的工程**

组件的示例代码可以通过编译链接到AliOS Things的任意案例（solution）来运行，这里选择helloworld_demo案例。helloworld_demo案例相关的源代码下载可参考[《aos-studio使用说明之创建工程》](https://g.alicdn.com/alios-things-3.3/doc/create_project.html)。

### 步骤2 添加组件

案例下载完成后，需要在helloworld_demo组件的package.yaml中添加对组件的依赖：

```yaml
depends:
  - osal_aos: master # helloworld_demo中引入osal_aos组件
```

### 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install osal_aos

```

上述命令执行成功后，组件源码则被下载到了./components/osal_aos路径中。

### 步骤4 添加示例

> osal_aos组件的package.yaml中添加[example示例代码](https://gitee.com/alios-things/osal_aos/tree/master/example)：

```yaml
depends:
  - rhino: master
  - cli: master # 添加cli依赖
source_file:
  - "*.c"
  - "example/mutex_example.c" # 添加 mutex_example.c
```

### 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

### 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

### 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

### 步骤8 测试示例
> CLI命令行输入：

```c
mutex_example
```

> 关键日志：

```sh
[aos_mutex_example][0x34035d80]field1=1709388289, field2=1709388289, field3=1709388289, field4=1709388289
[aos_mutex_example][0x340358b0]field1=1386345753, field2=1386345753, field3=1386345753, field4=1386345753
[aos_mutex_example][0x34035d80]field1=2042805823, field2=2042805823, field3=2042805823, field4=2042805823
[aos_mutex_example][0x340358b0]field1=1026149465, field2=1026149465, field3=1026149465, field4=1026149465
[aos_mutex_example][0x34035d80]field1=839374236, field2=839374236, field3=839374236, field4=839374236
[aos_mutex_example][0x340358b0]field1=532559370, field2=532559370, field3=532559370, field4=532559370
[aos_mutex_example][0x34035d80]field1=1406670318, field2=1406670318, field3=1406670318, field4=1406670318
[aos_mutex_example][0x340358b0]field1=24443955, field2=24443955, field3=24443955, field4=24443955
[aos_mutex_example][0x34035d80]field1=693331083, field2=693331083, field3=693331083, field4=693331083
```
## 注意事项

- 互斥量只能由获取该互斥量的任务的释放，不能由其他任务释放。
- 互斥量已被当前任务获取，若当前任务再次获取互斥量则返回错误。
## FAQ
Q1： 调用aos_mutex_lock()接口无限期的获取互斥量，timeout参数怎么设置？
答：将timeout赋值为AOS_WAIT_FOREVER。
# 事件
## 概述
事件是AliOS Things内核提供的一种任务间通信方式，它不同于信号量和互斥量，可以使用事件组实现一个任务同时等待多个事件的发生，或者等待同一个事件的任务在事件发生时解除阻塞状态。事件组是一个32位的数，每一位都对应一个事件标志，事件标志只有两种状态：

   - 1 代表被设置，即当有事件发生时，该事件对应的事件标志位将被设置为1
   - 0 代表被清除，即任务获得事件后，将事件对应的事件标志位被清除为0。

应用程序可以使用事件的如下特性来实现任务间同步：

   - 任务可以等待事件组中设置的所有事件都发生后，即“与”的方式，任务才解除阻塞进入就绪状态；
   - 任务可以等待事件组中设置的任意一个事件发生，即“或”的方式，任务就解除阻塞进入就绪状态；
   - 如果有多个任务等待同一事件，当事件发生时，若任务等待条件满足均会解除阻塞。

### 超时时间
事件等待可设置超时时间，如果任务在超时时间到期后仍未等到事件发生，则任务解除阻塞进入就绪状态。
### 事件功能函数
| **函数名** | **描述** |
| --- | --- |
| aos_event_create() | 事件创建函数（推荐） |
| aos_event_new() | 事件创建函数（兼容3.1） |
| aos_event_free() | 事件删除函数 |
| aos_event_get() | 获取事件函数 |
| aos_event_set() | 设置事件函数 |
| aos_event_is_valid() | 判断事件具柄是否合法函数 |

## 常用配置
> 事件功能: 默认使能，如需修改，在yaml中修改RHINO_CONFIG_EVENT_FLAG配置

```yaml
def_config:
  RHINO_CONFIG_EVENT_FLAG: 0
```


## API说明

- 参考 [aos_kernel_event](https://g.alicdn.com/alios-things-3.3/doc/group__aos__kernel__event.html)
## 使用示例

- 示例代码参考[example/event_example.c](https://gitee.com/alios-things/osal_aos/blob/master/example/event_example.c)，该示例使用事件机制实现任务间同步，具体场景为创建任务A和认为B，以及一事件。任务A以“与”的方式等待事件1和事件2；任务B以“或”的方式等待事件1和事件2。测试任务T设置事件1，则任务B因获取事件得到运行，之后测试任务T设置事件2，则任务A因等到全部事件而得到运行。
- 示例说明如下：
1. t0时刻，任务T调用aos_event_create()创建一事件。任务T然后调用aos_task_create()创建任务A和任务B。任务A调用aos_event_get()以RHINO_AND为选项参数等待事件1和事件2的发生；任务B调用aos_event_get()以RHINO_OR为选项参数等待事件1或事件2的发生。
1. t1时刻，任务T调用aos_event_get()设置事件1，任务B因等到事件1得到运行。
1. t2时刻，任务T调用aos_event_get()设置事件2，任务A因等到了所有事件1和2而得到运行。

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01F9Ij8h1vWwkllyh25_!!6000000006181-2-tps-430-250.png"  style="max-width:800px;" />
</div>

- 该示例可配置到helloworld_demo中运行，相关代码的下载、编译和固件烧录均依赖AliOS Things配套的开发工具 **alios-studio** ，所以首先需要参考[《aos-studio使用说明之搭建开发环境》](https://g.alicdn.com/alios-things-3.3/doc/setup_env.html)，下载安装 **alios-studio** 。
待开发环境搭建完成后，可以按照以下步骤进行示例的测试。

### 步骤1 创建或打开工程

**打开已有工程**

如果用于测试的案例工程已存在，可参考[《aos-studio使用说明之打开工程》](https://g.alicdn.com/alios-things-3.3/doc/open_project.html)打开已有工程。

**创建新的工程**

组件的示例代码可以通过编译链接到AliOS Things的任意案例（solution）来运行，这里选择helloworld_demo案例。helloworld_demo案例相关的源代码下载可参考[《aos-studio使用说明之创建工程》](https://g.alicdn.com/alios-things-3.3/doc/create_project.html)。

### 步骤2 添加组件

案例下载完成后，需要在helloworld_demo组件的package.yaml中添加对组件的依赖：

```c
depends:
  - osal_aos: master # helloworld_demo中引入osal_aos组件
```

### 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install osal_aos

```

上述命令执行成功后，组件源码则被下载到了./components/osal_aos路径中。

### 步骤4 添加示例
> 在osal_aos组件的package.yaml中添加[example示例代码](https://gitee.com/alios-things/osal_aos/tree/master/example)：

```yaml
depends:
  - rhino: master
  - cli: master # 添加cli依赖
source_file:
  - "*.c"
  - "example/evnet_example.c" # 添加 evnet_example.c
```

### 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

### 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

### 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

### 步骤8 测试示例
> CLI命令行输入：

```sh
event_example
```
> 关键日志：

```sh
[aos_event_example]set event 1!
[aos_event_example][taskA]field1=0, field2=0, field3=0, field4=0
[aos_event_example]set event 2!
[aos_event_example][taskB]field1=1123961442, field2=1123961442, field3=1123961442, field4=1123961442
```
## 注意事项

- 在删除事件时，如果有任务阻塞在该事件上，则将这些任务从此事件上脱离，这种任务行为像是事件发生导致的，但实际上并非如此。所以删除事件时需要谨慎。
## FAQ
Q1： 调用aos_event_get()接口无限期的等待事件，timeout参数怎么设置？
答：将timeout赋值为AOS_WAIT_FOREVER。
# 消息队列
## 概述
消息队列是一种任务间传递数据的有效方式。消息队列使用环形缓冲池（ring buffer）来管理消息的队列缓冲区，并使用类似信号量的机制进行任务间的同步。任务通过消息队列可以发送消息，也可以通过它接收消息，从而实现数据的同步及通信。任务发送的消息会暂存在消息队列中，当接收任务来读时，将暂存的数据传递给接收任务；若接收任务在接收数据时，消息队列中无可读数据，则任务会阻塞，直到有消息到来解除阻塞而进入就绪状态。
### 超时时间
消息接收可设置超时时间，如果任务在超时时间到期后仍未收到消息，则任务解除阻塞进入就绪状态。
### 消息队列功能函数
| **函数名** | **描述** |
| --- | --- |
| aos_queue_create() | 消息队列创建函数（推荐） |
| aos_queue_new() | 消息队列创建函数（兼容3.1） |
| aos_queue_free() | 消息队列删除函数 |
| aos_queue_send() | 向消息队列发送消息函数 |
| aos_queue_recv() | 从消息队列读取消息函数 |
| aos_queue_is_valid() | 判断消息队列具柄是否合法 |
| aos_queue_buf_ptr() | 获取消息队列消息数据区地址 |
| aos_queue_get_count() | 获取消息队列当前消息数 |

## 常用配置
> 消息队列功能: 默认使能，如需修改，在yaml中修改RHINO_CONFIG_BUF_QUEUE配置

```yaml
def_config:
  RHINO_CONFIG_BUF_QUEUE: 0
```


## API说明

- 参考 [aos_kernel_queue](https://g.alicdn.com/alios-things-3.3/doc/group__aos__kernel__queue.html)

## 使用示例

- 示例代码参考[example/queue_example.c](https://gitee.com/alios-things/osal_aos/blob/master/example/queue_example.c)，该示例使用消息队列实现任务间数据同步，具体场景为创建任务A和认为B，以及一消息队列。任务A作为生产者循环向消息队列发送消息，任务B作为消费者循环从消息队列接收消息，一般情况下，消费者处理数据是要花费很长时间，所以会导致消息产生的速度大于消息处理的速度，使得消息队列溢出。所以可以通过调整任务B优先级大于任务A来避免这种情况，或者使用信号量来控制数据收发同步。
- 示例说明如下：
1. t0时刻，任务T调用aos_queue_new()创建一互斥量。任务T然后调用aos_task_create()创建任务A和任务B，任务A优先级设置为31，任务B优先级设置为30。任务B因消息队列无消息而阻塞，任务A得到运行向消息队列发送消息。
1. t1时刻，任务B因从消息队列读取到消息而解除阻塞，任务B对消息进行处理后继续等待新的消息到来。
1. t2时刻，任务A向消息队列发送消息。
1. t3时刻，重复t1时刻的操作。

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01VjMeUN1XLuQimsUEW_!!6000000002908-2-tps-438-190.png"  style="max-width:800px;" />
</div>

- 该示例可配置到helloworld_demo中运行，相关代码的下载、编译和固件烧录均依赖AliOS Things配套的开发工具 **alios-studio** ，所以首先需要参考[《aos-studio使用说明之搭建开发环境》](https://g.alicdn.com/alios-things-3.3/doc/setup_env.html)，下载安装 **alios-studio** 。
待开发环境搭建完成后，可以按照以下步骤进行示例的测试。

### 步骤1 创建或打开工程

**打开已有工程**

如果用于测试的案例工程已存在，可参考[《aos-studio使用说明之打开工程》](https://g.alicdn.com/alios-things-3.3/doc/open_project.html)打开已有工程。

**创建新的工程**

组件的示例代码可以通过编译链接到AliOS Things的任意案例（solution）来运行，这里选择helloworld_demo案例。helloworld_demo案例相关的源代码下载可参考[《aos-studio使用说明之创建工程》](https://g.alicdn.com/alios-things-3.3/doc/create_project.html)。

### 步骤2 添加组件

案例下载完成后，需要在helloworld_demo组件的package.yaml中添加对组件的依赖：

```yaml
depends:
  - osal_aos: master # helloworld_demo中引入osal_aos组件
```

### 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install osal_aos

```

上述命令执行成功后，组件源码则被下载到了./components/osal_aos路径中。

### 步骤4 添加示例

> osal_aos组件的package.yaml中添加[example示例代码](https://gitee.com/alios-things/osal_aos/tree/master/example)：

```yaml
depends:
  - rhino: master
  - cli: master # 添加cli依赖
source_file:
  - "*.c"
  - "example/queue_example.c" # 添加 queue_example.c
```

### 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

### 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

### 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

### 步骤8 测试示例
> CLI命令行输入：

```shell
queue_example
```

> 关键日志：

```shell
[aos_queue_example]10 recv message : 3456789012
[aos_queue_example]10 recv message : 4567890123
[aos_queue_example]10 recv message : 5678901234
[aos_queue_example]10 recv message : 6789012345
[aos_queue_example]10 recv message : 7890123456
[aos_queue_example]10 recv message : 8901234567
[aos_queue_example]10 recv message : 9012345678
```
## 注意事项

- 发送消息时，如果没有任务在等待接收消息，则此消息会暂时保存在环形缓冲区中，如果有任务在等待数据接收，则将消息直接传递给任务。
## FAQ
Q1： 调用aos_queue_recv()接口无限期的等待消息，ms参数怎么设置？
答：将ms赋值为AOS_WAIT_FOREVER。


@page trace trace

[更正文档](https://gitee.com/alios-things/trace/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

# 概述

SystemView 是一个用于分析嵌入式系统性能的trace工具。SystemView可以分析系统中有哪些中断、任务执行了，以及它们的先后关系。 可以完整的深入观察一个应用程序运行时的行为，它揭示了在任务运行中发生了什么，哪个中断触发了任务切换，中断和任务调用了哪个底层系统的API函数。SystemView 不但可以实时分析和展示数据，而且可以帮助用户进行系统调试和问题诊断，尤其是在开发和处理具有多个任务的复杂系统时。

SystemView 由两个部分组成：

SystemView 嵌入式端程序：用于记录嵌入式系统的行为，通过实时传输(Real Time Transfer, RTT)技术将记录的数据通过J-link工具传输到PC机上。

SystemView PC端程序：用于收集目标板上传的数据信息，并在不同的窗口中显示这些信息。这些记录可以保存到文件中，用于以后的分析。

## 版权信息

> Apache license v2.0

## 目录结构

```tree
├── Config
│   ├── Global.h                             # Global 数据类型定义
│   ├── SEGGER_RTT_Conf.h                    # SYSTEMVIEW RTT配置文件
│   ├── SEGGER_SYSVIEW_AliOSThings.c         # SYSTEMVIEW AliOSThings 适配代码
│   ├── SEGGER_SYSVIEW_Conf.h                # SYSTEMVIEW配置文件
│   ├── SEGGER_SYSVIEW_Config_AliOSThings.c  # SYSTEMVIEW AliOSThings 相关配置
│   └── SYSVIEW_AliOSThings.txt              # SEGGER SYSTEMVIEW PC端配置文件
├── SEGGER
│   ├── SEGGER.h
|   ├── SEGGER_RTT.c                         # SEGGER RTT 模块源代码
|   ├── SEGGER_RTT.h                         # SEGGER RTT 模块头文件
|   ├── SEGGER_SYSVIEW.c                     #  SEGGER SYSTEMVIEW 模块源代码
|   ├── SEGGER_SYSVIEW.h                     # SEGGER SYSTEMVIEW 模块头文件
|   ├── SEGGER_SYSVIEW_ConfDefaults.h        # SEGGER SYSTEMVIEW 默认配置头文件
|   ├── SEGGER_SYSVIEW_Int.h                 # SEGGER SYSTEMVIEW 内部头文件
|   ├── SEGGER_RTT_printf.c                  # SEGGER RTT 模块 printf 源代码
│   └── SEGGER_RTT_ASM_ARMv7M.S              # 针对Cotex-M 优化RTT模块
├── k_trace.c                                # 核心逻辑代码
├── uart_send.c                              # 用串口传输数据的示例代码
├── README.md                                # 说明文档
└── package.yaml                             # 编译配置文件
```

## 依赖组件

* 无

# 常用配置

系统中相关配置已有默认值，如需修改配置，统一在package.yaml中**def_config**节点修改，具体如下：

> RTT 缓存大小，默认1024,  可修改为4096，修改yaml配置如下：

```yaml
def_config:
     SEGGER_SYSVIEW_RTT_BUFFER_SIZE: 4096
```

更多配置可以参考[SystemView用户手册](https://www.segger.com/downloads/free-utilities/UM08027) 第4.5章 Generic configuration

# API说明

使用 SEGGER_SYSVIEW_PrintfTarget  API通过J-link打印log 信息到时序图和Terminal窗口。

```sh
void SEGGER_SYSVIEW_PrintfTarget(const char *s, ...);
```

例子：SEGGER_SYSVIEW_PrintfTarget("hello world! count %d \r\n", count);

更多的API说明参考[SystemView用户手册](https://www.segger.com/downloads/free-utilities/UM08027) 第7章 API reference

# 使用示例

将HaaS100 SWDIO、SWCLK和地GND共3根线与J-Link仿真器连接。

| HaaS引脚编号 | HaaS引脚说明 | J-link引脚说明 |J-link引脚编号 |
| -------- | -------- | -------- |-------- |
| 41     | SWCLK     | CLK     | 9     |
| 42     | SWDIO     | TMS     | 7     |
| 40     | GND     | GND     | 8     |


硬件连接图：

<div align=left display=flex>     <img src="https://img.alicdn.com/imgextra/i4/O1CN01RD9QyS1zD1zYKLKxv_!!6000000006679-2-tps-976-671.png" style="max-width:800px;" /> 
</div>


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
  - trace: master # helloworld_demo中引入trace组件
```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install trace

```

上述命令执行成功后，组件源码则被下载到了./components/trace路径中。

## 步骤4 添加示例

修改文件solutions/helloworld_demo/helloworld.c


添加头文件
```c
#include "SEGGER_SYSVIEW.h"
```
将
```c
printf("hello world! count %d \r\n", count++);
```
改为；
```c
SEGGER_SYSVIEW_PrintfTarget("hello world! count %d \r\n", count++);
```

## 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

## 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

## 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

## 步骤8 测试示例

**1 下载安装SystemView**
[SystemView下载地址](www.segger.com/downloads/systemview)

根据你所用的操作系统下载对应版本的SystemView，然后进行安装。

**2 配置SystemView**

拷贝components/trace/Config/SYSVIEW_AliOSThings.txt文件到PC机SystemView软件安装目录 C:\Program Files\SEGGER\SystemView\Description下。


**3 打开SystemView PC软件**
点击菜单栏Target打开 Recorder Configuration。

<div align=left display=flex>     <img src="https://img.alicdn.com/imgextra/i2/O1CN01szknPD1ogNbBQBZEU_!!6000000005254-2-tps-488-266.png" style="max-width:800px;" /> 
</div>

参考下图配置参数信息，其中 Address 信息可以从开机串口log 中获取：
例如开机串口log中显示：_SEGGER_RTT:0x34683a1c。

<div align=left display=flex>     <img src="https://img.alicdn.com/imgextra/i3/O1CN01aiPYrd1Ij4DVZLR9h_!!6000000000928-2-tps-640-762.png" style="max-width:800px;" /> 
</div>

**4 开始采集**

点击菜单栏Target打开 Start Recording

<div align=left display=flex>     <img src="https://img.alicdn.com/imgextra/i2/O1CN01E4UIcZ24f2Hwl6Hco_!!6000000007417-2-tps-452-258.png" style="max-width:800px;" /> 
</div>

采集效果界面显示如下：

<div align=left display=flex>     <img src="https://img.alicdn.com/imgextra/i3/O1CN01YKD6fE1Pg2uvaaGmx_!!6000000001869-2-tps-2826-1754.png" style="max-width:800px;" /> 
</div>

# 注意

如果SystemView PC端软件提示overflow，可以通过修改package.yaml 文件增大设备端缓存大小。

修改方式如下：

def_config: 下添加一行
     SEGGER_SYSVIEW_RTT_BUFFER_SIZE: 4096

# FAQ

无

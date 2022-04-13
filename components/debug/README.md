@page debug debug

# 概述
AliOS Things维测（debug）组件支持以下功能：
- 异常管理，在系统异常后接管异常，并输出系统快照coredump信息
- 提供常见的系统调试诊断接口；
- 提供常见的CLI调试命令(可输入help查看)：
- cpuusage：  查询cpu利用率
- tasklist：  查询系统任务状态
- dumpsys mm: 查询系统内存使用状态
- debug：     查询系统内存/任务/队列/信号量/互斥锁等使用状态
- p:          查询内存值
- m:          改写内存值


## 版权信息
> Apache license v2.0
## 目录结构
```sh
├── include
│   └── aos
│       └── debug.h         #维测组件对外头接口
├── package.yaml
├── README.md
└── src
    ├── debug_api.h         #维测组件内部头文件
    ├── debug_backtrace.c   #栈回溯
    ├── debug_backtrace.h
    ├── debug.c             #维测组件对外接口实现
    ├── debug_cli_cmd.c     #调试命令
    ├── debug_cli_cmd.h
    ├── debug_cpuusage.c    #cpu利用率
    ├── debug_cpuusage.h
    ├── debug_dumpsys.c     #内存信息查询
    ├── debug_dumpsys.h
    ├── debug_infoget.c     #组件内部接口实现
    ├── debug_infoget.h
    ├── debug_lastword.c    #临终遗言管理
    ├── debug_lastword.h
    ├── debug_overview.c    #系统调试信息查询
    ├── debug_overview.h
    ├── debug_panic.c       #系统异常coredump
    ├── debug_panic.h
    ├── debug_print.c       #printk调试打印
    ├── debug_print.h
    ├── debug_test.c        #一些测试命令
    └── debug_test.h
```

## 依赖组件
* rhino
* vfs(lastword功能依赖)
* littlefs(lastword功能依赖)


# 常用配置
系统中相关配置已有默认值，如需修改配置，统一在yaml中**def_config**节点修改，具体如下：
> 系统异常后进入调试模式，可继续执行cli命令，系统不会自动复位（默认模式），可修改yaml配置如下，修改为0后，系统异常则会自动复位。
```sh
def_config:
  DEBUG_PANIC_CLI: 1
```
> 系统异常后，默认不会将ulog缓存里的log输出，可修改yaml配置如下，修改为1后，会输出ulog缓存里的log
```sh
def_config:
  DEBUG_ULOG_FLUSH: 0
```
> cpuusage统计结果默认输出到串口终端，可修改yaml配置如下，修改为1后，会将cpuusage统计结果写入文件，不在终端输出
```sh
def_config:
  DEBUG_CPUUSAGE_RECODE_TO_FILE_ENABLE: 0

  #可配置写入文件的cpuusage的统计任务的最大数量
  DEBUG_CPUUSAGE_MAX_TASK: 80

  #可配置写入文件的路径
  DEBUG_CPUUSAGE_FILE_NAME: "/data/cpuusage"
```

# API说明
@ref debug_aos_api

# 使用示例
示例代码参考example/debug_example.c，以运行helloworld_demo为例，具体步骤如下：

## 添加示例代码
> debug组件的package.yaml中添加example
```sh
source_file:
  - example/*.c ? <AOS_COMP_CLI> # add debug_example.c，同时会自动加上cli组件
```

## 添加debug组件
> helloworld_demo组件的pacxkage.yaml中添加
```sh
depends:
  - debug: dev_aos # helloworld_demo中引入cli组件
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

## debug示例测试
> 命令行输入：
```sh
help
```

## 关键日志
> 终端日志：
```sh
================ AliOS Things Command List ==============
debug_api #(注册的cli命令)
================ AliOS Things Command End ===============
```

> 命令行输入：
```sh
debug_api help
```
## 关键日志
> 终端日志：
```sh
You can use debug cmd to show api test:
debug_api help  --- show this
debug_api 1     --- show memory info
debug_api 2     --- show task info
debug_api 3     --- show bufqueue info
debug_api 4     --- show queue info
debug_api 5     --- show sem info
debug_api 6     --- show mutex info
debug_api 7     --- show backtrace now
debug_api 8     --- show backtrace task
debug_api all   --- show all above
```
>可根据提示使用debug_api 进行相应的接口演示
# FAQ
Q1： cpuusage命令的使用说明是什么？
```sh
答：
cpuusage [-d n] [-t m] 命令启动CPU利用率统计，结果输出到串口终端
其中：-d 选项用于指定统计周期，单位为ms，默认为1 s；
      -t 选项用于指定统计时长，单位为ms，默认为连续运行。

举例说明：
cpuusage                   -- 启动一个cpuusage任务，该任务默认每隔1s执行一次统计;
cpuusage -d 3000           -- 启动一个cpuusage任务，该任务默认每隔3s（3000ms）执行一次统计;
cpuusage -d 2000 -t 10000  -- 启动一个cpuusage任务，该任务默认每隔2s（2000ms）执行一次统计，
                              统计到10s（10000ms）后停止；

cpuusage -e                -- 停止统计
```

Q2： 查看修改内存p/m命令的使用说明是什么？
```sh
答：
查看内存
p addr [数量：默认为16个] [字节宽度显示,可选1/2/4，默认为4字节]

举例说明：
p 0x80000000     -- 查看0x80000000处的内存值，默认输出16个地址，以4字节数据宽度显示
0x80000000: 80020000 60012ee1 60012f81 60012f87
0x80000010: 60012fa9 60012fb5 60012fc1 00000000
0x80000020: 00000000 00000000 00000000 60013005
0x80000030: 60013005 00000000 60013005 60013005

p 0x80000000 32   -- 查看0x80000000处的内存值，输出32个地址，以4字节数据宽度显示
0x80000000: 80020000 60012ee1 60012f81 60012f87
0x80000010: 60012fa9 60012fb5 60012fc1 00000000
0x80000020: 00000000 00000000 00000000 60013005
0x80000030: 60013005 00000000 60013005 60013005
0x80000040: 60008919 600076f1 60008919 600145cd
0x80000050: 60008919 60008919 60008919 60014e65
0x80000060: 60008389 60008919 60014939 60008919
0x80000070: 60008919 60008919 600147fb 60008919

p 0x80000000 32 2  -- 查看0x80000000处的内存值，输出32个地址，以2字节数据宽度显示
0x80000000: 0000 8002 2ee1 6001 2f81 6001 2f87 6001
0x80000010: 2fa9 6001 2fb5 6001 2fc1 6001 0000 0000
0x80000020: 0000 0000 0000 0000 0000 0000 3005 6001
0x80000030: 3005 6001 0000 0000 3005 6001 3005 6001

p 0x80000000 32 1  -- 查看0x80000000处的内存值，输出32个地址，以1字节数据宽度显示
0x80000000: 00 00 02 80 e1 2e 01 60 81 2f 01 60 87 2f 01 60
0x80000010: a9 2f 01 60 b5 2f 01 60 c1 2f 01 60 00 00 00 00


修改内存
m addr new_value 将addr处的内存值修改为new_value

举例说明：
m 0x80000000 0x12345678  -- 将0x80000000处的内存值修改为0x12345678
value on 0x80000000 change from 0x80020000 to 0x12345678.

验证:
p 0x80000000
0x80000000: 12345678 60012ee1 60012f81 60012f87
0x80000010: 60012fa9 60012fb5 60012fc1 00000000
0x80000020: 00000000 00000000 00000000 60013005
0x80000030: 60013005 00000000 60013005 60013005

```
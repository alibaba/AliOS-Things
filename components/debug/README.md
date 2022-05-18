@page debug debug

[更正文档](https://gitee.com/alios-things/debug/edit/rel_3.3.0/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://help.aliyun.com/document_detail/302301.html)

# 概述
AliOS Things 调试诊断（debug）组件支持以下功能：
- 异常管理，在系统异常后接管异常，并输出系统快照coredump信息
- 提供常见的系统调试诊断接口；
- 提供常见的CLI调试命令(可输入help查看)，如：
- cpuusage：  查询cpu利用率
- tasklist：  查询系统任务状态
- dumpsys mm: 查询系统内存使用状态
- debug：     查询系统内存/任务/队列/信号量/互斥锁等使用状态
- p:          查询内存值
- m:          改写内存值


## 版权信息
> Apache license v2.0
## 目录结构
```tree
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
```yaml
def_config:
  DEBUG_PANIC_CLI: 1
```
> 系统异常后，默认不会将ulog缓存里的log输出，可修改yaml配置如下，修改为1后，会输出ulog缓存里的log
```yaml
def_config:
  DEBUG_ULOG_FLUSH: 0
```
> cpuusage统计结果默认输出到串口终端，可修改yaml配置如下，修改为1后，会将cpuusage统计结果写入文件，不在终端输出
```yaml
def_config:
  DEBUG_CPUUSAGE_RECODE_TO_FILE_ENABLE: 0

  #可配置写入文件的cpuusage的统计任务的最大数量
  DEBUG_CPUUSAGE_MAX_TASK: 80

  #可配置写入文件的路径
  DEBUG_CPUUSAGE_FILE_NAME: "/data/cpuusage"
```

# API说明
- 参考 [debug_aos_api](https://dev.g.alicdn.com/alios-things-3.3/doc/group__debug__aos__api.html)


# 使用示例

组件使用示例相关的代码下载、编译和固件烧录均依赖AliOS Things配套的开发工具，所以首先需要参考[《AliOS Things集成开发环境使用说明之搭建开发环境》](https://help.aliyun.com/document_detail/302378.html)，下载安装。
待开发环境搭建完成后，可以按照以下步骤进行示例的测试。


## 步骤1 创建或打开工程

**打开已有工程**

如果用于测试的案例工程已存在，可参考[《AliOS Things集成开发环境使用说明之打开工程》](https://help.aliyun.com/document_detail/302381.html)打开已有工程。

**创建新的工程**

组件的示例代码可以通过编译链接到AliOS Things的任意案例（solution）来运行，这里选择helloworld_demo案例。helloworld_demo案例相关的源代码下载可参考[《AliOS Things集成开发环境使用说明之创建工程》](https://help.aliyun.com/document_detail/302379.html)。


## 步骤2 添加组件

案例下载完成后，以运行helloworld_demo为例，需要在helloworld_demo组件的package.yaml中添加对组件的依赖：

```yaml

depends:
  - debug: rel_3.3.0  # helloworld_demo中引入debug组件

```

## 步骤3 下载组件

在已安装了  的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install debug

```

上述命令执行成功后，组件源码则被下载到了./components/debug路径中。

## 步骤4 添加示例

在debug组件的package.yaml中添加[example示例代码](https://gitee.com/alios-things/debug/tree/rel_3.3.0/example)：

```yaml
source_file:
  - example/debug_example.c
```


## 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《AliOS Things集成开发环境使用说明之编译固件》](https://help.aliyun.com/document_detail/302384.html)。

## 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《AliOS Things集成开发环境使用说明之烧录固件》](https://help.aliyun.com/document_detail/302383.html)来烧录固件。

## 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《AliOS Things集成开发环境使用说明之查看日志》](https://help.aliyun.com/document_detail/302382.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

## 步骤8 测试示例

**CLI命令行输入：**
```shell
debug_api help # debug接口测试
```

> 关键日志：
```shell
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
接着可根据提示使用debug_api n 进行相应的接口演示,如


**CLI命令行输入：**
```shell
debug_api 1 # show memory info
```

> 关键日志：
```shell
========== Heap Info  ==========
---------------------------------------------------------------------------
[HEAP]| TotalSz    | FreeSz     | UsedSz     | MinFreeSz  | MaxFreeBlkSz  |
      | 0x0067FFF8 | 0x0064D530 | 0x00032AC8 | 0x0064D120 | 0x0064D530    |
---------------------------------------------------------------------------
```

# 维测命令介绍
## tasklist
```
tasklist   --列出系统中当前所有任务
```
tasklist会列出系统中当前所有任务及其详细信息，具体见下面例子。

注意：根据不同的系统配置，如系统CPU的个数，显示信息会稍有不同。

举例说明：
```
(ash:/data)# tasklist
--------------------------------------------------------------------------------
Name               ID    State    Prio StackSize MinFreesize Runtime  Candidate cpu_binded cpu_num cur_exc
--------------------------------------------------------------------------------
dyn_mem_proc_task  1     PEND     6    2048      1588        531223      N          0         1         0
idle_task          2     RDY      61   4096      3736        130479420   N          1         0         0
idle_task          3     RDY      61   4096      3736        251621      Y          1         1         1
DEFAULT-WORKQUEUE  4     PEND     20   2048      1620        3           N          0         0         0
timer_task         5     PEND     5    8192      7716        482871      N          0         0         0
main_task          6     SLP      32   262144    260212      392482      N          1         0         0
ash                7     RDY      30   16384     14608       22739       N          0         0         1
--------------------------------------------------------------------------------
```
## dumpsys
```
dumpsys [mm | mm_info]  显示系统内存信息
  mm: 显示系统堆内存和小内存池的使用状态, 只有RHINO_CONFIG_MM_BLK配置打开时才会显示小内存的状态。
  mm_info: 显示系统中内存的分配情况，包括所有内存块的状态，
```
dumpsys 显示系统内存信息,包括堆内存和小内存池的使用状态， RHINO_CONFIG_MM_DEBUG配置打开时，使用mm_info参数显示堆内存的分配细节。

举例说明：
```
(ash:/data)# dumpsys mm
---------------------------------------------------------------------------
[HEAP]| TotalSz    | FreeSz     | UsedSz     | MinFreeSz  | MaxFreeBlkSz  |
      | 0x07F435A0 | 0x07E65F80 | 0x000DD620 | 0x07E03E18 | 0x07E244F8    |
---------------------------------------------------------------------------
[POOL]| PoolSz     | FreeSz     | UsedSz     | MinFreeSz  | MaxFreeBlkSz  |
      | 0x00000400 | 0x00000000 | 0x00000400 | 0x00000000 | 0x00000020    |
---------------------------------------------------------------------------
```

```
(ash:/data)# dumpsys mm_info

------------------------------- all memory blocks ---------------------------------
g_kmm_head = 400b8a60
ALL BLOCKS
Blk_Addr    Stat     Len  Chk      Caller    Point
0x0x400b8b40  used       8   OK  0x0
0x0x400b8b58  used    1368   OK  0x0
0x0x400b90c0  used      40   OK  0x40009ddd
0x0x400b90f8  used      40   OK  0x40009ddd
......
----------------------------- all free memory blocks ------------------------------
freelist bitmap: 0x201886
   Address  Stat     Len  Chk    Caller
0x0x401975d8  free      88   OK  0x0
0x0x401d79b8  free     232   OK  0x0
0x0x40195ed0  free    5384   OK  0x0
0x0x40197898  free  130656   OK  0x0
0x0x401b78b8  free  131256   OK  0x0
0x0x401d7ae8  free  132269304   OK  0x0

--------------------------- memory allocation statistic ---------------------------
[2^06] bytes:   747   |[2^07] bytes:   235   |[2^08] bytes:    61   |[2^09] bytes:    62   |
[2^10] bytes:   431   |[2^11] bytes:     9   |[2^12] bytes:     4   |[2^13] bytes:    21   |
[2^14] bytes:     0   |[2^15] bytes:     1   |[2^16] bytes:     0   |[2^17] bytes:     0   |
[2^18] bytes:    10   |[2^19] bytes:     1   |[2^20] bytes:     0   |[2^21] bytes:     0   |
[2^22] bytes:     0   |[2^23] bytes:     0   |[2^24] bytes:     0   |[2^25] bytes:     0   |
[2^26] bytes:     0   |[2^27] bytes:     0   |[2^28] bytes:     0   |[2^29] bytes:     0   |

-------------------------------[kernel] heap size overview --------------------------------
---------------------------------------------------------------------------
[HEAP]| TotalSz    | FreeSz   Fail  |
      | 32         | 1024       | 0          | 1024       | 0          |
Total |            | 1024       | 0          | 1024       | 0          |
Pool Size 1024, Free Slice size 0x0.
---------------------------------------------------------------------------

--------------------------- task allocation statistic -----------------------------
TaskName                 Prio  StackSize  HeapAllocSize
main_task                32    65536      593328
ash                      30    4096       1672
adbd-input               41    32768      240
adbd-output              41    32768      4360
Allocated in ISR 290328, Allocated by deleted task 1072

-----------------------------------------------------------------------------------

```

## taskbt
```
taskbt <taskid>  --任务栈回溯
```
taskbt 对taskid指定的任务进行调用栈回溯，逆序输出函数调用关系，即最近调用的函数最先输出。由于嵌入式系统中资源优先，不能加载系统符号表，所以cli中只能输出函数的地址。得到地址后，可以使用工具链中的addr2line工具解析地址得到函数名称。

举例说明：
```
(ash:/data)# taskbt 6
task name : main_task
========== Call stack ==========
backtrace : 0x4000CD50
backtrace : 0x4000CD50
backtrace : 0x4005A470
backtrace : ^task entry^
==========    End     ==========
```
如对于ARM架构的板子，在Linux Host上把工具链中的命令加入到PATH编辑变量。
```
export PATH= ~/.aliot/arm-ali-aoseabi/main/bin:$PATH
```
使用addr2line工具解析地址得到函数名称，把xxxxx.elf替换成你本地的elf文件。
```
arm-ali-aoseabi-addr2line -piCafe out/xxxxx.elf 0x4000CD50 0x4005A470
```

## taskbtn
```
taskbtn <task_name>  --任务栈回溯
```
taskbt 对task_name指定的任务进行调用栈回溯，逆序输出函数调用关系，即最近调用的函数最先输出。由于嵌入式系统中资源优先，不能加载系统符号表，所以cli中只能输出函数的地址。得到地址后，可以使用工具链中的addr2line工具解析地址得到函数名称。

举例说明：
```
(ash:/data)# taskbtn main_task
========== Call stack ==========
backtrace : 0x4000CD50
backtrace : 0x4000CD50
backtrace : 0x4005A470
backtrace : ^task entry^
==========    End     ==========
```

如对于ARM架构的板子，在Linux Host上把工具链中的命令加入到PATH编辑变量。
```
export PATH= ~/.aliot/arm-ali-aoseabi/main/bin:$PATH
```
使用addr2line工具解析地址得到函数名称，把xxxxx.elf替换成你本地的elf文件。
```
arm-ali-aoseabi-addr2line -piCafe out/xxxxx.elf 0x4000CD50 0x4005A470
```

## cpuusage
```
cpuusage [-d n] [-t m] [-f] [-s]  --统计cpu利用率
启动CPU利用率统计，结果输出到串口终端
  -d 选项用于指定统计周期，单位为ms，默认为1 s。
  -t 选项用于指定统计时长，单位为ms，默认为连续运行。
  -f 记录到文件。依赖DEBUG_CPUUSAGE_RECODE_TO_FILE_ENABLE配置打开。
  -s 同步模式，在cpuusage运行期间不会让出当前终端，无法同时执行其他命令。
```
注意cpuusage命令依赖于RHINO_CONFIG_SYS_STATS配置打开，否则找不到此命令。

举例说明：
```
(1) cpuusage                   -- 启动一个cpuusage任务，该任务默认每隔1s执行一次统计;
(2) cpuusage -d 3000           -- 启动一个cpuusage任务，该任务默认每隔3s（3000ms）执行一次统计;
(3) cpuusage -d 2000 -t 10000  -- 启动一个cpuusage任务，该任务默认每隔2s（2000ms）执行一次统计，统计到10s（10000ms）后停止；
(4) cpuusage -f                -- 启动一个cpuusage任务，统计CPU利用率输出到文件（DEBUG_CPUUSAGE_FILE_NAME 配置）
(4) cpuusage -e                -- 停止统计
```
## p
```
p <addr> <nunits> <width>  --查看内存数据
  addr: 起始内存地址
  nunits: 要显示单位的个数，默认为16
  width: 单位的字节宽度，可选1/2/4，默认为4字节
查看内存
```

举例说明：
```
p 0x80000000     -- 查看0x80000000处的内存值，默认输出16个单位，以4字节数据宽度为单位显示
0x80000000: 80020000 60012ee1 60012f81 60012f87
0x80000010: 60012fa9 60012fb5 60012fc1 00000000
0x80000020: 00000000 00000000 00000000 60013005
0x80000030: 60013005 00000000 60013005 60013005

p 0x80000000 32   -- 查看0x80000000处的内存值，输出32个单位，以4字节数据宽度为单位显示
0x80000000: 80020000 60012ee1 60012f81 60012f87
0x80000010: 60012fa9 60012fb5 60012fc1 00000000
0x80000020: 00000000 00000000 00000000 60013005
0x80000030: 60013005 00000000 60013005 60013005
0x80000040: 60008919 600076f1 60008919 600145cd
0x80000050: 60008919 60008919 60008919 60014e65
0x80000060: 60008389 60008919 60014939 60008919
0x80000070: 60008919 60008919 600147fb 60008919

p 0x80000000 32 2  -- 查看0x80000000处的内存值，输出32个单位，以2字节数据宽度为单位显示
0x80000000: 0000 8002 2ee1 6001 2f81 6001 2f87 6001
0x80000010: 2fa9 6001 2fb5 6001 2fc1 6001 0000 0000
0x80000020: 0000 0000 0000 0000 0000 0000 3005 6001
0x80000030: 3005 6001 0000 0000 3005 6001 3005 6001

p 0x80000000 32 1  -- 查看0x80000000处的内存值，输出32个单位，以1字节数据宽度为单位显示
0x80000000: 00 00 02 80 e1 2e 01 60 81 2f 01 60 87 2f 01 60
0x80000010: a9 2f 01 60 b5 2f 01 60 c1 2f 01 60 00 00 00 00
```

## m
```
m <addr> <value> <width> --修改内存数据
  addr: 起始内存地址
  value: 要修改的值，默认为0
  width: 要修改内存的长度，单位字节，可选1/2/4，默认为4
```

举例说明：
```
m 0x80000000 0x12345678  -- 将0x80000000处的内存值修改为0x12345678
value on 0x80000000 change from 0x80020000 to 0x12345678.
```

验证:
```
p 0x80000000
0x80000000: 12345678 60012ee1 60012f81 60012f87
0x80000010: 60012fa9 60012fb5 60012fc1 00000000
0x80000020: 00000000 00000000 00000000 60013005
0x80000030: 60013005 00000000 60013005 60013005
```

## f
```
f <func_addr> [arg]*  -- 运行函数
  func_addr: 函数的地址
```
f <func_addr> 在当前终端的上下文中运行函数func_addr， 此函数最多支持8个参数， 即参数arg0 ~ arg7，且参数的类型为uint32_t或相兼容的类型。 在编译生成的aos.map文件中可以根据函数名称查找函数地址。

举例说明：
```
(ash:/data)# f 0x4000a588
function 0x4000a588 runing...
function 0x4000a589 return 0x48f8.
```

## 打断卡死
在系统卡死的情况下，可以通过串口输入命令`$#@!`打断卡死。此命令会在中断模式下引发未定义指令的异常，从而进行异常现场的打印，接着系统进入panic cli模式。用户可以在panic模式下输入`tasklist/cpuusage/p/m`等常用的调试命令进行调试。 比如进入panic模式后，输入`cpuusage`命令找出占用CPU过高的任务，再通过`taskbt <task_id>`找出卡死的位置。

此功能需要芯片的UART驱动对接支持。需要在芯片的UART的中断ISR函数中调用`panicNmiInputFilterAll/panicNmiInputFilter`对`$#@!`命令进行特殊处理。至此只能处理普通任务死循环或者关调度后等待导致其他任务不能调度的卡死问题。如果关闭中断后，有死循环或等待从而不能响应中断的卡死问题，需要把UART的中断设置为FIQ（ARM CORTEX-A）或NMI中断，总之要让UART能够接收串口输入（依赖芯片设计）。

举例如下：
```
!!!!!!!! Stopped by '$#@!' !!!!!!!!

!!!!!!!!!! Exception  !!!!!!!!!!
crash   time : 1970-01-01 00:00:12
current task : idle_task
========== coreID: 0  ==========
========== Regs info  ==========
Exception Type: Undefined Instruction
PC      0x4000C6CA
LR      0x4000B031
SP      0x400E2920
CPSR    0x000000B3
R0      0x400E3B00
R1      0x400E0E70
R2      0x00000000
R3      0xB33F0001
R4      0x400E3B00
R5      0x00000013
R6      0x023D7B94
R7      0x00000000
R8      0x400E3B00
R9      0x09090909
R10     0x10101010
R11     0x11111111
R12     0x12121212
========== Fault info ==========
DFSR    0x00002ED1
DFAR    0x798373FB
IFSR    0x00001020
IFAR    0xED44F5E8
========== Call stack ==========
backtrace : 0x4000C6CA
backtrace : 0x4000B02C
backtrace : ^task entry^
========== Heap Info  ==========
---------------------------------------------------------------------------
[HEAP]| TotalSz    | FreeSz     | UsedSz     | MinFreeSz  | MaxFreeBlkSz  |
      | 0x07EE8020 | 0x07E0C9A8 | 0x000DB678 | 0x07DA97C8 | 0x07DCAF30    |
---------------------------------------------------------------------------
========== Task Info  ==========
......
========== Queue Info ==========
-------------------------------------------------------
QueAddr    TotalSize  PeakNum    CurrNum    TaskWaiting
-------------------------------------------------------
======== Buf Queue Info ========
------------------------------------------------------------------
BufQueAddr TotalSize  PeakNum    CurrNum    MinFreeSz  TaskWaiting
------------------------------------------------------------------
0x400E3B40 0x000001E0 0x00000000 0x00000000 0x000001E0 timer_task
0x40166080 0x00000003 0x00000000 0x00000000 0x00000003 adbd-input
========= Sem Waiting ==========
--------------------------------------------
SemAddr    Count      PeakCount  TaskWaiting
--------------------------------------------
0x400E3AB8 0x00000000 0x00000000 dyn_mem_proc_task
0x400E5D78 0x00000000 0x00000000 DEFAULT-WORKQUEUE
0x40165F28 0x00000000 0x00000000 adbd-output
Total: 0x00000087
======== Mutex Waiting =========
--------------------------------------------
MutexAddr  TaskOwner            NestCnt    TaskWaiting
--------------------------------------------
Total: 0x0000002F
========== Stack info ==========
......
!!!!!!!!!! dump end   !!!!!!!!!!

(panic)#
(panic)#tasklist
--------------------------------------------------------------------------------
Name               ID    State    Prio StackSize MinFreesize Runtime  Candidate cpu_binded cpu_num cur_exc
--------------------------------------------------------------------------------
dyn_mem_proc_task  1     PEND     6    2048      1588        481534      N0         1         0
idle_task          2     RDY      61   4096      3736        14946036    N1         0         1
idle_task          3     RDY      61   4096      3736        5720189     Y1         1         1
DEFAULT-WORKQUEUE  4     PEND     20   2048      1620        2           N0         0         0
timer_task         5     PEND     5    8192      7716        433155      N0         0         0
main_task          6     SLP      32   262144    260212      391109      N1         0         0
ash                7     PEND     30   16384     14664       213         N0         0         0
```

## debug
```
debug  --显示系统总体信息
```
debug命令显示系统当前的一些状态，包括堆内存使用状态，任务信息，消息队列信息，信号量，互斥量状态。

举例说明：
```
(ash:/data)# debug
========== Heap Info  ==========
---------------------------------------------------------------------------
[HEAP]| TotalSz    | FreeSz     | UsedSz     | MinFreeSz  | MaxFreeBlkSz  |
      | 0x07F435A0 | 0x07E65F88 | 0x000DD618 | 0x07E02E60 | 0x07E24500    |
---------------------------------------------------------------------------
[POOL]| PoolSz     | FreeSz     | UsedSz     | MinFreeSz  | MaxFreeBlkSz  |
      | 0x00000400 | 0x00000000 | 0x00000400 | 0x00000000 | 0x00000020    |
---------------------------------------------------------------------------
========== Task Info  ==========
--------------------------------------------------------------------------
TaskName             State    Prio       Stack      StackSize (MinFree)      cpu_binded    cpu_num    cur_exc
--------------------------------------------------------------------------
dyn_mem_proc_task    PEND     0x00000006 0x400A4AF0 0x00000800(0x00000634)
idle_task            RDY      0x0000003D 0x400A54D4 0x00001000(0x00000E98)
......
========== Queue Info ==========
-------------------------------------------------------
QueAddr    TotalSize  PeakNum    CurrNum    TaskWaiting
-------------------------------------------------------
======== Buf Queue Info ========
------------------------------------------------------------------
BufQueAddr TotalSize  PeakNum    CurrNum    MinFreeSz  TaskWaiting
------------------------------------------------------------------
0x400A76C0 0x000001E0 0x00000000 0x00000000 0x000001E0 timer_task
0x4010C640 0x00000003 0x00000000 0x00000000 0x00000003 adbd-input
========== Sem Waiting ==========
--------------------------------------------
SemAddr    Count      PeakCount  TaskWaiting
--------------------------------------------
0x400A7638 0x00000000 0x00000000 dyn_mem_proc_task
0x400A98F8 0x00000000 0x00000000 DEFAULT-WORKQUEUE
Total: 0x00000088
========= Mutex Waiting =========
--------------------------------------------
MutexAddr  TaskOwner            NestCnt    TaskWaiting
--------------------------------------------
Total: 0x00000030
```

## sysver
```
sysver --显示内核rhino的版本信息
```

使用举例：
```
(ash:/data)# sysver
kernel version :12000
```

## time
```
time --显示系统从启动开始的时间，单位:ms
```

使用举例：
```
(ash:/data)# time
UP time 570690 ms
```

## msleep
```
msleep <num> --使当前终端任务睡眠
  num: 指定要睡眠的时间，单位:ms
```

使用举例：
```
(ash:/data)# msleep 1000
(ash:/data)#
```

## devname
```
devname  --显示设备的名称
```
devname 显示SYSINFO_DEVICE_NAME配置的设备名称

使用举例：
```
(ash:/data)# devname
device name: HaaS100
```

## err2cli
```
err2cli <cli_flag> --异常时是否进入panic模式
  cli_flag: 为1时，系统异常后进入panic模式，即cli，此时可以查看一些系统状态。否则异常后直接重启。
```
err2cli 设置系统异常后是否进入panic模式（cli）。注意如果DEBUG_PANIC_CLI配置为1时，系统异常后总是进入panic模式。

使用举例：
```
(ash:/data)# err2cli 1
set panic_runto_cli flag:0x21314916
```

发生异常后，首先打印系统快照，然后根据配置，可能进入系统panic模式：
```
(panic)#
(panic)#
(panic)#tasklist
--------------------------------------------------------------------------------
Name               ID    State    Prio StackSize MinFreesize Runtime  Candidate cpu_binded cpu_num cur_exc
--------------------------------------------------------------------------------
dyn_mem_proc_task  1     PEND     6    2048      1588        399762      N0         1         0
idle_task          2     RDY      61   4096      3736        11767779    N1         0         0
idle_task          3     RDY      61   4096      3736        6716553     Y1         1         1
DEFAULT-WORKQUEUE  4     PEND     20   2048      1620        3           N0         0         0
timer_task         5     PEND     5    8192      7716        351381      N0         0         0
main_task          6     RDY      32   262144    260220      397494      N1         0         0
ash                7     RDY      30   16384     14664       1469        N0         0         1
--------------------------------------------------------------------------------
```


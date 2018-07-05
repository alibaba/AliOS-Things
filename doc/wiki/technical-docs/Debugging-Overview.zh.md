[EN](Debugging-Overview) | 中文

AliOS Things支持多种调试手段，针对不同的模块，不同的使用场景，开发者可以根据需要自己选择：
- linuxhost模拟环境：适合于硬件无关的模块或者代码，可以使用gdb，valgrind等流行的工具
- CLI环境：适合板上轻度调试，使用系统的各种内存调试工具
- JTAG环境：适合板上调试，利用硬件调试能力，本文不描述

## linuxhost模拟环境

AliOS Things支持在Linux（Ubuntu 16.04）上作为一个process来运行，可以使用Linux的各类流行的工具。

### Ubuntu环境设置
```
$ sudo apt-get install -y gcc-multilib
$ sudo apt-get install -y libssl-dev libssl-dev:i386
$ sudo apt-get install -y libncurses5-dev libncurses5-dev:i386
$ sudo apt-get install -y libreadline-dev libreadline-dev:i386
$ sudo apt-get install -y python-pip
$ sudo pip install aos-cube
```
### 编译运行

```
$ git clone https://github.com/alibaba/AliOS-Things.git
$ cd AliOS-Things
$ aos make helloworld@linuxhost
$ ./out/helloworld@linuxhost/binary/helloworld@linuxhost.elf
```

### gdb
```
$ gdb -x platform/mcu/linux/tools/gdbinit --args ./out/alinkapp@linuxhost/binary/alinkapp@linuxhost.elf
(gdb) r
```
### coredump
```
$ ulimit -c unlimited
# switch to 'root' account
$ sudo su
# generate core dump file as /tmp/core_<exec><pid>
$ echo '/tmp/core%e_%p' > /proc/sys/kernel/core_pattern
# exit 'root' account
$ exit

# if program crashed
$ gdb ./out/target/product/linuxhost/main /tmp/core_<exec>_<pid>
```

### valgrind

编译app时使能valgrind

```
$ aos make helloworld@linuxhost valgrind=1
```

调试

```
$ valgrind ./out/helloworld@linuxhost/binary/helloworld@linuxhost.elf
```

gdb+valgrind

```
$ valgrind --vgdb-error=1 ./out/helloworld@linuxhost/binary/helloworld@linuxhost.elf
#等待类似的log出现
==26510== (action on error) vgdb me ...
#另一个终端
$ gdb ./out/helloworld@linuxhost/binary/helloworld@linuxhost.elf
(gdb) target remote | vgdb
```

### LwIP

即使在Linux下面也可以使用LwIP进行网络相关开发调试

```
# switch to 'root'
$ sudo su
$ vi /etc/sysctl.conf
net.ipv4.ip_forward=1
$ sysctl -p /etc/sysctl.conf
# <netif>替换为可以访问外网的I/F的名字，比如eth0
$ iptables -t nat -A POSTROUTING -s 192.168.0.0/24 -o <netif> -j MASQUERADE
# exit 'root'
$ exit

# 会需要sudo权限，不要额外加sudo，因为tapup.sh里面会获取当前用户id
$ ./csp/vendor/linuxhost/tools/tapup.sh tap0
$ export PRECONFIGURED_TAPIF=tap0
$ aos make alinkapp@linuxhost LWIP=1
$ ./out/alinkapp@linuxhost/binary/alinkapp@linuxhost.elf
```



## CLI环境

AliOS Things提供了cli组件，只要使能改组件，就可以使用类shell的功能。

```
# help
====Build-in Commands====
====Support six cmds once, seperate by ; ====
help: 
echo: 
exit: CLI exit
devname: print device name
sysver: 
reboot: reboot system
time: system time
ota: system ota

====User Commands====
tasklist: list all thread info
dumpsys: dump system info
loglevel: set log level
udp: [ip] [port] [string data] send udp data
wifi_debug: wifi debug mode
mac: get/set mac
trace: trace [start ip (port) | task task_name| event event_id| stop]
```



### 检查各个任务的栈使用情况

关注MinFreesize这一列，显示任务剩余的最小栈

```
# tasklist
------------------------------------------------------------------------
Name               State    Prio StackSize MinFreesize Runtime Candidate
------------------------------------------------------------------------
dyn_mem_proc_task  PEND     5    8192      7897        0        N          
idle_task          RDY      61   8192      7404        0        N          
idle_task          RDY      61   8192      7453        0        N          
DEFAULT-WORKQUEUE  PEND     9    8192      7993        0        N          
timer_task         PEND     5    8192      8018        0        N          
app_task           PEND     20   8192      5302        0        N          
cpu_intr           PEND     0    8192      7420        0        N          
cli                RDY      33   8192      5225        0        Y          
------------------------------------------------------------------------
```

### 内存状态

使能board/*/k_config.h里面的RHINO_CONFIG_MM_DEBUG，及RHINO_CONFIG_MM_LEAKCHECK

那么dumpsys命令将多出几条子命令

```
# dumpsys
dumpsys help:
	dumpsys task         : show the task info.
	dumpsys mm_info      : show the memory has alloced.
	dumpsys mm_leak      : show the memory maybe leak.
	dumpsys mm_monitor   : [start/stop] [round time] fire a timerto monitor mm, default 10s.
```

#### dumpsys mm_info

```
# dumpsys mm_info

------------------------------- all memory blocks --------------------------------- 
g_kmm_head =  806f820
ALL BLOCKS
address,  stat   size     dye     caller   pre-stat    point
0x80701e0  used       8  fefefefe  0x0        pre-used;
0x80701f8  used    8224  fefefefe  0x0        pre-used;
----------------------------- all free memory blocks ------------------------------- 
address,  stat   size     dye     caller   pre-stat    point
FL bitmap: 0x2000
SL bitmap 0x4000
-> [13][14]
0x80b6e0c  free  756212  abababab  0x0        pre-used; free[   (nil),   (nil)] 

------------------------- memory allocation statistic ------------------------------ 
     free     |     used     |     maxused
      764212  |      284364  |      284364

-----------------alloc size statistic:-----------------
[2^02] bytes:     0   |[2^03] bytes:     1   |[2^04] bytes:     4   |[2^05] bytes:     3   |
[2^06] bytes:     6   |[2^07] bytes:     1   |[2^08] bytes:     3   |[2^09] bytes:     8   |
[2^10] bytes:     0   |[2^11] bytes:     0   |[2^12] bytes:     2   |[2^13] bytes:     1   |
[2^14] bytes:     1   |[2^15] bytes:     8   |[2^16] bytes:     0   |[2^17] bytes:     0   |
[2^18] bytes:     0   |[2^19] bytes:     0   |[2^20] bytes:     0   |[2^21] bytes:     0   |
[2^22] bytes:     0   |[2^23] bytes:     0   |[2^24] bytes:     0   |

```

### dumpsys mm_leak

```
# dumpsys mm_leak
adress:0x80acf28 owner:0x805ae3a len:3096  type:leak
```

改命令会显示可能出现内存泄漏的地方，owner代码调用了malloc分配的疑似泄露的函数地址，可以通过addr2line或者objdump，映射到相应的函数。

### Misc
各个模块根据需要会在提供自己的调试命令，比如KV
```
# kv list
# kv del key
```

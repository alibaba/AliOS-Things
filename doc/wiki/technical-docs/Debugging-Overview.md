EN | [中文](Debugging-Overview.zh)

AliOS Things supports a variety of debugging methods based on different modules and different usage scenarios. Developers can choose from the following ones according to their own needs:

- linuxhost virtual environment: It's suitable for modules and code that have nothing to do with hardwares; Popular tools such as GDB and Valgrind can be used.
- CLI environment: It's suitable for light debugging on the board; Various memory debugging tools offered by the system can be used.
- JTAG environment: It's suitable for debugging on the board; Hardware debugging capabilities are used (this part will not be unfolded in this article) .

## linuxhost virtual environment

AliOS Things is supported to work as a process in Linux（Ubuntu 16.04）, and various tools in Linux can be used.

### setting of Ubuntu environment
```
$ sudo apt-get install -y gcc-multilib
$ sudo apt-get install -y libssl-dev libssl-dev:i386
$ sudo apt-get install -y libncurses5-dev libncurses5-dev:i386
$ sudo apt-get install -y libreadline-dev libreadline-dev:i386
$ sudo apt-get install -y python-pip
$ sudo pip install aos-cube
```
### Compile and run

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

Run valgrind when compiling apps.

```
$ aos make helloworld@linuxhost valgrind=1
```

debug

```
$ valgrind ./out/helloworld@linuxhost/binary/helloworld@linuxhost.elf
```

gdb+valgrind

```
$ valgrind --vgdb-error=1 ./out/helloworld@linuxhost/binary/helloworld@linuxhost.elf
# wait for a similar log
==26510== (action on error) vgdb me ...
# another terminal
$ gdb ./out/helloworld@linuxhost/binary/helloworld@linuxhost.elf
(gdb) target remote | vgdb
```

### LwIP

Even in Linux, LwIP can be used for network-related debugging.

```
# switch to 'root'
$ sudo su
$ vi /etc/sysctl.conf
net.ipv4.ip_forward=1
$ sysctl -p /etc/sysctl.conf
# replace <netif> with a I/F name that have access to external network, such as eth0
$ iptables -t nat -A POSTROUTING -s 192.168.0.0/24 -o <netif> -j MASQUERADE
# exit 'root'
$ exit

# sudo permission is required, but no additional sudo is needed, since current user ID can be obtained in tapup.sh 
$ ./csp/vendor/linuxhost/tools/tapup.sh tap0
$ export PRECONFIGURED_TAPIF=tap0
$ aos make alinkapp@linuxhost LWIP=1
$ ./out/alinkapp@linuxhost/binary/alinkapp@linuxhost.elf
```



## CLI environment

AliOS Things provides CLI, and shell functions can be used when component is changed.

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



### Check the usage of each task stack

The MinFreesize column shows the least remaining size of that task stack.

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

### Memory state

Choose RHINO_CONFIG_MM_DEBUG and RHINO_CONFIG_MM_LEAKCHECK in board/*/k_config.h, and some subcommands will be added to dumpsys commands.

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

This command will show where memory leak is possible to happen. Code owner can call the function address of possible leak, which can further be mapped to the corresponding function by addr2line or objdump.

### Misc
Each module can provide its own debug commands according to its needs, such as KV.

```
# kv list
# kv del key
```

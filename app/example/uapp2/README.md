## Contents

```sh
app/example/uapp2/
├── Config.in      	# config file
├── README.md
├── aos.mk          # make file
├── include         # header files
└── src             # source files
```

## Introduction

This example will generate a standalone applications which runs at user space. User can update/load/unload this application at runtime while kernel and other applications still keep running.

To run this application, you should at least have compiled kernel which runs at kernel space.


```c
         app              app        app
    +------------+  +------------+  +---+
    |   uapp1    |  |    uapp2   |  |   | user space
    +-  -  -  -  +  +-  -  -  -  +  |...|
    |  syscall   |  |   syscall  |  |   |
    +------------+  +------------+  +---+
=====================================================
    +-----------------------------------+
    |             syscall               |
    |-  -  -  -  -  -  -  -  -  -  -  - | kernel space
    | rhino | hal | lwip | fs |   ...   |
    +-----------------------------------+
                    kernel
```

To run uapp2, you should at least have compiled and flashed kernel which runs at kernel space. If you want to run more user space applications, for example uapp1, you should compile and flash kernel image, uapp1 image, and uapp2 image.

**Also you can refer to kernel/mk/doc/AliOS-Things_user_space_program-v0.4.pdf for more complete experience.**

## Supported Boards

- **developerkit-mk**
- **stm32f429zi-mk**

## Build and flash

Assume you have intalled **aos-cube** tool on your machine. Refer to [Quick Start](https://github.com/alibaba/AliOS-Things/wiki/Quick-Start) for installing aos-cube if you haven't install it.

### Build kernel image

For example, Build kernel image for the developerkit-mk board.

`aos make uapp2@developerkit-mk -c config`

`aos make MBINS=kernel`

Kernel image will be generated in `out/uapp2@developerkit-mkkernel/binary` directory.

### Build application images

#### uapp2

`aos make uapp2@developerkit-mk -c config`

`aos make MBINS=app MBINS_APP=app2`

uapp2 will be generated in `out/uapp2@developerkit-mkapp/binary` directory.

## Downlaod images

You should install **st-link** tool to download kernel and app images to the board. More info about st-link, refer to [**st-link**](https://github.com/texane/stlink) on github.

### Download uapp2 image

flash uapp2 binary with command:

`st-flash write out/uapp2@developerkit-mkapp/binary/uapp2@developerkit-mk.app.bin 0x080a0000`

### Download kernel image

flash kernel binary with the command:

`st-flash write out/uapp1@developerkit-mkkernel/binary/uapp1@developerkit-mk.kernel.bin 0x08000000`

On developerkit-mk board, I recommand you flash kernel and app images twice, and then reset the chip.

## Useful CLI commands

kernel have registered some CLI commands related to user space application operations, the most useful commands are:

* list all the valid user space applications downloaded on the board

  with command `lsapp`, you can check all the valid user space applications have been flashed on the board.

  `# lsapp`

  `========== app info ==========`
  `name                    state`
  `uapp2                   run`

* check user space application PID

  command `process` will dump all the running applications name, PID and the total task count of the process

  `# process`

  `============ process info ============`
  `Name                pid       tasks`
  `uapp2               1         4`

* kill an user space application

  to kill a running user space application, issue command `kill PID`, you can check the application's PID number via `process` command

  `# kill 1`

  `process uapp2  is unloaded, pid 1`

* Load an user space application

  Load an application with command `start app_name`, **app_name** is defined with macro `RHINO_CONFIG_APP_NAME` in entry_elf.c. For example, load uapp2 with command：

  `# start uapp2`

  `uapp1 start to run...`

* list user space commands

  Type `help` command in CLI, it wil list all the commands, including user space commands if there are any. For example, the third part commands below are user space commands.

  `# help`

  `====Build-in Commands====`
  `====Support 4 cmds once, seperate by ; ====`
  `help      : print this`
  `sysver    : system version`
  `reboot    : reboot system`
  `time      : system time`
  `ota       : system ota`
  `cpuusage  : show cpu usage`
  `p         : print memory`
  `m         : modify memory`
  `echo      : echo for command`
  `exit      : close CLI`
  `devname   : print device name`
  `process   : process info`
  `kill      : kill process`

  `====User Commands====`
  `tasklist  : list all thread info`
  `dumpsys   : dump system info`
  `loglevel  : set sync log level`
  `ifconfig  : start net interface`
  `kernel_read_g_proc_var: kernel read g_proc_var`
  `kernel_set_g_proc_var: kernel set g_proc_var`
  `lsapp     : list all app`
  `start     : start app`

  **`====User app Commands====`**

  `uapp2_proc_msg_test: uapp2 proc_msg_test`
  **`uapp2_set_g_proc_var: uapp2 set g_proc_var`**
  **`uapp2_read_g_proc_var: uapp2 read g_proc_var`**
  **`uapp2_heap: dump uapp1 heap statistics`**

Refer to `board/developerkit-mk/README.md` or `board/stm32f429zi-mk/README.md` for details.

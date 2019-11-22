stm32f429zi-mk/README.md ## Overview

**developerkit-mk** is not a new board, actually the same as **developerkit** board,  developerkit-mk board is created to demostrate user space applications.

## Feature of the Board

### Supported applications

* **uapp1**
* **uapp2**

Currently the board only supports two applications, user space **uapp1** and **uapp2**, you can find the two applications in app/example directory.

### Directories

```sh
developerkit-mk
├── Config.in                       # board configs
├── Inc                             # includes files
├── STM32L496VGTx_FLASH_kernel.ld   # linker file for kernel
├── Src                             # source files
├── aos                             # source files
├── aos.mk                          # make file
├── app1.ld                         # linker file for app1
├── app2.ld                         # linker file for app2
├── gen_image_bin.mk
├── hal                             # hal files
└── startup_stm32l496xx.s           # startup file
```

### Board Hardware Resources

Please refer to developkerkit board for the hardware resources.

### Flash Partitions

| Start Addr | Size      | Description                           |
| ---------- | --------- | ------------------------------------- |
| 0x08000000 | 512KBytes | kernel flash region                   |
| 0x08080000 | 128KBytes | the first application's flash region  |
| 0x080a0000 | 128KBytes | the second application's flash region |

### RAM Partitions

| Start Addr | Size      | Description                         |
| ---------- | --------- | ----------------------------------- |
| 0x20000000 | 128KBytes | kernel RAM region                   |
| 0x20020000 | 64KBytes  | the first application's RAM region  |
| 0x20030000 | 64KBytes  | the second application's RAM region |

## Build images

Assume you have intalled **aos-cube** tool on your machine. Refer to [Quick Start](https://github.com/alibaba/AliOS-Things/wiki/Quick-Start) for installing aos-cube if you haven't install it.

### Build kernel image

Build kernel image for the developerkit-mk board

```sh
aos make uapp1@developerkit-mk -c config
aos make MBINS=kernel
```

Kernel image will be generated in `out/uapp1@developerkit-mkkernel/binary` directory.

### Build application images

#### uapp1

```sh
aos make uapp1@developerkit-mk -c config
aos make MBINS=app MBINS_APP=app1
```

uapp1 will be generated in `out/uapp1@developerkit-mkapp/binary` directory.

#### uapp2

```sh
aos make uapp2@developerkit-mk -c config
aos make MBINS=app MBINS_APP=app2
```

uapp2 will be generated in `out/uapp2@developerkit-mkapp/binary` directory.

## Downlaod images

You should install **st-link** tool to download kernel and app images to the board. More info about st-link, refer to [**st-link**](https://github.com/texane/stlink) on github.

### Download app images

#### uapp1

flahs uapp1 binary with command:

```sh
st-flash write out/uapp1@developerkit-mkapp/binary/uapp1@developerkit-mk.app.bin 0x08080000 
```

#### uapp2

flash uapp2 binary with command:

```sh
st-flash write out/uapp2@developerkit-mkapp/binary/uapp2@developerkit-mk.app.bin 0x080a0000
```

### Download kernel image

flash kernel binary with the command:

```sh
st-flash write out/uapp1@developerkit-mkkernel/binary/uapp1@developerkit-mk.kernel.bin 0x08000000
```

On developerkit-mk board, I recommand you flash kernel and app images twice, and then reset the chip.

## Useful CLI commands

kernel have registered some CLI commands related to user space application operations, the most useful commands are:

* list all the valid user space applications downloaded on the board

  with command `lsapp`, you can check all the valid user space applications have been flashed on the board.

  ```sh
  # lsapp
  ========== app info ==========
  name                    state
  uapp1                   run
  uapp2                   run
  ```

* check user space application PID

  command `ps` will dump all the running applications name, PID and the total task count of the process

  ```sh
  # ps
  ============ process info ============
  Name                pid       tasks
  uapp2               2         4         
  uapp1               1         4
  ```

* kill an user space application

  to kill a running user space application, issue command `kill PID`, you can check the application's PID number via `process` command

  ```sh
  # kill 2
  process uapp2  is unloaded, pid 2
  ```

* Load an user space application

  Load an application with command `start app_name`, **app_name** is defined with macro `RHINO_CONFIG_APP_NAME` in entry_elf.c. For example, load uapp2 with command：

  ```sh
  # start uapp2
  uapp2 start to run...
  ```


* list user space commands

  Type `help` command in CLI, it wil list all the commands, including user space commands if there are any. For example, the third part commands below are user space commands.

  ```
# help
  ====Build-in Commands====
  ====Support 4 cmds once, seperate by ; ====
  help      : print this
  sysver    : system version
  reboot    : reboot system
  time      : system time
  ota       : system ota
  cpuusage  : show cpu usage
  p         : print memory
  m         : modify memory
  echo      : echo for command
  exit      : close CLI
  devname   : print device name
  ps        : process info
  kill      : kill process

  ====User Commands====
  tasklist  : list all thread info
  dumpsys   : dump system info
  loglevel  : set sync log level
  ifconfig  : start net interface
  kernel_read_g_proc_var: kernel read g_proc_var
  kernel_set_g_proc_var: kernel set g_proc_var
  lsapp     : list all app
  start     : start app

  ====User app Commands====
  uapp2_proc_msg_test: uapp2 proc_msg_test
  uapp2_set_g_proc_var: uapp2 set g_proc_var
  uapp2_read_g_proc_var: uapp2 read g_proc_var
  uapp2_heap: dump uapp1 heap statistics
  uapp1_proc_msg_test: uapp1 proc_msg_test
  uapp1_set_g_proc_var: uapp1 set g_proc_var
  uapp1_read_g_proc_var: uapp1 read g_proc_var
  certificate: run user space certificate test
  uapp1_exit: uapp1 process exit
  uapp1_heap: dump uapp1 heap statistics
  ```
  
  

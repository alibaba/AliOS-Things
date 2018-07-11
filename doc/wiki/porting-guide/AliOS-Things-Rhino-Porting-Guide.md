EN| [中文](AliOS-Things-Rhino-Porting-Guide.zh) 

# Content

- [1 Overview](#1概述)
- [2 System porting](#2系统移植)
- [3 Test of porting target](#3验收测试)
- [4 Porting example based on keil](#4基于keil最小内核移植示例)

------

# 1 Overview

## 1.1 porting target

The system can create a project and call krhino_task_sleep to realize timed printing, such as printing a log every second.

## 1.2 porting content

- CPU architecture related porting (directory platform/arch/*). If CPU support has been completed, please skip this step.
- clock interrupt porting (krhino_tick_proc is needed to call in clock interrupts).

## 1.3 toolchain

 GCC is used to compile by default, and currently mainly for arm architecture.

## 1.4 related directory

Code related to porting are mainly stored in two directories: arch and MCU

- arch

Arch mainly stores files for hardware architecture, such as task switching, startup and switch interruption (interfaces defined in arch/include/port.h).

Example (armv7m): 

```
header file：arch/arm/armv7m/gcc/m4/port*.h，
[code example](https://github.com/alibaba/AliOS-Things/blob/master/platform/arch/arm/armv7m/gcc/m4/port.h)。  
source code：.c file and compiled file in arch/arm/armv7m/gcc/m4/
[code example](https://github.com/alibaba/AliOS-Things/blob/master/platform/arch/arm/armv7m/gcc/m4/)。  
Note: the directory structure under arch is distinguished by CPU architecture. Please refer to the existing directory.
```

- MCU

MCU mainly stores code or binary files provided by vendors, such as system startup, drive, compile / link scripts. Directory structure under MCU is distinguished by “厂商/芯片系列”.

# 2 System porting

## 2.1 Environment setup

### 2.1.1 Code storage

Code related to the hardware architecture is stored in arch directory, such as armv7m.

Code related to startup, peripherals, and driver are stored in MCU directory, such as STM32. 

### 2.1.2 Compilation environment setup

- linux host environment

  Makefile is used to build compilation system in Linux host. You can modify the Makefile files in Rhino root directory according to requirements, and replace the default Linux virtual files with chip related files.

  AOS uses aos-cube toolkit to manage the compilation system. Command example:

    `aos make example_name@board_name`  
  aos-cube install (ubuntu)：  
    `$ sudo apt-get install -y gcc-multilib`  
    `$ sudo apt-get install -y libssl-dev libssl-dev:i386`  
    `$ sudo apt-get install -y libncurses5-dev libncurses5-dev:i386`  
    `$ sudo apt-get install -y libreadline-dev libreadline-dev:i386`    
    `$ sudo apt-get install -y python-pip`  
    `$ sudo pip install aos-cube`

## 2.2 System startup

### 2.2.1 System initialization

System initialization mainly refers to initialization operations for bss, data segment and system clock frequency. Those operations should be completed before system startup and have been completed before porting. For cortex-m4f or cortex-m7f with FPU, we need to confirm whether support for hard floating-point numbers  has been opened. If opened, we must make sure that FPU has been initialized on hardware before starting-up the system.

### 2.2.2 Clock interrupt

- HZ

HZ is the source power of Rhino. It is set by system timer, and usually as 100, which means the system will be interrupted in every 100 second (SysTick timer is used in Cortex-M). Its macro is defined as:

  **_RHINO_CONFIG_TICKS_PER_SECOND_**

Note: When setting a specific hardware timer, you need to use RHINO_CONFIG_TICKS_PER_SECOND to calculate the corresponding interruption time.

- interrupt handling

In order to run Rhino, you need to call krhino_tick_proc. The code example is:

```
  void tick_interrupt(void) {
      krhino_intrpt_enter();
      krhino_tick_proc();
      krhino_intrpt_exit();
  }
```

### 2.2.3 Start kernel

  The code to start kernel is:  

```
  int main(int argc, char **argv) {
      heap_init();
      driver_init();
      krhino_init(); /* app task create */
      krhino_start();
  }
```

Note：

(1) You need to initialize stack when using function main. More details can refer to the subsequent porting of soc_impl.c.

(2) There should not be interruption in driver_init (), or the whole system will crash when krhino_start () is called.  

### 2.2.4 newlib porting

Since newlib porting is universal, it has been unified in utility/libc of the system. All function interfaces that newlib is connected with are in newlib_stub.c, and all porting operations have already been completed, so you just need to add the file. Note: The initialization of newlib needs to call _start.

## 2.3 CPU porting

Note: You can skip this part if the porting of CPU has been done. This chapter mainly focuses on porting to a new CPU.

### 2.3.1 CPU porting

Porting of the core system mainly refers to porting of the interfaces defined in the various port.h file under folder arch, and the related interfaces are described as follows: 

- size_t cpu_intrpt_save(void)  
  It works to stop interruption.
- void cpu_intrpt_restore(size_t cpsr)  
  It works to restore interruption.
- void cpu_intrpt_switch(void)  
  It works to restore the task with highest priority when interruption is switched  It will get the stack pointer of highest priority task and restore the register of it.
- void cpu_task_switch(void)  
  It works to switch tasks. It will save the register of current task, and restore the register of the highest priority task.
- void cpu_first_task_start(void)  
  It works to complete the first task to start the system.
- void *cpu_task_stack_init(cpu_stack_t *base,size_t size, void *arg,task_entry_t entry)  
  It works to initialize the task stack, and size is calculated by word length.
- int32_t cpu_bitmap_clz(uint32_t val)  
  It works to achieve fast lookup of bitmaps, similar to clz instruction in Arm. When RHINO_CONFIG_BITMAP_HW is opened (set as 1), users need CPU-related instructions to use the interface. When it is unopened, software algorithm in Rhino is used by default.
- RHINO_INLINE uint8_t cpu_cur_get(void)  
  The default result of it in port.h is :

```
  RHINO_INLINE uint8_t cpu_cur_get(void) {
      return 0;
  }
```

  Note:  
  **_All the porting interfaces mentioned above needs to be declared in port.h. You can refer to _** [code example](https://github.com/alibaba/AliOS-Things/blob/master/platform/arch/arm/armv7m/gcc/m4/port.h)。

### 2.3.2 Porting of Kernel functions

It means the realization of functional modules in kernel by modifying k_config.h file.
The simplest way is to copy the k_config.h of an existing project (such as arch/arm/armv7m/gcc/m4) .

Besides that, some necessary interfaces defined in the k_soc.h, such as the memory allocation, need to be implemented.

The simplest way is to copy the soc_impl.c of an existing project (such as platform/mcu/stm32l4xx/aos).

What must be realized in soc_impl.c is the configuration of memory allocation g_mm_region, [参考实现](https://github.com/alibaba/AliOS-Things/blob/master/platform/mcu/stm32l4xx/aos/soc_impl.c)。

### 2.3.3 Porting of debug modules

- Porting of serial drivers

The serial port is mainly used to print logs by using printf. Since different chips have different serial drivers,  aos_uart_send is needed to complete the porting.

## 2.4 Porting templates

The main reference of Rhino's porting templates is its existing porting projects. Nowadays, porting of armv5 and Cortex-M Series have been completed. 

## 2.5 Porting of drivers 

Drivers refer to those needed when completing business logic, such as WiFi, BLE and other peripheral ones. It has no direct relationship with kernel, and can refer to the design of HAL. All HAL interfaces are stored under include/hal.

# 3 Test of porting target

The system can create a project and call krhino_task_sleep to realize timed printing, such as printing a log every second.

# 4 Porting example based on keil

target development board : STM32L496G-Discovery，Cotex-m4 

porting target : operate basic tasks, realize krhino_task_sleep in tick clock, print basic serial port

## 4.1 Basic code

### 4.1.1 Drivers in STM32CubeL4 

Download and install STM32CubeMX. Generate STM32L496AGI6 driver, or download STM32CubeL4 software package directly in its official network.

Address to download and update the driver package:

Http://www.st.com/content/st_com/en.html

Search the keyword such as STM32CubeMX or STM32CubeL4.

The usage of STM32CubeMX will not unfolded here, except the following basic generation information:

- check "*Discovery kit with STM32L496AG MCU*"in the homepage of 32L496GDISCOVERY official website, and see the MCU model: STM32L496AGI6
- check Virtual COM port through USART2

![](https://i.imgur.com/Qn0Ijx9.png)

- choose STM32L496AGIx in STM32CubeMX 

![](https://i.imgur.com/0QpmvB3.png)

- Set the related clock in USART2

![](https://i.imgur.com/HRkLZKc.png)

![](https://i.imgur.com/Lq5ftd5.png)

driver generation example : platform\mcu\stm32l4xx

project Keil : projects\Keil\STM32L496G-Discovery\helloworld\

### 4.1.2 Cotex-m4 related code

code example：platform\arch\arm\armv7m\armcc\m4

manily included the interfaces mentioned in 2.3.1

### 4.1.3 rhino source code

source code：kernel\rhino\core

### 4.1.4 code related to startup and initialization

code example：platform\mcu\stm32l4xx\src\STM32L496G-Discovery

mainly includes：

- startup_stm32l496xx_keil.s （initial stack, abnormal vector table）
- stm32l4xx_it.c （exception handling）
- system_stm32l4xx.c （system initialization ）
- soc_init.c （serial port drive; total entrance stm32_soc_init）

### 4.1.5 function main and print task entrance example

example code：example\rhinorun

## 4.2 Modification of basic kernel code

To complete a basic time-delay printing task, the required code modifications include:

- code for basic task processing and scheduling (have been provided in 4.1.2 and 4.1.3)
- Tick clock: SysTick_Handler call krhino to process krhino_tick_proc; HAL_InitTick set tick times per second by using RHINO_CONFIG_TICKS_PER_SECOND.


- soc_init.c (basic driver initialization): implement fputc basic print interface; stm32_soc_init implements the main driver entrance function.

- example\rhinorun implements the main entry function: call krhino_init, stm32_soc_init, krhino_start, , create and start demo tasks.

## 4.3 keil project setup

Based on the path of project Keil, the setup of project needs compiled source files, header files that need to be included, compilation options that need to be used and required link files, etc.

The main configuration items include:



- the .c and .s files that need to be compiled in groups established in "Manage Project Items".


- ![](https://i.imgur.com/ffpFg4W.png)
- Select “Options for Target”. Set compilation options and create header files in “C/C++” ; Select the default link script (or build scatter) in "linker"；Select ST-Linker Debugger in “Debug".
  ![](https://i.imgur.com/0KAoEZz.png)

You can modify other options according to your needs. 

## 4.4 Debug on the board

Burn the code through flash->download, and start debugging on the board through debug->start.

demo_task will print once per tick.

![](https://i.imgur.com/91tdLoL.png)








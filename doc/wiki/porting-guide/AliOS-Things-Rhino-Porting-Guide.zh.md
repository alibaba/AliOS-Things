[EN](Flash-and-KV-Storage-Porting-Guide) | 中文



# 目录

  * [1 概述](#1概述)

  * [2 系统移植](#2系统移植)

  * [3 验收测试](#3验收测试)

  * [4 基于keil最小内核移植示例](#4基于keil最小内核移植示例)

------

# 1概述
## 1.1 移植目标
  系统能启动一个任务并调用krhino_task_sleep函数实现定时打印，例如每秒打印一次日志。  

注：本文档移植的目标主要针对的是kernel最小集的一个移植。

## 1.2 移植内容
  * cpu体系架构相关移植(目录platform/arch/*)，如果使用的cpu已经支持，请忽略此步骤。
  * 时钟中断移植（在时钟中断中需要调用krhino_tick_proc）。

## 1.3 工具链
  系统默认使用gcc编译，目前主要针对arm架构。

## 1.4 相关目录
  移植相关的代码主要存放在**arch**和**mcu**两个目录中：
  * arch  
    该目录主要存放硬件体系架构所需要的移植接口实现文件，  
    如任务切换、启动、开关中断等（即arch/include/port.h中所定义的接口）。  
    示例(armv7m)：  
        头文件：arch/arm/armv7m/gcc/m4/port*.h，[代码示例](https://github.com/alibaba/AliOS-Things/blob/master/platform/arch/arm/armv7m/gcc/m4/port.h)。  
        源代码：arch/arm/armv7m/gcc/m4/下的.c文件和汇编文件，[代码示例](https://github.com/alibaba/AliOS-Things/blob/master/platform/arch/arm/armv7m/gcc/m4/)。  
        注：arch下的目录结构按CPU架构区分，请参照已有目录。
  * mcu  
    该目录主要存放厂商提供的代码或二进制文件，如系统启动、驱动、编译/链接脚本等。mcu下的目录结构按“厂商/芯片系列”进行区分。

# 2系统移植
## 2.1 环境配置
### 2.1.1 代码存放
  硬件体系结构相关的代码存放在arch目录，如armv7m。
  启动、外设及驱动相关的代码存放在mcu目录，如stm32。

### 2.1.2 编译配置

  * linux host环境 
    linux host环境下直接使用Makefile构建编译系统，根据需求修改Rhino根目录下的Makefile文件，将默认的linux模拟文件替换为芯片相关文件。  

    AOS采用aos-cube工具包来管理编译系统，编译命令示例：  
      `aos make example_name@board_name`  
    aos-cube安装配置 (ubuntu)：  
      `$ sudo apt-get install -y gcc-multilib`  
      `$ sudo apt-get install -y libssl-dev libssl-dev:i386`  
      `$ sudo apt-get install -y libncurses5-dev libncurses5-dev:i386`  
      `$ sudo apt-get install -y libreadline-dev libreadline-dev:i386`    
      `$ sudo apt-get install -y python-pip`  
      `$ sudo pip install aos-cube`

## 2.2 系统启动
### 2.2.1 系统初始化
系统初始化主要是指bss、data段的初始化以及系统时钟频率等必须在系统启动之前进行的初始化操作，如配置中断向量表、MCU运行频率等，该部分在移植开始前均已实现。系统运行前，务必保证 bss段的清0动作得到执行。对于cortex-m4f或者cortex-m7f的具备FPU的系列需要确认是否编译打开了硬浮点数的支持，如果打开了务必保证启动前硬件上对FPU做初始化。

### 2.2.2 时钟中断
  * 节拍率（HZ）  
      节拍率是Rhino运行的源动力，其通过系统定时器设置，通常为100，即每秒有100次系统定时中断（cortex-m中使用SysTick定时器），其宏定义为：
        **_RHINO_CONFIG_TICKS_PER_SECOND_**。

  注: 设置具体硬件定时器的时候需要使用RHINO_CONFIG_TICKS_PER_SECOND来计算相应的定时中断时间。

  * 中断处理  
      为了驱动Rhino的运行，需要在中断处理函数中调用krhino_tick_proc这个函数。示例代码如下:  
  ```
  void tick_interrupt(void) {
      krhino_intrpt_enter();
      krhino_tick_proc();
      krhino_intrpt_exit();
  }
  ```
### 2.2.3 内核启动
  内核启动的简易示例代码如下：  
  ```
  int main(int argc, char **argv) {
      heap_init();
      driver_init();
      krhino_init(); /* app task create */
      krhino_start();
  }
  ```
  注意：  
  **_(1) main函数中首先需要初始化堆，具体的注意事项请参考后续soc_impl.c的移植。_**  
  **_(2) driver_init()里面不能产生中断，否则整个系统在krhino_start()调用前会发生崩溃。_**

### 2.2.4 C库移植
  目前系统使用newlib仓库，newlib的移植由于具有通用性，已经统一到系统utility/libc下。newlib对接的所有函数接口在newlib_stub.c里面，所有移植接口都已经移植完，只需要加入该文件即可。需要注意的是对于c库的初始化需要在汇编启动文件中调用_start来完成。

## 2.3 cpu接口移植

  注：如果是使用现有的已经移植的cpu，这一个章节可以跳过，此章节主要是针对移植到一个新的cpu上面去。

### 2.3.1 cpu体系架构移植
  核心系统的移植主要是指实现arch文件夹下各个port.h中所定义的接口，相关接口描述如下：
  * size_t cpu_intrpt_save(void)  
      该接口主要完成关中断的操作，关中断的cpu状态需要返回。

  * void cpu_intrpt_restore(size_t cpsr)  
      该接口主要完成开中断的操作，需要设置现有的cpu状态cpsr。

  * void cpu_intrpt_switch(void)  
      该接口主要完成中断切换时还原最高优先级的任务，需要取得最高优先级任务的栈指针并还原最高优先级任务的寄存器。

  * void cpu_task_switch(void)  
      该接口主要完成任务切换，需要首先保存当前任务的寄存器，然后取得最高优先级任务的栈指针并还原最高优先级任务的寄存器。

  * void cpu_first_task_start(void)  
      该接口主要完成启动系统第一个任务，需要还原第一个任务的寄存器。

  * void *cpu_task_stack_init(cpu_stack_t *base,size_t size, void *arg,task_entry_t entry)  
      该接口主要完成任务堆栈的初始化，其中size以**字长**为单位。

  * int32_t cpu_bitmap_clz(uint32_t val)  
      该接口主要是通过类似Arm中的clz指令实现位图的快速查找，在**RHINO_CONFIG_BITMAP_HW**宏打开（置1）时用户需要使用cpu相关的指令实现该接口，在未打开时默认使用Rhino中的软件算法查找。

  * RHINO_INLINE uint8_t cpu_cur_get(void)  
      该接口在port.h中默认的单核实现如下：
  ```
  RHINO_INLINE uint8_t cpu_cur_get(void) {
      return 0;
  }
  ```
  注意：  
  **_上述所有的移植接口都应在port.h声明，可以参考现有平台的port.h的实现：_** [代码示例](https://github.com/alibaba/AliOS-Things/blob/master/platform/arch/arm/armv7m/gcc/m4/port.h)。
### 2.3.2 内核特性移植
  内核特性移植主要是通过修改k_config.h来使能kernel的功能模块。  
  最简单的方法是拷贝一个现有工程的（例如arch/arm/armv7m/gcc/m4）k_config.h来快速达到移植的目的。  
  除此之外还需要实现k_soc.h里面定义的一些必要的接口，比如内存分配这块。  
  最简单的方法是拷贝一个现有工程的（例如platform/mcu/stm32l4xx/aos）soc_impl.c来快速达到移植的目的。  
  soc_impl.c里面必须要实现的是内存分配这块的配置g_mm_region，[参考实现](https://github.com/alibaba/AliOS-Things/blob/master/platform/mcu/stm32l4xx/aos/soc_impl.c)。

### 2.3.3调试模块移植
* 串口驱动移植  
  串口主要用于打印日志。打印主要是使用printf，因为每个芯片的串口驱动不一样，所以需要对接aos_uart_send这个函数来完成针对printf的移植。

## 2.4 移植模板
Rhino内核的移植模版主要是参照现有的工程的移植。目前移植的有armv5以及cortex-m系列等cpu架构。 

## 2.5 驱动移植
驱动是指完成上层业务逻辑所需要的驱动，如WiFi、BLE等外设驱动，该部分与内核没有直接关系，可参考HAL接口设计。所有HAL接口存放在include/hal下面。

# 3验收测试
系统能启动一个任务并调用krhino_task_sleep函数实现定时打印，例如每秒打印一次日志。

# 4基于keil最小内核移植示例
目标开发板：STM32L496G-Discovery，Cotex-m4架构

移植目标：基本任务运行，tick时钟实现krhino_task_sleep，基本串口打印

## 4.1 基本代码准备
### 4.1.1 STM32CubeL4系列Drivers驱动程序
下载安装STM32CubeMX，选择生成STM32L496AGI6驱动程序，或者官网直接下载STM32CubeL4软件包。

驱动包下载更新地址：

http://www.st.com/content/st_com/en.html

选择搜索STM32CubeMX或者STM32CubeL4关键字。

STM32CubeMX使用此处不详述，提供以下基本生成信息：

  * 查看官网32L496GDISCOVERY主页《Discovery kit with STM32L496AG MCU》 ，查看MCU型号：STM32L496AGI6；

  * 查看Virtual COM port使用USART2 

![](https://i.imgur.com/Qn0Ijx9.png)

  * STM32CubeMX选择STM32L496AGIx系列

![](https://i.imgur.com/0QpmvB3.png)

  * 使能USART2，并配置相关时钟

![](https://i.imgur.com/HRkLZKc.png)

![](https://i.imgur.com/Lq5ftd5.png)

Driver驱动生成样例目录：platform\mcu\stm32l4xx

Keil工程路径：projects\Keil\STM32L496G-Discovery\helloworld\

### 4.1.2 Cotex-m4相关代码

实现代码路径：platform\arch\arm\armv7m\armcc\m4

主要包括2.3.1章节相关接口实现

### 4.1.3 rhino内核源码

源码路径：kernel\rhino\core

### 4.1.4 启动、初始化相关代码

参考代码路径：platform\mcu\stm32l4xx\src\STM32L496G-Discovery

主要包括：

  * startup_stm32l496xx_keil.s （初始堆、栈、异常向量表）

  * stm32l4xx_it.c （异常处理实现）

  * system_stm32l4xx.c （系统初始化SystemInit）

  * soc_init.c （串口驱动、驱动总入口stm32_soc_init）

### 4.1.5 main函数以及打印任务入口样例

参考代码路径：example\rhinorun

## 4.2 基本内核代码修改

目标建立一个基本的延时打印任务，需要的代码修改包括：

  * 基本的任务处理和调度在4.1.2和4.1.3代码中已经提供；

  * tick时钟：SysTick_Handler中断处理调用krhino处理函数krhino_tick_proc；HAL_InitTick设置每秒Tick数时，使用宏RHINO_CONFIG_TICKS_PER_SECOND；

  * soc_init.c基本驱动初始化：实现fputc基本打印接口；stm32_soc_init实现主驱动入口函数。

  * example\rhinorun实现main入口函数：分别调用krhino_init、stm32_soc_init、krhino_start，并创建和启动demo任务。


## 4.3 keil工程配置

基于生成的Keil工程路径，配置需要编译的源文件，需要包含的头文件，需要使用的编译选项，以及需要的链接文件等。

主要配置项包括：

  * “Manage Project Items” 建立相关groups，包含需要编译的.c和.s文件；
      ![](https://i.imgur.com/ffpFg4W.png)

  * 选择工程“Options for Target”，“C/C++”选项卡设置编译选项、建立编译依赖头文件；“Linker”选择默认链接脚本（或者自建立scatter）；“Debug”选择ST-Linker Debugger。
      ![](https://i.imgur.com/0KAoEZz.png)

其他选项请按需调整。

工程建立后编译工程到编译OK。

## 4.4 单板调试

通过keil工程的flash->download完成代码烧写，通过debug->start进行单板启动调试。

demo_task中每tick打印一次，运行OK后如下图所示：

![](https://i.imgur.com/91tdLoL.png)








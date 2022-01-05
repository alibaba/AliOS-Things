@page bsp_development  BSP开发

[更正文档](https://gitee.com/alios-things/documentation/edit/rel_3.3.0/manual/bsp_development.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://help.aliyun.com/document_detail/302301.html)

# 1. BSP目录
## 1.1 chip目录规范
### 1.1.1 目录功能
chip目录用于存放芯片厂家mcu/soc的SDK和适配AliOS Things的代码。
​

建议芯片厂家在提供SDK时只提供必要的代码，如驱动。清理掉不必要的代码，如AliOS Things中已经存在的组件, 这样可以减少后续开源合规的问题和风险，而且可以使代码更清晰。如果需要的SDK中通用组件在AliOS Things不存在， 可以考虑贡献给AliOS Things开源社区。贡献代码请参考 [贡献代码说明](https://haas.iot.aliyun.com/aliosthings/contribute_code.html) 。
​

注意板级相关的代码应放到board目录。


### 1.1.2 命名规范
目录命名应使用业界通用的chip名称，并使用全小写和下划线的组合，如rtl872xd。
​

### 1.1.3 目录结构规范
```
<chip_name>/
|-- aos_adapter       # 适配AliOS Things内核和对接AOS驱动子系统的代码
|-- aos_adapter/csi   # 可选，对接csi驱动时需要
|-- aos_adapter/hal   # 可选，对接hal驱动接口时需要
|-- vendor_sdk        # 芯片厂家原始SDK，建议只包含需要的驱动代码
|-- prebuild          # 预编译的lib和bin文件
|-- package.yaml      # 编译配置文件
|-- README.md         # 芯片说明文档
|-- <chip_name>.ld    # 芯片的链接文件
|-- scripts           # 一些脚本文件

```


### 1.1.4 package.yaml编写规范
package.yaml是AliOS Things的编译配置文件， 作用类似Makefile, 但不支持多级目录，chip目录下只能有一个package.yaml。下面是一个chip的package.yaml的例子
```

## 第一部分： 基础信息
name: rtl872xd                                      # <必选项> 包名称 （符合C语言变量命名规则），长度少于等于64字节
version: master                                     # <必选项> 组件版本号
description: BSP of chip rtl872xd                   # <必选项> 建议至少20字以上
type: chip                                          # <必选项> 组件类型，为：solution, chip, board, common, sdk

## 第二部分： 详细信息
tag: 核心模块                                         # <可选项> 组件分类，缺省值： ''
keywords:                                            # <可选项> 标签，会影响到组件被搜索的效果，合理的标签很重要
  - base
license: Apache license v2.0                         # <可选项> 源代码的许可证，要确保所有代码、文件的许可证不冲突。如：MIT，Apache license v2.0，BSD

## 第三部分： 依赖信息
#           指定该组件依赖的组件及版本，版本支持条件比较，支持：>=v1.0, >v1.0, ==v1.0, <=v1.0, <v1.0, v1.0
#           未指定条件时，默认为 ==，如 v1.0 与 ==v1.0
# depends:                                 # <可选项> 该组件依赖其他的组件，合理的依赖才能保证组件能编译、使用
#   - minilibc: v7.2.0
#   - aos: >=v7.2.0
depends:
  - armv8m: master
  - rhino: master
  - libc_stub: master
  - osal_aos: master
  - i2c: master
  - spi: master
  - flash: master
  - gpio: master
  - uart: master
  - pwm: master
  - adc: master
  - timer: master
  - watchdog: master


## 第四部分：编译连接信息
# build_config:                            # <可选项> 编译配置项
#   include:                               # <可选项> 编译时，影响编译器的-I 参数 ,全局有效
#     - src                                #        include 只能是该软件包下的目录，不能使用外部目录
#   internal_include:                      # <可选项> 编译时，影响编译器的-I 参数 ,组件内有效
#     - include
#   cflag: ''                              # <可选项> C 编译器所需要要的编译参数
#   cxxflag: ''                            # <可选项> CXX 编译器所需要要的编译参数
#   asmflag: ''                            # <可选项> 汇编器所需要要参数
#   define:                                # <可选项> 宏定义, 增加编译器的-D 选项,如：
#     XXX: 1                               #   -DXXX=1
#     AAA: 1                               #   -DAAA
#     STR: "abc"                           #   -DSTR="abc"
#   libs:                                  # 该组件中支持的二进制静态库,如：libxxx.a, libyyy.a
#     - xxx                                #   -lxxx
#     - yyy                                #   -lyyy
#   libpath:                               # 指定静态库所在的路径（相对于该组件路径）
#     - libs                               #   -Llibs
build_config:
  cflag: >
  			-march=armv8-m.main+dsp -mthumb -mfloat-abi=softfp -mfpu=fpv5-sp-d16 -mlittle-endian
        -nostartfiles -DWITH_LWIP -DBT_RTL

  cxxflag: >
        -march=armv8-m.main+dsp -mthumb -mfloat-abi=softfp -mfpu=fpv5-sp-d16 -mlittle-endian
        -std=gnu++11 -DWITH_LWIP

  ldflag: -nostartfiles -specs nosys.specs -Wl,--gc-sections -Wl,-Map="aos.map"
  asmflag: -march=armv8-m.main+dsp -mthumb -mfloat-abi=softfp -mfpu=fpv5-sp-d16  -mlittle-endian -x assembler-with-cpp

  postimage_script: gen_crc_bin.py

  include:
    - aos/include
    - drivers/include

  libs:
    - rtl8721dhp

  libpath:
    - lib

# source_file:                             # <可选项> 指定参与编译的源代码文件，支持通配符，采用相对路径
#   - src/*.c                              # 例：组件 src 目录下所有的扩展名为 c 的源代码文件
source_file:
  - aos/aos_osdep.c
  - aos/soc_impl.c
  - aos/ethernetif.c ? <CONFIG_WIFI>


## 第五部分：配置信息
def_config:
  CONFIG_LWIP_VERSION: "lwip2.0.0"
  CONFIG_LWIP_V200: y
  CONFIG_WIFI: 1
  CLI_IOBOX_ENABLE: 1

## 第六部分：硬件信息
hw_info:
  arch_name: arm
  cpu_name: cortex-m33
  toolchain_prefix: arm-ali-aoseabi
  vendor_name: Realtek
  ld_script: rlx8721d.ld
  flash_program: release/aos_burn_tool/flash_program.py
#  program_data_files:
#     - filename: prebuild/littlefs.bin
#       address: 0x0354000

```
​

详细语法请参考： [构建配置package.yaml](https://haas.iot.aliyun.com/aliosthings/packageyaml_user_manual.html)

更多例子可以参考hardware/chip下其他芯片的package.yaml文件。
​

### 1.1.5 编码规范
芯片厂家原始SDK遵循厂家的编码规范，适配AliOS Things的代码需要遵循AliOS Things的编码规范，请参考：[编码规范](https://haas.iot.aliyun.com/aliosthings/coding.html)。


## 1.2 board目录规范
### 1.2.1 目录功能
hardware/board目录用于存放板级相关启动、配置、初始化代码，以及板级外设驱动。
​

### 1.2.2 命名规范
目录命名应使用业界通用的板子名称，并使用全小写和下划线的组合，如haaseduk1。
​

### 1.2.3 目录结构规范
```
<board_name>
|-- test                     # 可选，板子产测代码
|-- drivers                  # 板级外设驱动
|-- config
|   |-- k_config.c           # 板子对内核hook的实现
|   |-- k_config.h           # 内核配置文件
|   |-- partition_conf.c     # Flash分区配置
|-- startup
|   |-- startup.c            # 板子启动实现
|-- package.yaml             # 编译配置文件
|-- README.md                # 板子说明文档
|-- <board_name>.ld          # 可选，板子链接脚本

```
### 1.2.4 package.yaml编写规范
一个board的package.yaml的例子
```
## 第一部分： 基础信息
name: haaseduk1                           # <必选项> 包名称 （符合C语言变量命名规则），长度少于等于64字节
version: master                           # <必选项> 组件版本号
description: HaaS EDU K1 board            # <必选项> 建议至少20字以上
type: board                               # <必选项> 组件类型，为：solution, chip, board, common

## 第二部分： 详细信息
tag: 核心模块                              # <可选项> 组件分类，缺省值： ''
keywords:                                 # <可选项> 标签，会影响到组件被搜索的效果，合理的标签很重要
  - base
license: Apache license v2.0              # <可选项> 源代码的许可证，要确保所有代码、文件的许可证不冲突。如：MIT，Apache license v2.0，BSD

## 第三部分：依赖信息
#           指定该组件依赖的组件及版本，版本支持条件比较，支持：>=v1.0, >v1.0, ==v1.0, <=v1.0, <v1.0, v1.0
#           未指定条件时，默认为 ==，如 v1.0 与 ==v1.0
# depends:                                 # <可选项> 该组件依赖其他的组件，合理的依赖才能保证组件能编译、使用
#   - minilibc: v7.2.0
#   - aos: >=v7.2.0
depends:
  - haas1000: master

## 第四部分：编译连接信息
# build_config:                            # <可选项> 编译配置项
#   include:                               # <可选项> 编译时，影响编译器的-I 参数 ,全局有效
#     - src                                #        include 只能是该软件包下的目录，不能使用外部目录
#   internal_include:                      # <可选项> 编译时，影响编译器的-I 参数 ,组件内有效
#     - include
#   cflag: ''                              # <可选项> C 编译器所需要要的编译参数
#   cxxflag: ''                            # <可选项> CXX 编译器所需要要的编译参数
#   asmflag: ''                            # <可选项> 汇编器所需要要参数
#   define:                                # <可选项> 宏定义, 增加编译器的-D 选项,如：
#     XXX: 1                               #   -DXXX=1
#     AAA: 1                               #   -DAAA
#     STR: "abc"                           #   -DSTR="abc"
#   libs:                                  # 该组件中支持的二进制静态库,如：libxxx.a, libyyy.a
#     - xxx                                #   -lxxx
#     - yyy                                #   -lyyy
#   libpath:                               # 指定静态库所在的路径（相对于该组件路径）
#     - libs                               #   -Llibs
build_config:
  include:
    - drivers/include
  cflag:
  asmflag:

# source_file:                             # <可选项> 指定参与编译的源代码文件，支持通配符，采用相对路径
#   - src/*.c                              # 例：组件 src 目录下所有的扩展名为 c 的源代码文件
source_file:
  - startup/startup.c
  - config/board.c
  - config/partition_conf.c

## 第五部分：配置信息
def_config:
  WITH_LWIP: 1
  POSIX_DEVICE_IO_NEED: 1
  SYSINFO_PRODUCT_MODEL: "ALI_AOS_HAAS_EDU_K1"
  SYSINFO_DEVICE_NAME: "HAAS_EDU_K1"
  SYSINFO_ARCH: "Cortex-M4F"
  SYSINFO_MCU: "haas1000"
  SYSINFO_BOARD: "haaseduk1"
  BOARD_HAASEDUK1: 1

board:
  chip_name: haas1000

```
详细语法请参考： [构建配置package.yaml](https://haas.iot.aliyun.com/aliosthings/packageyaml_user_manual.html)

更多例子可以参考hardware/board下其他板子的package.yaml文件。
​

### 1.1.5 编码规范
厂家原有外设驱动代码可以遵循厂家的编码规范，适配AliOS Things的代码需要遵循AliOS Things的编码规范，请参考：[编码规范](https://haas.iot.aliyun.com/aliosthings/coding.html)。
# 2. 初始化
## 2.1 系统初始化
系统的标准初始化流程如图

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01NgHh7N1qXL4LfmM3G_!!6000000005505-2-tps-661-516.png" style="max-width:800px;" />
</div>

一个芯片对接的例子：

1. chip init阶段芯片厂家应该实现的，初始化完成后调用board目录下的main函数，具体芯片初始化流程由芯片厂家决定。
1. 板子初始化, 应实现在hardware/board/xxx_board/startup/startup.c
```
int main(int argc, char *argv[])
{
    /* maybe other board level init */

    /* kernel init, malloc can use after this! */
    aos_init();

    /* main task to run */
    aos_task_create(&g_main_task, "main_task", aos_maintask, NULL, NULL,
                    AOS_MAIN_TASK_STACK_SIZE, AOS_MAIN_TASK_PRI, AOS_TASK_AUTORUN);

    /* kernel start schedule! */
    aos_start();

    /* never run here */
    return 0;
}
```

3. maintask 初始化，应实现在solutions/app_xxx/maintask.c。application_start 是应用的入口。
```
void aos_maintask(void* arg)
{
    board_kinit_init(&kinit);
    aos_components_init(&kinit);

#ifndef AOS_BINS
    application_start(kinit.argc, kinit.argv);  /* jump to app entry */
#endif
}
```
## 2.2 驱动初始化
AliOS Things 的中间件和驱动是在aos_components_init这个阶段进行初始化的，aos_components_init会按照以下级别顺序进行驱动的初始化。
```
CORE_DRIVER_ENTRY
BUS_DRIVER_ENTRY
VFS_DRIVER_ENTRY
EARLY_DRIVER_ENTRY
LEVEL1_DRIVER_ENTRY
LEVEL2_DRIVER_ENTRY
LEVEL3_DRIVER_ENTRY
```
一些需要推后初始化的驱动，可以使用下列宏声明。 aos_components_init会在上述驱动初始化，uagent和cli组件初始化之后再进行这些驱动的初始化。
```
POST_DRIVER_ENTRY
```


对于驱动对接来说，只需要根据驱动的初始化顺序要求，选择一个XXXX_DRIVER_ENTRY(your_driver_init_func)，其中your_driver_init_func是驱动对接时需要实现的初始化函数， 函数原型：int (*your_driver_init_func)(void) 。
​

例如芯片对接时在<chip_name>/aos_adapter/uart.c对接AliOS Things的uart驱动子系统并实现初始化函数， 添加初始化申明： LEVEL1_DRIVER_ENTRY(tty_uart_init) 。
​

注意驱动初始化的顺序是按照上述驱动级别进行的，所以对有依赖关系的驱动，不要申明为同一个级别。
# 3. 对接
## 3.1 链接脚本
把下列AliOS Things需要的片段加入芯片链接脚本的.text section中，并可以根据实际需要裁剪。
```
  . = ALIGN (4) ;
  /* AliOS Things cli start */
  _cli_region_begin = .;
  KEEP(*(CliRegion))
  . = ALIGN(4);
  _cli_region_end = .;
  /* AliOS Things cli end */

  /* AliOS Things driver subsystem start*/
  __core_driver_start__ = .;
  KEEP(*(.core_driver_entry))
  . = ALIGN(4);
  __core_driver_end__ = .;

  __bus_driver_start__ = .;
  KEEP(*(*.bus_driver_entry))
  __bus_driver_end__ = .;

  __early_driver_start__ = .;
  KEEP(*(*.early_driver_entry))
  __early_driver_end__ = .;

  __vfs_driver_start__ = .;
  KEEP(*(*.vfs_driver_entry))
  __vfs_driver_end__ = .;

  __level0_driver_start__ = .;
  KEEP(*(*.level0_driver_entry))
  __level0_driver_end__ = .;

  __level1_driver_start__ = .;
  KEEP(*(*.level1_driver_entry))
  __level1_driver_end__ = .;

  __level2_driver_start__ = .;
  KEEP(*(*.level2_driver_entry))
  __level2_driver_end__ = .;

  __level3_driver_start__ = .;
  KEEP(*(*.level3_driver_entry))
  __level3_driver_end__ = .;

  __post_driver_start__ = .;
  KEEP(*(*.post_driver_entry))
  __post_driver_end__ = .;
  /* AliOS Things driver subsystem end. */

  /* For C++ support start */
  . = ALIGN(0x4) ;
  PROVIDE_HIDDEN (__preinit_array_start = .);
  KEEP(*(.preinit_array))
  PROVIDE_HIDDEN (__preinit_array_end = .);

  . = ALIGN(4);
  PROVIDE(__ctors_start__ = .);
  PROVIDE_HIDDEN (__init_array_start = .);
  KEEP(*(SORT(.init_array.*)))
  KEEP(*(.init_array))
  PROVIDE_HIDDEN (__init_array_end = .);
  PROVIDE(__ctors_end__ = .);

  . = ALIGN(4);
  PROVIDE(__dtors_start__ = .);
  PROVIDE_HIDDEN (__fini_array_start = .);
  KEEP(*(SORT(.fini_array.*)))
  KEEP(*(.fini_array))
  PROVIDE_HIDDEN (__fini_array_end = .);
  PROVIDE(__dtors_end__ = .);
  /* For C++ support end */

```


## 3.2 中断对接
krhino_intrpt_enter ：主要功能包括中断栈溢出检查，中断嵌套层级递增。 需要在每次中断进入时调用。
krhino_intrpt_exit ：  主要功能包括中断栈溢出检查，中断嵌套层级递减，触发调度选择当前最高优先级任务。需要在每次中断退出前调用。
例如芯片实现的tick中断处理函数
```
void systick_handler( void )
{
    krhino_intrpt_enter();
    krhino_tick_proc();
    krhino_intrpt_exit();
}
```
​

## 3.3 系统tick
BSP应该实现硬件timer作为系统tick， 根据tick频率设置k_config.h中的RHINO_CONFIG_TICKS_PER_SECOND参数，建议不超过1000。 timer的callback函数应该调用krhino_tick_proc更新系统tick，检查并更新任务状态。 由于中断退出之前会调用krhino_intrpt_exit，触发调度选择当前ready的最高优先级任务运行。
​

## 3.4 硬件timer
如芯片支持硬件timer，则打开RHINO_CONFIG_HW_COUNT配置（默认打开）
芯片对接时需要实现硬件timer的下列三个函数。
​


1. soc_hw_timer_init
硬件timer初始化函数
函数原型在kernel/rhino/include/k_soc.h定义，void soc_hw_timer_init(void) 。
​
2. soc_lr_hw_cnt_get
低精度硬件timer函数 。
函数原型在kernel/rhino/include/k_soc.h定义，lr_timer_t soc_lr_hw_cnt_get(void) 。

3. soc_hr_hw_cnt_get
和高精度硬件timer函数
函数原型在kernel/rhino/include/k_soc.h定义，hr_timer_t soc_hr_hw_cnt_get(void) 。



## 3.5 内核错误处理
在不使用OS的debug组件时，芯片可以通过实现g_err_proc hook函数， 接管内核的错误处理。g_err_proc函数原型定义在kernel/rhino/include/k_err.h中，函数原型
```
typedef void (*krhino_err_proc_t)(kstat_t err);
extern krhino_err_proc_t g_err_proc;
```
芯片对接的一个例子：
```
void soc_err_proc(kstat_t err)
{
    (void)err;

    assert(0);
}

krhino_err_proc_t g_err_proc = soc_err_proc;
```
注意： 如果使能了debug组件， 错误处理会被debug接管，处理之后重启或者停止。则不会走到g_err_proc函数中。
​

## 3.6 内核堆配置
内核rhino提供了堆管理，需要芯片对接的时候需要提供g_mm_region和g_region_num，来定义用作堆管理的内存区域。 内核堆管理功能需要RHINO_CONFIG_MM_TLF配置是打开的（默认打开）。
​

堆内存区域定义方式一般由三种： **链接脚本定义**（推荐）、汇编定义、数组定义。
​


1. 链接脚本中定义堆空间（推荐）：
```
PROVIDE (__heap_start = .);
__heap_limit = ORIGIN(RAM) + LENGTH(RAM);
PROVIDE (__heap_end = __heap_limit);
PROVIDE (__heap_len = __heap_end - __heap_start);
```
这样堆内存区域从当前地址开始，终点为RAM的结尾，把系统中剩余的RAM都分配给内核堆管理。应用可以通过malloc等内存分配函数从堆管理中动态分配需要的内存。


<chip_name>/aos/soc_impl.c中定义g_mm_region和g_region_num
```
#if (RHINO_CONFIG_MM_TLF > 0)

extern void         *__heap_start;
extern void         *__heap_len;

const k_mm_region_t g_mm_region[] = {
    {(uint8_t *)&__heap_start, (size_t)&__heap_len},
};
int g_region_num  = sizeof(g_mm_region)/sizeof(g_mm_region[0]);

#endif
```

2. 汇编文件中定义堆空间
在汇编文件中定义堆空间，此方式并没有将剩余RAM的空间都直接交给堆管理，需要用户自己来调整大小。
```
__heap_len      EQU     0x200
AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_start
__heap_mem        SPACE   __heap_len
__heap_end
```
<chip_name>/aos/soc_impl.c中定义g_mm_region和g_region_num
```
#if (RHINO_CONFIG_MM_TLF > 0)

extern void         *__heap_start;
extern void         *__heap_len;

const k_mm_region_t g_mm_region[] = {
    {(uint8_t *)&__heap_start, (size_t)&__heap_len},
};
int g_region_num  = sizeof(g_mm_region)/sizeof(g_mm_region[0]);

#endif
```


3. 用数组定义堆空间
定义一个堆的数组，注意此方式并没有将剩余RAM的空间都直接交给内核堆管理，需要用户自己来调整大小。
```
#define HEAP_BUFFER_SIZE 1024*30
uint8_t g_heap_buf[HEAP_BUFFER_SIZE];
```
<chip_name>/aos/soc_impl.c中定义g_mm_region和g_region_num
```
#if (RHINO_CONFIG_MM_TLF > 0)

const k_mm_region_t g_mm_region[] = {
    {g_heap_buf, HEAP_BUFFER_SIZE},
};
int g_region_num  = sizeof(g_mm_region)/sizeof(g_mm_region[0]);

#endif
```


## 3.7 内核配置
AliOS Things的内核（称为Rhino）可以通过宏进行功能配置。完整的配置宏可以在"k_default_config.h"文件中看到，里面的宏可分为两类——开关类与数值设置类。开关类负责打开或关闭一个内核模块，数值设置用于设定一些参数。"k_default_config.h"文件位于kernel/rhino/include/k_default_config.h，其对每个可配置宏都进行了默认值的设置。


针对每一个AliOS Things支持的板子，还配套有一个"k_config.h"。其用于设定本单板环境下特定的内核Rhino配置，这些宏配置值通常与"k_default_config.h"值不同。这些"k_config.h"位于hardware/board/xxxx 目录下。


AliOS Things内部组件都是通过#include "k_api.h"来使用这些配置宏的，"k_api.h"中固定包含顺序：


```
#include "k_config.h"
#include "k_default_config.h"
```


所以，单板特定的宏设置优先于默认设置。以信号量功能开关举例：


```
//"k_config.h"中这么描述：
#ifndef RHINO_CONFIG_SEM
#define RHINO_CONFIG_SEM                     1
#endif

//"k_default_config.h"中这么描述
#ifndef RHINO_CONFIG_SEM
#define RHINO_CONFIG_SEM                     0
#endif
```


最终RHINO_CONFIG_SEM生效值为1，即信号量功能打开。
对于"k_config.h"中未出现的Rhino内核配置项，则"k_default_config.h"中默认值生效。
​

## 3.8 内核接口规范
如在BSP对接的时候，需要调用内核的接口，优先使用aos/kernel.h定义的aos接口，如果有缺少的接口可以联系我们。
​

使用aos_init() 初始化内核，它会初始化内核需要的数据结构，内部任务等。在它之前不可调用内核接口，和C库中依赖内核的接口，比如内存动态分配，printf等。
​

使用aos_start() 启动内核， 它会开始任务的调度。
​

## 3.9 其他组件的对接
如果其他组件需要芯片实现对接的，请参考各组件的文档， 如外设驱动的对接， 维测（debug）的对接，OTA的对接。

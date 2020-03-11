== INTRODUCTION ==

The XMC Peripheral Library (XMC Lib) consists of low-level drivers for the XMC product family peripherals.
It addition the Cortex Microcontroller Software Interface Standard (CMSIS) is included. 
CMSIS provides a hardware abstraction layer for the Cortex-M processor series.
XMC Lib is built on top of CMSIS and MISRA-C 2004 compliant. 

The following tool chains are supported:
- GNU GCC for ARM (gcc)
- MDK-ARM Microcontroller Development Kit v5 (armcc)
- Keil Arm Compiler Version 6
- IAR Embedded Workbench for ARM (iccarm)
- TASKING VX-toolset for ARM v5.(carm)

The following 32-Bit Industrial Microcontrollers based on ARM Cortex are supported:
- XMC4800 series
- XMC4700 series
- XMC4500 series
- XMC4400 series
- XMC4300 series
- XMC4200 series
- XMC4100 series
- XMC1400 series
- XMC1300 series
- XMC1200 series
- XMC1100 series

== CONTENTS ==

  Readme.txt
  License.txt
  XMClib/
    doc/
    examples/
    /inc
    /src 
  CMSIS/
    Include/
    Infineon/
      SVD/
      XMC1100_series/
      XMC1200_series/
      XMC1300_series/
      XMC1400_series/
      XMC4100_series/
      XMC4200_series/
      XMC4300_series/
      XMC4400_series/
      XMC4500_series/
      XMC4700_series/
      XMC4800_series/
    Lib/

Now let's explain the purpose of each directory:
 - XMClib: The XMC Peripheral Library   
 - XMClib/doc: Holds complete documentation
 - XMClib/examples: Contains examples using the XMClib for the supported devices and tool chains.
 - XMClib/inc: Include files per peripheral
 - XMClib/src: Implementation files per peripheral
 - CMSIS: The Cortex Microcontroller Software Interface Standard abstraction layer
 - CMSIS/Include: Hardware Abstraction Layer (HAL) for Cortex-M processor registers with standardized definitions for the SysTick, 
                  NVIC, System Control Block registers, MPU registers, FPU registers, and core access functions
 - CMSIS/Infineon: Includes System View Description files (SVD) for use with debuggers, device header files with the register description, 
                   system files and startup files defined by CMSIS
 - CMSIS/Lib: Precompiled CMSIS DSP libraries for XMC4 (ARM Cortex M4F) and XMC1 (ARM Cortex M0) families.
      
== USAGE ==

Several examples for the supported tool chains are provided that can serve as starting point.

To start a project from scratch follow the steps:
1. Copy the CMSIS, XMClib and Newlib folders into your project.
3. Add the following folders into the include paths of your project:
   - ${ProjName}/XMCLib/inc
   - ${ProjName}/CMSIS/Include
   - ${ProjName}/CMSIS/Infineon/XMC4400_series/Include
4. Select the device for which your compiling defining a preprocessor symbol, i.e. XMC4500_F144x1024
5. Include into your source the header files of the peripherals you want to use, i.e. #include <xmc_vadc.h>
6. Configure the peripheral and make use of the APIs described in the documentation.



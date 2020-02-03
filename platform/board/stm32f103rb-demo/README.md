## Overview

**The Stm32f103rb-nucleo** performance line microcontroller incorporates the high-performance ARM® Cortex™-M3 32-bit RISC core operating at a 72 MHz frequency, high-speed embedded memories, and an extensive range of enhanced I/Os and peripherals connected to two APB buses. The STM32F103RC offers three 12-bit ADCs, four general-purpose 16- bit timers plus two PWM timers, as well as standard and advanced communication interfaces: up to two I2Cs, three SPIs, two I2Ss, one SDIO, five USARTs, an USB and a CAN.

## Directories
```sh
stm32f103rb-nucleo # configuration files for board stm32f103rb-nucleo
=============================================================================================================
Dir\File                                Description                                           Necessary for kernel run
=============================================================================================================
|-- drivers                  # board peripheral driver                                              N
|-- config
|   |-- board.h              # board config file, define for user, such as uart port num            Y
|   |-- k_config.c           # user's kernel hook and mm memory region define                       Y
|   |-- k_config.h           # kernel config file .h                                                Y
|   |-- partition_conf.c     # board flash config file                                              N
|-- startup
|   |-- board.c              # board_init implement                                                 Y
|   |-- startup.c            # main entry file                                                      Y
|   |-- startup_gcc.s        # board startup assember for gcc                                       Y
|   |-- startup_iar.s        # board startup assember for iar                                       Y
|   |-- startup_keil.s       # board startup assember for keil                                      Y
|-- stm32f103xb_flash.icf          # linkscript file for iar                                              Y
|-- STM32F103RBTx_FLASH.ld           # linkscript file for gcc                                              Y
|-- stm32f103rb-nucleo.sct          # linkscript file for sct                                              Y
|-- aos.mk                   # board makefile                                                       Y
|-- ucube.py                 # config for CI autorun app                                            N
```

## Board Hardware Resources

* CPU: ARM Cortex M3
* Flash: 128 KB
* RAM: 20 KB

## Pin Mapping

## Driver Support

## Programming

## Debugging

## Update log

## Reference

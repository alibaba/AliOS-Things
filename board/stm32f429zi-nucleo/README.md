## Overview

**Stm32f429zi-nucleo** is hardware board base on [AliOS-Things](https://github.com/alibaba/AliOS-Things), with a variety of peripherals. The STM32F429zi devices are based on the high-performance Arm® Cortex®-M4 32-bit RISC core operating at a frequency of up to 180 MHz. The Cortex-M4 core features a Floating point unit (FPU) single precision which supports all Arm® single-precision data-processing instructions and data types. It also implements a full set of DSP instructions and a memory protection unit (MPU) which enhances application security.

The STM32F429zi device incorporate high-speed embedded memories (Flash memory up to 2 Mbyte, up to 256 Kbytes of SRAM), up to 4 Kbytes of backup SRAM, and an extensive range of enhanced I/Os and peripherals connected to two APB buses, two AHB buses and a 32-bit multi-AHB bus matrix.

The devices offer three 12-bit ADCs, two DACs, a low-power RTC, twelve general-purpose 16-bit timers including two PWM timers for motor control, two general-purpose 32-bit timers. They also feature standard and advanced communication interfaces.

More information about the board can be found at here:

- [AliOS Things github wiki](https://github.com/alibaba/AliOS-Things/wiki)

## Directories

```sh
stm32f429zi-nucleo
├── aos                           # aos interface
├── arch
├── drv
├── ethernetif.c
├── ethernetif.h
├── Fs
├── fsdata_custom.c
├── httpserver-netconn.c
├── httpserver-netconn.h
├── Inc
├── lwipopts.h
├── makefsdata.exe
├── mbmaster_hal                  # the hal of mbmaster
├── Nucleo_F429ZI.ioc
├── README.md
├── Src                           # driver
├── startup_stm32f429xx_keil.s
├── startup_stm32f429xx.s
├── STM32F429.icf
├── STM32F429ZITx_FLASH.ld        # link file
├── STM32F429ZITx.sct
└── ucube.py

```

## Board Hardware Resources

* CPU: STM32F429ZI
    * arch: Cortex-M4 with FPU
    * frequency: 180 MHz
* Flash: 2 M
* SRAM: 256 KB

## Update log

## Reference

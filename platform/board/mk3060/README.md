## Overview

**Mk3060** is hardware board base on [AliOS-Things](https://github.com/alibaba/AliOS-Things), with a variety of peripherals. The chip's mcu is MOC108. The MOC108 is a highly integrated, high-performance, low-power IoT WiFi SOC, which includes ARM 9 core processor and 2.4GHz single frequency WiFi subsystem, and power management unit. The processor's main frequency is as high as 120MHz. At the same time, SoC integrated 256KB SRAM, 2MB Flash. It also contains rich peripheral interfaces such as UART, I2C and SPI. It only needs DC 3.3V voltage, and a single crystal oscillator can work. The WiFi subsystem consists of 802.11b/g/n radio frequency, baseband and multimedia access control (MAC) design to meet low power and high throughput applications.

More information about the board can be found at here:

- [AliOS Things github wiki](https://github.com/alibaba/AliOS-Things/wiki)

## Directories

```sh
mk3060
├── aos.mk
├── board.c
├── board.h
├── boot.bin
├── boot.elf
├── Config.in
├── flash_partitions.c
├── flash_prog.elf
├── k_config.h
├── memory_2ndboot.ld.S
├── memory.ld.S
├── README.md
└── ucube.py
```

## Board Hardware Resources

* CPU: 120 MHz ARM9
* ROM: 2048 KB
* RAM: 256 KB

## Update log

## Reference

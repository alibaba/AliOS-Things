## Overview

**ESP32** is a single 2.4 GHz Wi-Fi-and-Bluetooth combo chip designed with the TSMC ultra-low-power 40 nm technology. It is designed to achieve the best power and RF performance, showing robustness, versatility and reliability in a wide variety of applications and power scenarios.

* support one core with no sram;
* support two cores with no sram; with build cmds adding "dual=1"

More information about the board can be found at here:

- [AliOS Things github wiki](https://github.com/alibaba/AliOS-Things/wiki)

## Directories

```sh
esp32devkit
├── aos.mk
├── arch
│   └── cc.h
├── ble_config.h
├── board.c
├── bt_mesh_opt.h
├── Config.in
├── k_config.h
├── lwipopts.h
├── README.md
├── sdkconfig.h
└── ucube.py
```

## Board Hardware Resources

* CPU: Xtensa® single-/dual-core 32-bit LX6 microprocessor(s), up to 600 MIPS (200 MIPS for ESP32-S0WD, 400MIPS for ESP32-D2WD)
* ROM: 448 KB
* SRAM: 520 KB
* 16 KB SRAM in RTC
* QSPI supports multiple flash/SRAM chips

## Update log

## Reference

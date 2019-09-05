## Overview

**Pca10040** is hardware board base on [AliOS-Things](https://github.com/alibaba/AliOS-Things), with a variety of peripherals. The chip's mcu is nRF52832. The nRF52832 is the mid-range member of the nRF52 Series SoC family. It meets the challenges of a broad range of applications that need Bluetooth 5 feature sets, protocol concurrency and a rich and varied set of peripherals and features. In addition, it offers generous memory availability for both Flash and RAM.

The nRF52832 is fully multiprotocol capable with full protocol concurrency. It has protocol support for Bluetooth 5, Bluetooth mesh, ANT and 2.4 GHz proprietary stacks.

It is built around an ARM® Cortex™-M4 CPU with floating point unit running at 64 MHz. It has NFC-A Tag for use in simplified pairing and payment solutions. It has numerous digital peripherals and interfaces such as PDM and I2S for digital microphones and audio.

More information about the board can be found at here:

- [AliOS Things github wiki](https://github.com/alibaba/AliOS-Things/wiki)

## Directories

```sh
pca10040
├── aos.mk
├── ble_config.h
├── board.c
├── bt_mesh_opt.h
├── Config.in
├── k_config.h
├── pca10040_boot.bin
├── README.md
└── ucube.py
```

## Board Hardware Resources

* CPU: 64 MHz ARM® Cortex-M4F
* FLASH: 512 KB
* RAM: 64 KB
* Flexible and configurable 32 pin GPIO
* Full set of digital interfaces including: SPI/2-wire/I²S/UART/PDM/QDEC with EasyDMA
* 12-bit/200 ksps ADC
* 128-bit AES ECB/CCM/AAR co-processor

## Update log

## Reference

## Contents

## Introduction
supported MCU code here. It includes MCU soc drivers and hal layer API.
Peripheral drivers for board should be added to board directory.

### Features
- MCU supported

### Directories

```sh
Aamcu_demo is a mcu demo for consuling, not a true realization.
|--mcu                                   #supported MCU list
    |--atsamd5x_e5x
    |--bk7231
    |--bk7231u
    |--csky
    |--cy8c4147
    |--cy8c6347
    |--dahua
    |--efm32gxx
    |--es8p508x
    |--esp32
    |--esp8266
    |--freedom-e
    |--imx6
    |--imx6sl_svc
    |--linux
    |--lpc54018
    |--lpc54102
    |--lpc54114
    |--lpc54608
    |--lpc54628
    |--mimxrt1021
    |--mimxrt1052
    |--mkl26z4
    |--mkl27z644
    |--mkl28z7
    |--mkl43z4
    |--mkl81z7
    |--mkl82z7
    |--moc108
    |--msp432p4xx
    |--mx1101
    |--nrf52xxx
    |--r5f100lea
    |--r5f565ne
    |--r7f0c004
    |--rda5981x
    |--rda8955
    |--rtl8710bn
    |--stm32f4xx
    |--stm32f4xx_cube
    |--stm32f7xx
    |--stm32l0xx
    |--stm32l4xx
    |--stm32l4xx_cube
    |--stm32l4xx_syscall
    |--stm32l475
    |--windows
    |--xm510
```
### Standand MCU directories layout
|--mcu
    |--xxx
        |--drivers              : SDK files
        |--hal                  : hal layer for driver
        |--aos.mk               : mcu makefile
        |--README

Attention: If there are several mcu types here, sub mk files are needed here.
### Dependencies

## Reference

## Overview

The ASR6501 is a general LoRa Wireless Communication Chipset, with integrated LoRa Radio Transceiver, LoRa Modem and a 32-Bit RISC MCU. The MCU uses ARM Cortex M0+, with 48MHz operation frequency. The LoRa Radio Transceiver has continuous frequency coverage from 150MHz to 960MHz. The LoRa Modem supports LoRa modulation for LPWAN use cases and (G)FSK modulation for legacy use cases. The LoRa Wireless Communication module designed by ASR6501 provides ultra long range, ultra low power communication for LPWAN application.

ASR6501 is compiled by cypress IDE(PSoc Creator 4.2)

## Feature of Board

## Directories

```sh
asr6510 # configuration files for board asr6510
├── aos.mk
├── board.c
├── board.h
├── inc
├── k_config.h
├── README.md
└── src
```

## Compile and Download Method

ASR6501 is compiled from Creator IDE, the project file for ASR6501 is placed in folder of projects/Creator/ASR6501/alios_small.cydsn, compile steps are descript as follows,

- Download PSoc Creator 4.2 from Cypress Website, and install PSoc Creator

- Open the project file which is named as alios_small.cyprj

- Click the Build->Build All Projects in the tools bar of IDE, and generate the alios_small.hex

- Program alios_small.hex into the board, click Debug->Program in the toolbar

## Board Hardware Resources

## Update log

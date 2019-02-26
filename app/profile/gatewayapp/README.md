### Content
- gatewayapp
    - [Content](#content)
    - [Overview](#overview)
    - [Requirements](#requirements)
    - [Build](#build)
    - [Run](#run)

### Overview

* Industry protocol gateway based on stm32f429zi-nucleo evaluation board. 

### Requirements

Depends on device profile:
* UART scanner device
* Modbus sensors
* CAN bus sensors

### Supported Boards

- stm32f429zi-nucleo

### Build

```sh
aos make menuconfig
aos make
```

### Run

Poweron then watch the logs and watch events/properties in the IoT platform.

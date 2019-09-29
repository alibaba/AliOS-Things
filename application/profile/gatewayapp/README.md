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
# generate gatewayapp@stm32f429zi-nucleo default config
aos make gatewayapp@stm32f429zi-nucleo -c config

# or customize config manually
aos make menuconfig

# build
aos make
```

### Run

Poweron then watch the logs and watch events/properties in the IoT platform.

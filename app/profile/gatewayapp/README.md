## profile/gatewayapp

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

### Build

```sh
aos make gatewayapp@stm32f429zi-nucleo dtc=1 GW_LINKEDGE_WS=1 SAL=1 PT_SCANNER=1
```
* compile vars
** Network module
*** default: Ethernet
*** sal=1: AT WIFI
*** sal=1 module=gprs.sim800: AT GPRS

** Device model
*** PT_SCANNER=1: Scanner 
*** PT_SENSOR=1: MODBUS/CAN sensors

** Cloud link method
*** dtc=1: directly cloud link
*** dtc=1 GW_LINKEDGE_WS=1: link via linkedge server

### Run

Poweron then watch the logs and watch events/properties in the IoT platform.

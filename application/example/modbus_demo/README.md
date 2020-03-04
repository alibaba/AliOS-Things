## Contents

```sh
.
├── aos.mk
├── modbus_demo.c
├── modbus_test.c
├── Config.in
├── README.md
└── ucube.py
```

## Introduction

This example is an demo of using modbus master stack.

### Requirements

You should connecting a modbus slave device to uart2 port of developerkit board. The slave device' address is 0x0, and the slave device can respond fun 0x3 request for register 0x0 and register 0x1.

### Supported Boards

- stm32f429zi-nucleo

### Features

* modbus master demo

### Build

```sh
# generate modbus_app@stm32f429zi-nucleo default config
aos make modbus_app@stm32f429zi-nucleo -c config

# or customize config manually
aos make menuconfig

# build
aos make
```

### Result
```
 ...
simulator1: 18,simulator2: 12
simulator1: 20,simulator2: 14
simulator1: 23,simulator2: 17
```

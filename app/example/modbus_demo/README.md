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

### Features

* modbus master demo

### Build

```sh
aos make clean
aos make modbus_demo@developerkit
```

### Result
```
 ...
simulator1: 18,simulator2: 12
simulator1: 20,simulator2: 14
simulator1: 23,simulator2: 17
```
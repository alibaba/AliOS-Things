# blink sample

## Contents

```sh
blink
├── blink.c         # blink source code
├── Config.in       # kconfig file
├── blink.mk        # aos build system file
└── k_app_config.h  # aos app config file
```

## Introduction

The **blink** example shows how to drive LEDs and use of GPIO input with interrupts on the [supported boards](../../../board) in AliOS-Things, the example will work like this:
* LED0 blink every 1s.
* push button will turn LED1 on/off.

### Requirements

in `blink.c` need to redefine the following macro:
* `GPIO_TRIGGER_IO`(LED0)
* `GPIO_LED_IO`(LED1)
* `GPIO_INPUT_IO`(Button)

### Features

* LED0 blink every 1s.
* push button will turn LED1 on/off.

### Dependencies

* yloop
* cli

### Build

```sh
aos make clean
aos make blink@yourboard
```

> if you want to see AliOS-Things supports boards, click [board](../../../board).

### Install

```sh
aos upload blink@yourboard
```

> if you are not sure is the`aos upload` command supports your board, check [aos upload](../../../build/site_scons/upload).

### Reference

* [【AliOS Things学习笔记】在Developerkit开发板上运行blink例程](https://yq.aliyun.com/articles/669088)
# blink sample

## Contents

```sh
yloop
├── yloop_demo.c         # blink source code
├── maintask.c           # main task source code
├── Config.in            # kconfig file
├── aos.mk               # aos build system file
└── k_app_config.h       # aos app config file
```

## Introduction

The **yloop_demo** example shows how to drive LEDs and use of GPIO input with interrupts on the [supported boards](../../../board) in AliOS-Things, the example will work like this:
* LED0 blink every 1s.
* push button will turn LED1 on/off.

### Requirements

in `yloop_demo.c` need to redefine the following macro:
* `GPIO_TRIGGER_IO`(LED0)
* `GPIO_LED_IO`(LED1)
* `GPIO_INPUT_IO`(Button)

### Features

* LED0 blink every 1s.
* push button will turn LED1 on/off.

### Dependencies

* yloop
* cli

### Supported Boards

- all

### Build

```sh
# generate yloop_demo@developerkit default config
aos make yloop_demo@developerkit -c config

# or customize config manually
aos make menuconfig

# build
aos make
```

> if you want to see AliOS-Things supports boards, click [board](../../../board).

### Install

```sh
aos upload yloop_demo@yourboard
```

> if you are not sure is the`aos upload` command supports your board, check [aos upload](../../../build/site_scons/upload).

### Reference

* https://yq.aliyun.com/articles/669088

### support for st nucleo board
* verified on stm32l476rg nucleo board, other nucleo also can be supported, please refer to the hardware guide for the gpio pin number

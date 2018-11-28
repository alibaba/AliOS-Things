## Contents

```sh
smpapp_4core
├── smpapp_4core.c        # smpapp_4core source code
├── Config.in             # kconfig file
├── smpapp_4core.mk       # aos build system file(for make)
├── k_app_config.h        # aos app config file
└── ucube.py              # aos build system file(for scons)
```

## Introduction

The **smpapp_4core** example will print `hello world` and other information every 5 seconds.
It is only used in four CPU cores' system, which need to run smp schedule.

### Dependencies

* yloop
* cli

### Build

```sh
aos make clean
aos make smpapp_4core@yourboard
```

> if you want to see AliOS-Things supports boards, click [board](../../../board).

### Install

### Result

## Reference

* https://yq.aliyun.com/articles/589967

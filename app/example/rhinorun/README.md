## Contents

```sh
rhinorun
├── rhinorun.c      # rhinorun source code
├── Config.in       # kconfig file
├── rhinorun.mk     # aos build system file(for make)
├── k_app_config.h  # aos app config file
└── ucube.py        # aos build system file(for scons)
```

## Introduction

The **rhinorun** example is used for rhino project, which does not repy on yloop but only task、tick and printf. Use micro CONFIG_AOS_CLI to open cli.

### Dependencies

* cli

### Build

```sh
aos make clean
aos make rhinorun@yourboard
```

> if you want to see AliOS-Things supports boards, click [board](../../../board).

### Install

### Result

## Reference

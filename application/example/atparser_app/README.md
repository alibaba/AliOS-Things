## Contents

```sh
atparser_app
├── atparser_app.c  # atparser_app source code
├── Config.in       # kconfig file
├── aos.mk          # aos build system file(for make)
├── k_app_config.h  # aos app config file
└── ucube.py        # aos build system file(for scons)
```

## Introduction

The **atparser_app** example aims to show how to use atparser component to ease the use of AT modules.

### Dependencies

* atparser

### Supported Boards

- all

### Build

```sh
# generate atparser_app@developerkit default config
aos make atparser_app@developerkit -c config

# or customize config manually
aos make menuconfig

# build
aos make
```

> if you want to see AliOS-Things supports boards, click [board](../../../board).

### Install

```sh
aos upload
```

> if you are not sure is the`aos upload` command supports your board, check [aos upload](../../../build/site_scons/upload).

### Result

```sh
```

## Reference

* [Quick-Start](https://github.com/alibaba/AliOS-Things/wiki/Quick-Start)

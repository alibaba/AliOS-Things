## Contents

```sh
kernel_app
├── kernel_app.c    # kernle app source code
├── Config.in       # kconfig file
├── aos.mk          # aos build system file(for make)
├── k_app_config.h  # aos app config file
└── ucube.py        # aos build system file(for scons)
```

## Introduction

The **kernel_app** example shows how to use the kernel interface.

### Dependencies

* cli

### Supported Boards

- all

### Build

```sh
# generate kernel_app@developerkit default config
aos make kernel_app@developerkit -c config

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

## Reference

* [Quick-Start](https://github.com/alibaba/AliOS-Things/wiki/Quick-Start)

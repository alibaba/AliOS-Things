## Contents

```sh
amp
├── amp.c           # amp source code
├── Config.in       # kconfig file
├── aos.mk          # aos build system file(for make)
├── k_app_config.h  # aos app config file
└── ucube.py        # aos build system file(for scons)
```

## Introduction

The **amp** example will start `AliOS Things mini program framwork` service.

### Dependencies

* yloop
* cli
* amp_sdk_c

### Supported Boards

- esp32

### Build

```sh
# generate helloworld@developerkit default config
aos make amp@esp32devkitc -c config

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


## Reference

* [Quick-Start](https://github.com/alibaba/AliOS-Things/wiki/Quick-Start)

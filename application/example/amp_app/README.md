## Contents

```sh
amp_app
├── amp_app.c
├── aos.mk
├── autobuild.json
├── Config.in
├── k_app_config.h
├── maintask.c
├── README.md
└── ucube.py
```

## Introduction

The **amp** example will start `AliOS Things mini program framwork` service.

### Dependencies

* osal_aos
* kv
* littlefs
* cli
* amp

### Supported Boards

- haas100

### Build

```sh
# generate amp_app@haas100 default config
aos make amp_app@haas100 -c config

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

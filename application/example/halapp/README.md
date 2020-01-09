## Contents

```sh
halapp
├── halapp.c    # helloworld source code
├── Config.in       # kconfig file
├── aos.mk          # aos build system file(for make)
├── k_app_config.h  # aos app config file
└── ucube.py        # aos build system file(for scons)
```

## Introduction

The **halapp** example will show hal API calling demos. As the gpio pins used in demo are not in common use for all boards, so the halapp demo is just used for reference but not for run.

### Dependencies


### Supported Boards

- all

### Build

```sh
# generate halapp@developerkit default config
aos make halapp@developerkit -c config

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

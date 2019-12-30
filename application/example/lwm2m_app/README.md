## Contents

```sh
lwm2mapp
├── lwm2m_example.c # lwm2m app
├── Config.in       # kconfig file
├── aos.mk          # aos build system file(for make)
├── k_app_config.h  # aos app config file
└── ucube.py        # aos build system file(for scons)
```

## Introduction

The **lwm2mapp** example will run a simple lwm2m example.

### Dependencies

* yloop
* cli

### Supported Boards

- all

### Build

```sh
aos make clean
aos make lwm2mapp@yourboard
```

> if you want to see AliOS-Things supports boards, click [board](../../../board).

### Install

```sh
aos upload lwm2mapp@yourboard
```

> if you are not sure is the`aos upload` command supports your board, check [aos upload](../../../build/site_scons/upload).

### Result

```sh
 [   0.000]<V> AOS [aos_framework_init#17] : aos framework init.
 [   0.000]<V> AOS [application_start#15] : application started.
```

## Reference

* [Quick-Start](https://github.com/alibaba/AliOS-Things/wiki/Quick-Start)

## Contents

```sh
krhino_api_app
├── aos.mk          # aos build system file(for make)
├── bufqueue_app.c  # the demo of how to use bufqueue
├── Config.in       # kconfig file
├── event_app.c     # the demo of how to use event
├── krhino_api_main.c  # the main entry of krhino kernel API
├── mutex_app.c     # the demo of how to use mutex
├── queue_app.c     # the demo of how to use queue
├── sem_app.c       # the demo of how to use semaphore
├── task_app.c      # the demo of how to create task
├── task_asyndelete.c  # the demo of how to delete tasks asynchronously
├── timer_app.c        # the demo of how to delete softeware timer
└── ucube.py           # aos build system file(for scons)
└── work_app.c         # the demo of how to use workqueue
```

## Introduction

The **krhino_api_demo** example shows how to use the krhino API. This set of interfaces
is the native kernel interface to AliOS Things and is typically used by internal modules and drivers.

### Dependencies

* cli

### Supported Boards

- all

### Build

```sh
# generate default config
aos make kernel_demo.krhino_api_demo@stm32f429zi-nucleo -c config

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

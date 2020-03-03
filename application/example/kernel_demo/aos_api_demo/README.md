## Contents

```sh
aos_api_app
├── aos_api_main.c       # application main entry of aos kernel API
├── aos.mk               # aos build system file(for make)
├── Config.in            # kconfig file
├── mem_app
│   ├── mem_alloc.c      # the demo of how to allocates memory
│   └── mem_realloc.c    # the demo of how to expand the allocated memory
├── mutex_app.c          # the demo of mutex
├── queue_app.c          # the demo of queue
├── sem_app.c            # the demo of sem
├── task_app
│   ├── task_key.c       # the demo of how to use task private variables
│   ├── task_new.c       # the demo of how to create task using aos_task_new()
│   └── task_new_ext.c   # the demo of how to create task using aos_task_new_ext()
├── time_app.c           # the demo of how to use system time API
├── timer_app
│   ├── timer_change.c   # the demo of how to change the parameters of the timer
│   ├── timer_new.c      # the demo of how to create timer using aos_timer_new()
│   └── timer_new_ext.c  # the demo of how to create timer using aos_timer_new_ext()
├── ucube.py             # aos build system file(for scons)
└── work_app
    ├── work_cancel.c    # the demo of how to cancel the work
    ├── work_new.c       # the demo of how to create the work
    └── workqueue_new.c  # the demo of how to create the workqueue
```

## Introduction

The **aos_api_app** example shows how to use the aos API. This set of interfaces is
provided to the application layer.

### Dependencies

* cli

### Supported Boards

- all

### Build

```sh
# generate default config
aos make kernel_demo.aos_api_demo@stm32f429zi-nucleo -c config

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

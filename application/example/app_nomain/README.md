## Contents

```sh
appdemo
├── main.c          # helloworld source code
├── Config.in       # kconfig file
├── aos.mk          # aos build system file(for make)
├── k_app_config.h  # aos app config file
└── ucube.py        # aos build system file(for scons)
```

## Introduction

The **helloworld** example will print `hello world` and other information every 5 seconds, the example is simple.

### Dependencies

* yloop
* cli

### Supported Boards

- all

### Build

```sh
# generate helloworld@developerkit default config
aos make helloworld@developerkit -c config

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
 [   0.000]<V> AOS [aos_framework_init#17] : aos framework init.
 [   0.000]<V> AOS [application_start#15] : application started.
 [   1.000]<V> AOS [app_delayed_action#9] : helloworld app_delayed_action:9 app_task

 [   6.000]<V> AOS [app_delayed_action#9] : helloworld app_delayed_action:9 app_task

 [  11.000]<V> AOS [app_delayed_action#9] : helloworld app_delayed_action:9 app_task

 [  16.000]<V> AOS [app_delayed_action#9] : helloworld app_delayed_action:9 app_task
```

## Reference

* [Quick-Start](https://github.com/alibaba/AliOS-Things/wiki/Quick-Start)

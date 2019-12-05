## Contents

```sh
sal_app
├── sal_app.c       # sal app source code
├── Config.in       # kconfig file
├── aos.mk          # aos build system file(for make)
├── k_app_config.h  # aos app config file
└── ucube.py        # aos build system file(for scons)
```

## Introduction

The **sal_app** example will demonstrate how to use SAL component to do TCP/UDP connection.

### Dependencies

* yloop
* cli
* netmgr
* sal

### Supported Boards

- all boards which support SAL.

### Build

```sh
# generate sal_app@developerkit default config
aos make sal_app@developerkit -c config

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

### Run the Test

```sh
netmgr connect <AP_ssid> <AP_password>
```

After the board gets IP, it will start TCP and UDP connection to server.

### Result

See device log.

## Reference

* [Quick-Start](https://github.com/alibaba/AliOS-Things/wiki/Quick-Start)

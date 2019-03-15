## Content

```
middleware/uagent/udev/
├── aos.mk
├── include
│   └── udev.h
├── udev.c                  # udev
├── udev_mqtt_client.c      # mqtt client
├── udev_mqtt_client.h
├── udev_net.c              # network interface
└── udev_net.h
```

## Introduction

[中文](./README-zh.md)
**udev** is a debugging component with Internet thinking, It simplifies the development, upgrade remote devices with one click or one command. No need the device to connect to the PC with the serial port or debugging interface, only need the device access the Internet. **udev** is designed for engineers who develop up-applications of IoT devices, quickly and efficiently develop IoT applications without much care about the drivers and architecture.

> **udev is only used for equipment development and debugging, need to diabled when firmware release.**

##### udev quick-start

The `aos ota` command upgrade remote devices quickly:

![](https://img.alicdn.com/tfs/TB1GINADwTqK1RjSZPhXXXfOFXa-919-571.gif)

> need to update `aos-cube` to 0.3.0 or later.

Upgrade remote devices quickly in `AliOS Studio`:

![](https://img.alicdn.com/tfs/TB14ROOHbrpK1RjSZTEXXcWAVXa-1140-820.gif)

> need to update `AliOS Studio` to 0.10.9 or later.

##### udev architecture Overview

![](https://img.alicdn.com/tfs/TB1NMk4DlLoK1RjSZFuXXXn0XXa-526-329.png)

##### enable udev in `menuconfig`

```
aos make menuconfig
```

choose：
`Middleware Configuration ` > `uAgent Configuration` > `udev`

## Feature

- No need the device to connect to the PC with the serial port or debugging interface.
- Able to upgrade remote devices.
- Upgrading remote devices with one command: `aos ota`.
- Upgrading remote devices with one click in [AliOS-Studio](https://marketplace.visualstudio.com/items?itemName=alios.alios-studio).

### Supported Boards

- developerkit
- esp8266
- esp32devkitc
- mk3060
- mk3080

## Dependencies

- linkkit_sdk_c
- cjson
- ota

## API

```c
int udev_init(const char *pk, const char *dn);
```

|params|description|
|:---:|:---:|
|`const char *pk`| product key|
|`const char *dn`| device name|

## Sample code

sample code: [udevapp](../../../app/example/udevapp).

```c

#include "udev.h"

static void udev_app(void *unused)
{
    udev_init(PRODUCTKEY, DEVICENAME);
    return;
}

static void wifi_service_event(input_event_t *event, void *priv_data)
{
    .....
    aos_task_new("udev_app", udev_app, NULL, 2048);
    .....
}
```

## Others

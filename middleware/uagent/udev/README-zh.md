## 目录

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

## 介绍

**udev** 是一个互联网思维的调试组件，主要是简化开发过程，可以实现一键更新设备的bin文件，也支持远程更新，无需设备通过串口或者调试接口接入到PC，只需要设备能够接入互联网。**udev** 主要面向开发物联网设备端上层应用的开发工程师，无需过多的关心设备底层驱动和原理，就能快速高效的开发物联网应用。

> **该组件仅用于设备开发调试阶段，量产时需要把这个功能禁掉。**

##### udev 的开发体验

使用`aos ota`命令就能快速更新设备固件：

![](https://img.alicdn.com/tfs/TB1GINADwTqK1RjSZPhXXXfOFXa-919-571.gif)

> 需要更新`aos-cube`到0.3.0版本及以上。

配合`AliOS Studio`一键更新设备固件：

![](https://img.alicdn.com/tfs/TB14ROOHbrpK1RjSZTEXXcWAVXa-1140-820.gif)

> 需要更新`AliOS Studio`到0.10.9版本及以上。

##### udev 的架构原理

![](https://img.alicdn.com/tfs/TB1NMk4DlLoK1RjSZFuXXXn0XXa-526-329.png)

##### `menuconfig` 中启用udev

```
aos make menuconfig
```

选择：
`Middleware Configuration ` > `uAgent Configuration` > `udev`

## 特性

- 无需设备通过串口或者调试接口接入到PC，就能够更新设备固件
- 支持远程更新
- 结合`aos-cube`，一条指令更新设备固件
- 结合[AliOS-Studio](https://marketplace.visualstudio.com/items?itemName=alios.alios-studio)，一键更新设备固件

## 依赖

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

## 示例代码

simple code: [udevapp](../../../app/example/udevapp).

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

## 其他

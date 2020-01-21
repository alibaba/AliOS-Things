## Contents

## Introduction

The **sensor\_cloud\_demo** is a test example for Sensor Hal Interface.

Users can initialize the sensor driver, read the sensor data. On the boards which support linkkit, the sensor data can be sent to the cloud.
The test example flow is as follows.

![sensor_002_01.PNG](https://img.alicdn.com/tfs/TB1EDgeQmrqK1RjSZK9XXXyypXa-1190-763.png)


### Features

- sensor data read
- sensor data to cloud

### Directories

```sh
sensor_cloud_demo
├─linkkit
│  ├─app_entry.c               # init function for data to cloud through linkkit
│  └─linkkit_example_solo.c    # callback function for data to cloud through linkkit
└─sensor_cloud_demo.c          # sensor data connect to cloud example
```

### Dependencies

- drivers.sensor(necessary)
- utility.cjson(necessary)
- linkkit_sdk_c(necessary)
- netmgr(necessary)

### Supported Boards

- developerkit

### Build & Flash

```sh
# generate udata_demo.sensor_cloud_demo@developerkit default config
aos make udata_demo.sensor_cloud_demo@developerkit -c config

# or customize config manually
aos make menuconfig

# build
aos make

# upload
aos upload
```

### Others

When the data links to cloud, it's recommended to use the linkdevelop platform which is designed for developers.
You can visit the linkdevelop platform through the following website:

- https://linkdevelop.aliyun.com/

The development guide is blow:

- https://linkdevelop.aliyun.com/platform-doc#index.html
- https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-Linkkit-Application.zh#21-%E4%BF%AE%E6%94%B9%E8%AE%BE%E5%A4%87%E5%87%AD%E8%AF%81

## Reference

- [AliOS Things uData Sensor Driver Porting Guide](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-uData-Sensor-Driver-Porting-Guide)

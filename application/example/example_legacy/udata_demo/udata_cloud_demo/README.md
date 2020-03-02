## Contents

## Introduction

The **udata\_cloud\_demo** is a test example for uData which is an IoT oriented sensoring device processing framework integrating both characteristics of actual IoT application scenarios and AliOS Things itself.

Users can subscribe the uData service if they need as it shows in the udata_cloud_demo. On the boards which support linkkit, the sensor data can link to the cloud.


The test example flow is as follows.


![udata_002_01.PNG](https://img.alicdn.com/tfs/TB1QjQzQcfpK1RjSZFOXXa6nFXa-1275-727.png)<br />

### Features

- service subscribe/unsubscribe
- data to cloud

### Directories

```sh
udata_cloud_demo
├─linkkit
│  ├─app_entry.c               # init function for data to cloud through linkkit
│  └─linkkit_example_solo.c    # callback function for data to cloud through linkkit
└─udata_cloud_demo.c           # uData connect to cloud example code
```

### Dependencies

- drivers.sensor(necessary)
- middleware.udata(necessary)
- utility.cjson(necessary)
- libiot_devmodel(necessary)
- netmgr(necessary)

### Supported Boards

- developerkit

### Build & Flash

```sh
# generate  udata_demo.udata_cloud_demo@developerkit default config
aos make udata_demo.udata_cloud_demo@developerkit -c config

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

- [AliOS Things uData Framework Porting Guide](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-uData-Framework-Porting-Guide)

- [AliOS Things uData Sensor Driver Porting Guide](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-uData-Sensor-Driver-Porting-Guide)

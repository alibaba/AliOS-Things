## Contents

## Introduction

The **uDataapp** is a test example for uData which is an IoT oriented sensoring device processing framework integrating both characteristics of actual IoT application scenarios and AliOS Things itself.

Users can subscribe the uData service if they need as it shows in the uDataapp. On the boards which support linkkit, the sensor data can link to the cloud.

### Features

- service subscribe/unsubscribe
- data to cloud(optional)

### Directories

```sh
udata
├── data
│   └── solo_tsl.data            # TSL string for data to cloud
├── linkkit_example_solo.c       # callback function for data to cloud
└── uData-example.c              # uData example code
```

### Dependencies

- device.sensor(necessary)
- middleware.udata(necessary)
- middleware.uagent.uota(optional)
- middleware.linkkit(optional)

### Others

When the data links to cloud, it's recommended to use the linkdevelop platform which is designed for developers.
You can visit the linkdevelop platform through the following website:

- https://linkdevelop.aliyun.com/

The development guide is blow:

- https://linkdevelop.aliyun.com/developGuide#index.html
- https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-Linkkit-Application.zh#21-%E4%BF%AE%E6%94%B9%E8%AE%BE%E5%A4%87%E5%87%AD%E8%AF%81

## Reference

- [AliOS Things uData Framework Porting Guide](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-uData-Framework-Porting-Guide)

- [AliOS Things uData Sensor Driver Porting Guide](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-uData-Sensor-Driver-Porting-Guide)

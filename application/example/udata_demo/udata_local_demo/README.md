## Contents

## Introduction

The **udata\_local\_demo** is a test example for uData which is an IoT oriented sensoring device processing framework integrating both characteristics of actual IoT application scenarios and AliOS Things itself.

Users can subscribe the uData service if they need and prit it as it shows in the udata_local_demo.

The test example flow is as follows.

![udata_001_01.PNG](https://img.alicdn.com/tfs/TB1ZK7oQkvoK1RjSZPfXXXPKFXa-607-549.png)

### Features

- service subscribe/unsubscribe
- sensor data local print

### Directories

```sh
udata_local_demo
└─udata_example.c              # uData local test example code
```

### Dependencies

- drivers.sensor(necessary)
- middleware.udata(necessary)
- utility.cjson(optional)

### Supported Boards

- developerkit

### Build & Flash

```sh
# generate udata_demo.udata_local_demo@developerkit default config
aos make udata_demo.udata_local_demo@developerkit -c config

# or customize config manually
aos make menuconfig

# build
aos make

# upload
aos upload
```

## Reference

- [AliOS Things uData Framework Porting Guide](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-uData-Framework-Porting-Guide)

- [AliOS Things uData Sensor Driver Porting Guide](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-uData-Sensor-Driver-Porting-Guide)

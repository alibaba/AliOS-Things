## Contents

## Introduction

The **sensor\_local\_demo** is a test example for Sensor Hal Interface.

Users can initialize the sensor driver, read the sensor data and print it. 

The test example flow is as follows.

![sensor_001.PNG](https://img.alicdn.com/tfs/TB15e7oQkvoK1RjSZPfXXXPKFXa-395-692.png)

### Features

- sensor data read
- sensor data local print

### Directories

```sh
sensor_local_demo
└─sensor_local_demo.c          # sensor local test example
```
### Dependencies

- drivers.sensor(necessary)

### Supported Boards

- developerkit

### Build & Flash

```sh
# generate udata_demo.sensor_local_demo@developerkit default config
aos make udata_demo.sensor_local_demo@developerkit -c config

# or customize config manually
aos make menuconfig

# build
aos make

# upload
aos upload
```


## Reference

- [AliOS Things uData Sensor Driver Porting Guide](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-uData-Sensor-Driver-Porting-Guide)

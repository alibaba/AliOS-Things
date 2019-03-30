## Contents

## Introduction

The **sensor\_local\_demo** is a test example for Sensor Hal Interface.

Users can initialize the sensor driver, read the sensor data and print it. On the boards which support linkkit, the sensor data can be sent to the cloud.

### Features

- sensor data read
- sensor data to cloud

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
aos make udata_demo.sensor_local_demoudataapp@developerkit -c config

# or customize config manually
aos make menuconfig

# build
aos make

# upload
aos upload
```


## Reference

- [AliOS Things uData Sensor Driver Porting Guide](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-uData-Sensor-Driver-Porting-Guide)

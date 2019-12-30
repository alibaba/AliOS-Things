## Contents

## Introduction
**uData** is designed based on traditional sensorhub. It is an IoT oriented sensoring device processing framework integrating both characteristics of actual IoT application scenarios and AliOS Things itself.

There are two layers in uData Framework. One is kernel layer, mainly responsible for sensor driving and related static calibration, including axial calibration. The other is framework layer, mainly responsible for application service management, dynamic calibration management and external module interfaces.

### Features
- Sensor framework
- Support sensor driver
- Static calibration
- Dynamic calibration algorithm
- Data to cloud

### Directories

```sh
udata
├── abs_data_model  # abs layer
├── cali_data       # calibration algorithm
├── include
├── service         # application services
└── service_mgr     # service manager
```

### Dependencies
- device.sensor

## Reference

- [AliOS Things uData Framework Porting Guide](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-uData-Framework-Porting-Guide)

- [AliOS Things uData Sensor Driver Porting Guide](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-uData-Sensor-Driver-Porting-Guide)

- https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-uData%E7%AC%AC%E4%B8%89%E6%96%B9%E7%AE%97%E6%B3%95%E7%A7%BB%E6%A4%8D%E8%AF%B4%E6%98%8E%E6%96%87%E6%A1%A3

- https://github.com/alibaba/AliOS-Things/wiki/%E4%BC%A0%E6%84%9F%E5%99%A8%E6%95%B0%E6%8D%AE%E4%B8%80%E9%94%AE%E5%BC%8F%E4%B8%8A%E4%BA%91%E7%9A%84%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E

- https://github.com/alibaba/AliOS-Things/wiki/%E5%9F%BA%E4%BA%8EAliOS-Things-Developer-Kit%E5%BC%80%E5%8F%91%E6%9D%BF%E7%9A%84%E5%A4%96%E6%8E%A5%E4%BC%A0%E6%84%9F%E5%99%A8%E5%BC%80%E5%8F%91%E6%95%99%E7%A8%8B
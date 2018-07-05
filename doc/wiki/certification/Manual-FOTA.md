EN|[中文](Manual-FOTA.zh)

# AliOS Things FOTA Self Test Guide

- [MQTT Channel FOTA Self Test Guide](#mqtt)
- [WSF Channel FOTA Self Test Guide](#wsf)
- [LinkKit Channel FOTA Self Test Guide](#linkkit)

<a id="mqtt"></a>

## MQTT Channel FOTA  Self Test Guide

### 1. Create upgrade firmware

Create an upgrade firmware and get its version number

```
$ aos make clean && aos make mqttapp@{board}
```

record version number as `app-1.0.0-20180319.2129`
![](assets/fota_mqtt_1.png)

### 2. Upload upgrade firmware

Log in on [IoT Suite platform](iot.console.aliyun.com)，`固件升级` -> `添加固件`
![](assets/fota_mqtt_2.png)
![](assets/fota_mqtt_3.png)

> The`固件版本号` must be correct or will lead to upgrade failure

### 3. Start upgrade 

Log in on [IoT Suite platform](iot.console.aliyun.com)，and click on` 验证固件`
![](assets/fota_mqtt_4.png)
![](assets/fota_mqtt_5.png)

> `版本号` refers to current version number 
> `deviceName` is name of the device to be upgraded 

You can see upgrade process through serial port. After upgrade, the device will restart, and report new version number.
![](assets/fota_mqtt_6.png)

<a id="wsf"></a>

## WSF Channel FOTA Self Test Guide

<a id="linkkit"></a>

## LinkKit Channel FOTA Self Test Guide

### 1. Create upgrade firmware

Create an upgrade firmware and get its version number

```
$ aos make clean && aos make linkkitapp@{board}
```

record version number as`app-1.0.0-20180320.1310`
![](assets/fota_linkkit_1.png)

### 2. Upload upgrade firmware

Log in on [IoT Suite platform](iot.console.aliyun.com)，`固件升级` -> `新增固件`
![](assets/fota_linkkit_2.png)
![](assets/fota_linkkit_3.png)

> The`固件版本号` must be correct or will lead to upgrade failure

### 3. Start upgrade 

> **Scenario**：upgrade the device (version number`app_1.0.0_20180319.1020`，device name `test_light_08` to `app_1.0.0_20180320.1310`

Log in on [develop platform](https://linkdevelop.aliyun.com)，select the firmware to be upgraded, and click on `验证固件`
![](assets/fota_linkkit_4.png)
![](assets/fota_linkkit_5.png)

You can see upgrade process through serial port. After upgrade, the device will restart, and report new version number.
![](assets/fota_linkkit_6.png)
![](assets/fota_linkkit_7.png)

It has been upgraded to `app_1.0.0_20180320.1310`.
![](assets/fota_linkkit_8.png)
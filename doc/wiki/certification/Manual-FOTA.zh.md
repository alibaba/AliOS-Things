[EN](Manual-FOTA)|中文

# AliOS Things FOTA 自测指南

* [MQTT 通道 FOTA 自测指南](#mqtt)
* [WSF 通道 FOTA自测指南](#wsf)
* [LinkKit 通道 FOTA自测指南](#linkkit)

<a id="mqtt"></a>

## MQTT 通道 FOTA 自测指南
### 1. 制作升级固件
制作升级固件，获取固件版本号
```
$ aos make clean && aos make mqttapp@{board}
```
记录版本号为`app-1.0.0-20180319.2129`
![](assets/fota_mqtt_1.png)

### 2. 上传升级固件
登录[物联网套件控制台](iot.console.aliyun.com)，`固件升级` -> `添加固件`
![](assets/fota_mqtt_2.png)
![](assets/fota_mqtt_3.png)
> 填写的`固件版本号`必须与上传的固件真实版本号一致，否则会导致升级失败

### 3. 触发升级
登录[物联网套件控制台](iot.console.aliyun.com)，点击`验证固件`
![](assets/fota_mqtt_4.png)
![](assets/fota_mqtt_5.png)
> `版本号`是指待升级设备当前的版本号
> `deviceName`是指待升级设备名

升级中的任务如下，通过设备串口观察设备升级过程，设备升级成功后会重启，并上报新的版本号，验证固件显示成功，一次完整的FOTA结束
![](assets/fota_mqtt_6.png)

<a id="wsf"></a>

## WSF 通道 FOTA 自测指南

<a id="linkkit"></a>

## LinkKit 通道 FOTA 自测指南
### 1. 制作升级固件
制作升级固件，获取固件版本号
```
$ aos make clean && aos make linkkitapp@{board}
```
记录版本号为`app-1.0.0-20180320.1310`
![](assets/fota_linkkit_1.png)

### 2. 上传升级固件
登录[一站式开发平台](https://linkdevelop.aliyun.com)，`固件升级` -> `新增固件`
![](assets/fota_linkkit_2.png)
![](assets/fota_linkkit_3.png)
> 填写的`固件版本号`必须与上传的固件真实版本号一致，否则会导致升级失败

### 3. 触发升级
> **场景**：将版本号为`app_1.0.0_20180319.1020`，设备名为`test_light_08`的设备升级到`app_1.0.0_20180320.1310`版本

登录[一站式开发平台](https://linkdevelop.aliyun.com)，选择待升级的固件，点击`验证固件`
![](assets/fota_linkkit_4.png)
![](assets/fota_linkkit_5.png)

升级中的任务如下，通过设备串口观察设备升级过程，设备升级成功后会重启，并上报新的版本号，验证固件显示成功，一次完整的FOTA结束
![](assets/fota_linkkit_6.png)
![](assets/fota_linkkit_7.png)

查看设备详情，设备固件版本发现已经升级到`app_1.0.0_20180320.1310`版本
![](assets/fota_linkkit_8.png)
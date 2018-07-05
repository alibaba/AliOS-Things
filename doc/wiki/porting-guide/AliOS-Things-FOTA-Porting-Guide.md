EN| [中文](AliOS-Things-FOTA-Porting-Guide.zh)

# Content 

- [1 Porting overview](#1移植概要)
- [2 Implement of interface ](#2接口实现)

------

# 1 Porting overview

Before porting, please make sure the current platform have had the following functions. 

```
1.bootloader supports firmware update
2.support flash operation
3.support Alink, or MQTT, or CoAP to connect to related OTA platform in Alibaba
```

We recommend you to store the interface related to specific chips in platform/mcu/xxxx/port/fota_port.c.  XXXX is the name of the platform that is currently being ported.

Some functions to implement `hal_ota_module_t` in fota_port.c : 

```c
      int (*init)(hal_ota_module_t *m, void *something);

      int (*ota_write)(hal_ota_module_t *m, volatile uint32_t *off_set,
                       uint8_t *in_buf , uint32_t in_buf_len);

      int (*ota_read)(hal_ota_module_t *m,  volatile uint32_t *off_set,
                      uint8_t *out_buf , uint32_t out_buf_len);

      int (*ota_set_boot)(hal_ota_module_t *m, void *something);
```

# 2 Implement of interface 

Some interface in implement of  `hal_ota_module_t`，[Implement example](https://github.com/alibaba/AliOS-Things/blob/master/platform/mcu/stm32l4xx/hal/ota_port.c).

1. Implement `int (*init)(hal_ota_module_t *m, void *something)` : This interface is used for initiation of flash hardwares and related variables. 
   Instruction of parameters: 

| Name      | Instruction                              |
| --------- | ---------------------------------------- |
| m         | interface pointer. There's no need to use in functions. |
| something | current breakpoint address, use :  _off_set = *(uint32_t*)something。 |

  When initiating, see whether _off_set is 0.

```
If it is 0, you need to erase the flash partition in FOTA and prepare for a new download. For the sake of safety, we suggest you to execute CRC16 Check, and initiate CRC16 at the same time.

If it is not 0, which means the last download is interrupted,you don't need to erase the flash partition.

If CRC16 check was used,  the backup CRC16 check at last breakpoint should be read from flash.
```

2. Implement  `int (*ota_write)(hal_ota_module_t *m, volatile uint32_t *off_set, uint8_t *in_buf , uint32_t in_buf_len)`: This interface is used to save firmwares when downloading FOTA.

Instruction of parameters: 

| Name       | Instruction                              |
| ---------- | ---------------------------------------- |
| m          | interface pointer. There's no need to use in functions. |
| off_set    | Write offset address. Since the upper level is currently set 0, there's no need for use. |
| in_buf     | The data you need to write this time.    |
| in_buf_len | The length of data you need to write this time.本次写入数据长度。 |

Since off_set is always 0 when it is called, the write offset address should be implemented by functions. We suggest you to use the offset address obtained in function init, and add your write length after each write. This function needs to call flash write function to write data.

3. Implement `int (*ota_read)(hal_ota_module_t *m,  volatile uint32_t *off_set,uint8_t *out_buf , uint32_t out_buf_len)`  :

This interface hasn't been used in FOTA currently, so you don't necessarily need to implement it.
 Instruction of parameters: 

| Name       | Instruction                              |
| ---------- | ---------------------------------------- |
| m          | interface pointer. There's no need to use in functions. |
| off_set    | Write offset address. There's no need for use. |
| out_buf    | read the buffer memory                   |
| in_buf_len | The length of data you read this time.   |

You can encapsulate flash read function with this function.

4. Implement`int (*ota_set_boot)(hal_ota_module_t *m, void *something)` : This interface is used to update the system after download, or save the scene if download is interrupted. 

 Instruction of parameters: 

| Name      | Instruction                              |
| --------- | ---------------------------------------- |
| m         | interface pointer. There's no need to use in functions. |
| something | the pass-in parameter. The actual pass-in type is : ota_finish_param_t *。 |

Definition of ota_finish_param_t is :

```
typedef struct  {
    OTA_ENUM_UPDATE_TYPE update_type;
    OTA_ENUM_RESULT_TYPE result_type ;
} ota_finish_param_t;
```

Get this variable :  `ota_finish_param_t *param = (ota_finish_param_t *)something` . The meaning of two sub variables is :

```
update_type  update type.Enumerated types. OTA_KERNEL, OTA_APP, or OTA_ALL, which respectively represent kernel update, APP update and update them all.
result_type  update result. Enumerated types. OTA_FINISH or OTA_BREAKPOINT, which respectively represent update complete and update break off. 
```

  When `result_type == OTA_BREAKPOINT`, current crc result should be saved in flash, in oder to continue based on it. 

When `result_type == OTA_FINISH`,  download is complete. You can set boot by update_type and then restart. Firmware replacement task can be completed by bootloader.

# 3 Test of functions

## Test of Alink FOTA

Alink FOTA means connecting to Alibaba cloud platform, taking Alink as an interaction channel to query new  firmware version, acquire download link and take http/https as channel to download firmware.

Test method: (take mk3060 as an example) :

1. compile mk3060's Alinkapp:


`aos make  alinkapp@mk3060`

2. Burn the compiled app image into mk3060.
3. Prepare the bin file for OTA. Make sure the version number of the new file is larger than the previous one, (you can modify framework/common/common.mk to change the version number temporarily).


`make clean`
`aos make  alinkapp@mk3060`

Record the new version number in compilation information.
![](https://img.alicdn.com/tfs/TB1mwFbicrI8KJjy0FhXXbfnpXa-865-118.png)

4. Open Alibaba cloud platform : http://smart.aliyun-inc.com/admin/upgrade/manager.htm . Find "升级管理-规则管理" in left task bar, and click "添加升级" at the top right corner of the page.

![](https://img.alicdn.com/tfs/TB1yrJlicjI8KJjSsppXXXbyVXa-865-339.png)

Fill in update information as followed. Fill in ALINKTEST_LIVING_LIGHT_ALINK_TEST in "设备型号", and uuid can be got by inputing uuid at cli when the board is connected to Alink.
![](https://img.alicdn.com/tfs/TB1W9dQinnI8KJjy0FfXXcdoVXa-865-723.png)

5. Use 小智APP for network provision



Give power to the serial port of the board. Input "netmgr clear" to clear the original network information, restart the board, and input "netmgr start" again.

Open ONEAPK app, and click "添加设备-分类查找-模组认证-配网V3_热点配网_小智-查找设备" on the upper right corner of the page. Make sure the router connected with your mobile phone can be scanned and complete network provision by scanning it.

6. Click “固件升级”-“刚配对设备名称”-“立即升级” in “我的” column in ONEAPK page to begin updating. Progress information will be displayed in app and serial port. Pay attention to the progress until it succeeds or fails. If update fails, you can analyze the reason and inform related developers through the print result in serial port.

## Test of MQTT/CoAP FOTA

MQTT FOTA means connecting to Alibaba IoT platform, taking MQTT as an interaction channel to query new  firmware version, acquire download link and take http/https as channel to download firmware.

CoAP FOTA means connecting to Alibaba IoT platform, taking CoAP as an interaction channel to query new  firmware version, acquire download link and take http/https as channel to download firmware.

Test method:

1. Create product  http://iot.console.aliyun.com/?spm=5176.6660585.774526198.1.BoBWs7#/product/newlist/ region/cn-shanghai ：

![](https://img.alicdn.com/tfs/TB1WS8kilTH8KJjy0FiXXcRsXXa-865-284.png)

2. Add device：

![](https://img.alicdn.com/tfs/TB1LBFQinnI8KJjy0FfXXcdoVXa-865-243.png)

3. Check new device information in "设备管理"：

![](https://img.alicdn.com/tfs/TB1yyxLigDD8KJjy0FdXXcjvXXa-865-269.png)

4. Modify the authentication code related to products:

MQTT:

Fill in the obtained deviceName, productKey, and deviceSecret to the corresponding location of aos/example/mqttapp/mqttapp.c:

![](https://img.alicdn.com/tfs/TB18zXPih6I8KJjy0FgXXXXzVXa-865-104.png)

COAP:
Fill in the obtained deviceName, productKey, and deviceSecret to the corresponding location of aos/example/mqttapp/mqttapp.c:. 

You can directly add ".1" in device Name to get "IOTX_DEVICE_ID".
![](https://img.alicdn.com/tfs/TB1aBJQinnI8KJjy0FfXXcdoVXa-865-128.png)

5. Prepare firmware update files

Modify framework/common/common.mk :
![](https://img.alicdn.com/tfs/TB1Yqpaif2H8KJjy0FcXXaDlFXa-865-378.png)
Change the app version number with the beginning of app-2. The purpose is to make the version of server-side higher than terminal side.
Example of compilation, take mk3060 as an example :
MQTT: ` aos make mqttapp@mk3060`  generate mqttapp bin file.

Path: out/mqttapp@mk3060/binary/mqttapp@mk3060.ota.bin

CoAP: `aos make coapapp@mk3060 ` generate coapapp bin file.

Path: out/coapapp@mk3060/binary/coapapp@mk3060.ota.bin
![](https://img.alicdn.com/tfs/TB17mdQinnI8KJjy0FfXXcdoVXa-865-114.png)
You can meanwhile get the version number of app as the above picture.

6. Upload the newly generated bin file to Alibaba cloud IOT platform:

![](https://img.alicdn.com/tfs/TB1jPAMh8fH8KJjy1XbXXbLdXXa-865-386.png)

7. Prepare firmware file for operation :

Modify framework/common/common.mk, and change the version number to the previous one.
Compile the related version after making clean :
MQTT:` aos make mqttapp@mk3060`  generate mqttapp bin file.

Path : out/mqttapp@mk3060/binary/mqttapp@mk3060.ota.bin

CoAP:`aos make coapapp@mk3060` generate coapapp bin file.

Path : out/coapapp@mk3060/binary/coapapp@mk3060.ota.bin

Burn the generated version to the board. Give it power and operate it.

8．test the firmware：
In "固件升级管理" in IoT platform, click "验证固件" of the uploaded firmware, and select the current device for firmware push: 
![](https://img.alicdn.com/tfs/TB17xQVh4TI8KJjSspiXXbM4FXa-865-321.png)

Since MQTT has subscription ability, it will start FOTA process once update is pushed. For CoAP, since it doesn't have subscription ability, it won't immediately start FOTA process unless it finds out server update. We recommend you to restart the device when update is pushed, because an update check will start after restarting. 

During update process, the terminal will restart. When restart is completed, if the firmware version is the latest one, update is successful. Refresh the page, there will be a message page:

![](https://img.alicdn.com/tfs/TB1EF4ailHH8KJjy0FbXXcqlpXa-865-377.png)

### Test of CoAP only FOTA：

For test of CoAP only FOTA ( CoAP as channel for both information interaction and firmware download )，则you can add "FOTA_DL_COAP=1" when compiling：

`aos make clean`

`aos make coapapp@mk3060 FOTA_DL_COAP=1`
Other steps remain the same.


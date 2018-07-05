[EN](AliOS-Things-FOTA-Porting-Guide)| 中文

# 目录

  * [1 移植概要](#1移植概要)
  * [2 接口实现](#2接口实现)
------
# 1移植概要
  FOTA移植前先确认当前平台以下功能已经具备或完成移植：

    1.bootloader支持固件升级。
    2.支持flash操作。
    3.支持Alink，或者MQTT，或者CoAP连接到阿里相应OTA平台。

  FOTA跟具体芯片相关的接口建议放置在platform/mcu/xxxx/port/fota_port.c。其中，xxxx为当前正在移植的平台名。
  fota_port.c需要实现结构体`hal_ota_module_t`中的几个函数：
  ```c
      int (*init)(hal_ota_module_t *m, void *something);

      int (*ota_write)(hal_ota_module_t *m, volatile uint32_t *off_set,
                       uint8_t *in_buf , uint32_t in_buf_len);

      int (*ota_read)(hal_ota_module_t *m,  volatile uint32_t *off_set,
                      uint8_t *out_buf , uint32_t out_buf_len);

      int (*ota_set_boot)(hal_ota_module_t *m, void *something);
  ```
# 2接口实现
  实现`hal_ota_module_t`中的几个接口，[参考实现](https://github.com/alibaba/AliOS-Things/blob/master/platform/mcu/stm32l4xx/hal/ota_port.c)。
    1. 实现`int (*init)(hal_ota_module_t *m, void *something)`，此接口主要进行flash硬件及相关变量的初始化。
      参数说明：

| 名称        | 描述                                       |
| --------- | ---------------------------------------- |
| m         | 接口模块指针，函数中暂无需使用。                         |
| something | 当前被用作断点续传的断点地址，使用： _off_set = *(uint32_t*)something。 |

  初始化时，判断_off_set是否为0。

    若为0，则需擦除fota下载flash分区,准备进行一次全新的下载。为了安全起见，
    建议对flash操作进行crc16校验，故同时初始化crc16。
    若不为0，则认为上次下载中断，接下来将进行断点续传。无需擦除fota下载flash分区。
    如使用了crc16校验，则需从flash指定区域读取上次断点时备份的crc16校验。 
    
    2. 实现 `int (*ota_write)(hal_ota_module_t *m, volatile uint32_t *off_set, uint8_t *in_buf , uint32_t in_buf_len)`，
      此接口用于fota下载时实现固件逐块保存。
        参数说明：

| 名称         | 描述                   |
| ---------- | -------------------- |
| m          | 接口模块指针，函数中暂无需使用。     |
| off_set    | 写偏移地址，当前上层直接置0，无需使用。 |
| in_buf     | 本次需要写入的数据。           |
| in_buf_len | 本次写入数据长度。            |

  由于off_set调用时永远置0，所以写偏移地址需要函数自身实现，建议使用init函数中初始化或得到的偏移地址，每次写入之后将此偏移量加上本次写入长度进行累加。此函数需要调用flash写操作函数对数据进行写入。

    3. 实现`int (*ota_read)(hal_ota_module_t *m,  volatile uint32_t *off_set,uint8_t *out_buf , uint32_t out_buf_len)`;
      当前fota没有用到此接口，可以不实现。
        参数说明：

  | 名称 | 描述 |
  | ------ | ------ |
  |  m          | 接口模块指针，函数中暂无需使用。
  |  off_set    | 读偏移地址，无需使用。
  |  out_buf    | 读出缓存。
  |  in_buf_len | 本次读出的数据长度。

  此函数封装flash读操作函数即可。

  4.实现`int (*ota_set_boot)(hal_ota_module_t *m, void *something)`本接口用于下载完成后对系统进行升级或者下载中断后保存现场。
  参数说明：

| 名称        | 描述                                  |
| --------- | ----------------------------------- |
| m         | 接口模块指针，函数中暂无需使用。                    |
| something | 传入的参数，实际传入类型为：ota_finish_param_t *。 |

ota_finish_param_t定义如下：

    typedef struct  {
        OTA_ENUM_UPDATE_TYPE update_type;
        OTA_ENUM_RESULT_TYPE result_type ;
    } ota_finish_param_t;

  获取此变量：`ota_finish_param_t *param = (ota_finish_param_t *)something`其中两个子变量含义：

    update_type  升级类型。枚举类型，必须为 OTA_KERNEL,OTA_APP,OTA_ALL之一，
                 分别代表升级内核，升级APP及全部升级。
    result_type  升级结果。枚举类型，必须为 OTA_FINISH或OTA_BREAKPOINT。
                 分别代表升级完成和升级中断。

  当`result_type == OTA_BREAKPOINT`时，应保存当前crc校验值到flash，以备断电后可以断点续传。
  当`result_type == OTA_FINISH`，说明下载完成，接下来根据`update_type`设置boot参数，然后重启，将固件替换任务交由bootloader完成。

# 3功能验证
## Alink fota验证
Alink fota是指连接阿里智能云平台以Alink作为交互通道，进行固件新版本查询，下载链接获取及进度上报；以http/https做为下载通道进行固件下载。
测试方法，以mk3060为例：

1.编译mk3060的Alinkapp：
`aos make  alinkapp@mk3060`

2.将编译成功的app镜像烧入mk3060开发板中。

3.准备用于ota的bin文件,保证新文件版本号大于之前烧入版本号,（可以修改framework/common/common.mk，将app版本号临时改大）：

`make clean`
`aos make  alinkapp@mk3060`

从编译信息中记录新版本号。
![](https://img.alicdn.com/tfs/TB1mwFbicrI8KJjy0FhXXbfnpXa-865-118.png)

4.打开智能云平台：http://smart.aliyun-inc.com/admin/upgrade/manager.htm 。左侧菜单栏找到升级管理-规则管理，然后在页面右上角点击添加升级。
![](https://img.alicdn.com/tfs/TB1yrJlicjI8KJjSsppXXXbyVXa-865-339.png)

按照以下方式填写升级信息，设备型号填写ALINKTEST_LIVING_LIGHT_ALINK_TEST，uuid需要在板子启动接入alink后在cli输入uuid获得。
![](https://img.alicdn.com/tfs/TB1W9dQinnI8KJjy0FfXXcdoVXa-865-723.png)

5.使用小智APP配网。
板子连接串口上电。输入netmgr clear清除原有配网信息，重启板子，再次输入 netmgr start。
打开ONEAPK 手机版app,点击右上角添加设备-分类查找-模组认证-配网V3_热点配网_小智-查找设备，保证手机连接的路由器设备可以扫描到，通过扫描设备完成配网。

6.在ONEAPK的“我的”页面栏找到“固件升级”-“刚配对设备名称”-“立即升级”，开始升级，app上会有进度显示，同时串口端也会有相应升级信息，观察，直至升级成功或失败，如果升级失败，可通过串口打印信息分析原因并可反映给相关开发人员。

## MQTT/CoAP fota验证
MQTT fota 是指连接阿里IoT平台，以MQTT作为交互通道进行固件新版本查询，下载链接获取及进度上报；以http/https作为下载通道进行固件下载；

CoAP fota 是指连接阿里IoT平台，以CoAP作为交互通道进行固件新版本查询，下载链接获取及进度上报；以http/https或者CoAP作为下载通道进行固件下载；
测试方法:

1.创建产品 http://iot.console.aliyun.com/?spm=5176.6660585.774526198.1.BoBWs7#/product/newlist/ region/cn-shanghai ：
![](https://img.alicdn.com/tfs/TB1WS8kilTH8KJjy0FiXXcRsXXa-865-284.png)

2.添加设备：
![](https://img.alicdn.com/tfs/TB1LBFQinnI8KJjy0FfXXcdoVXa-865-243.png)

3.在设备管理里面查看新添加设备的信息：
![](https://img.alicdn.com/tfs/TB1yyxLigDD8KJjy0FdXXcjvXXa-865-269.png)

4.修改产品相关认证代码：
MQTT：
将获取到的 deviceName, productKey及 deviceSecret填写到mqtt例程 aos/example/mqttapp/mqttapp.c相应位置:  
![](https://img.alicdn.com/tfs/TB18zXPih6I8KJjy0FgXXXXzVXa-865-104.png)

COAP：
将获取到的 deviceName, productKey及 deviceSecret填写到mqtt例程 aos/example/mqttapp/mqttapp.c相应位置，其中IOTX_DEVICE_ID可直接用在deviceName后加上.1即可：
![](https://img.alicdn.com/tfs/TB1aBJQinnI8KJjy0FfXXcdoVXa-865-128.png)

5.准备更新固件文件
修改framework/common/common.mk:
![](https://img.alicdn.com/tfs/TB1Yqpaif2H8KJjy0FcXXaDlFXa-865-378.png)
先将app版本号改为app-2开头，目的：让服务器端版本高于终端运行版本。
编译 相关的例程,以 mk3060 为例:
MQTT:` aos make mqttapp@mk3060`生成mqttapp bin 文件,路径：out/mqttapp@mk3060/binary/mqttapp@mk3060.ota.bin

CoAP: `aos make coapapp@mk3060`生成coapapp bin 文件,路径：out/coapapp@mk3060/binary/coapapp@mk3060.ota.bin
![](https://img.alicdn.com/tfs/TB17mdQinnI8KJjy0FfXXcdoVXa-865-114.png)
同时得到app版本号，见上图。

6.在阿里云IOT平台上传刚生成的bin文件：
![](https://img.alicdn.com/tfs/TB1jPAMh8fH8KJjy1XbXXbLdXXa-865-386.png)

7.准备运行用固件文件：
修改framework/common/common.mk，将版本号改回最初版本号。
make clean后在次编译相应版本：
MQTT:` aos make mqttapp@mk3060`生成mqttapp bin 文件,路径：out/mqttapp@mk3060/binary/mqttapp@mk3060.ota.bin

CoAP:`aos make coapapp@mk3060`生成coapapp bin 文件,路径：out/coapapp@mk3060/binary/coapapp@mk3060.ota.bin

将生成的版本刷入相应开发版，上电运行。

8．验证固件：
在iot平台固件升级管理点击刚上传的固件的验证固件按钮，选择当前设备进行固件推送：
![](https://img.alicdn.com/tfs/TB17xQVh4TI8KJjSspiXXbM4FXa-865-321.png)

mqtt 由于有订阅能力会在推送触发后立马进入fota升级流程，coap由于没有订阅功能，推送更新后，不会立即进入更新状态，需要定时查询到服务器有更新时才会进入更新流程，建议推送后重启一下测试终端，重启后会立即查询更新。

升级过程中，终端会进行一次重启动作，重启完成后，如果固件版本为服务器端设置的更新版本，则升级成功，刷新IOT平台固件验证页面，会有升级成功提示：
![](https://img.alicdn.com/tfs/TB1EF4ailHH8KJjy0FbXXcqlpXa-865-377.png)


### 纯CoAP FOTA测试：
如果需要测试纯CoAP FOTA(信息交互与固件下载都走CoAP通道)，则在编译版时加上FOTA_DL_COAP=1：

`aos make clean`

`aos make coapapp@mk3060 FOTA_DL_COAP=1`
其他步骤不变。


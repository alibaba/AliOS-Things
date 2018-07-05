EN | [中文](stm32-networking.zh)

# Step by Step: MQTT to Cloud with STM32

This article will show you how to use mqtt on STM32L475 to connect to the cloud, based on the AliOS Things 1.2.0.

## 1、Hardware environment setup

1、Development board STM32 B-L475E-IOT01A 

- The development board we use is STM32L475, with 1M bytes of Flash and 128K bytes of SRAM.
- Support WiFi, Bluetooth, SubG, NFC and many other wireless connections. Besides, it supports Arduino interface, so users can also use other wireless connection modules, such as the GSM.

- 64 Mbits's Quad-SPI Flash is also integrated on the board.
- There are many kinds of sensors on board, such as temperature and humidity sensors, 3 axis magnetometer, acceleration sensors, gyroscopes, proximity sensors and pressure sensors. It is convenient for developers to apply a variety of applications.

- There are two digital full-angle microphones on the board. If needed, it can achieve voice control.


Detailed information about hardwares can refer to [Introduction of STM32](http://www.st.com/content/st_com/en/products/evaluation-tools/product-evaluation-tools/mcu-eval-tools/stm32-mcu-eval-tools/stm32-mcu-discovery-kits/b-l475e-iot01a.html)

![](https://img.alicdn.com/tfs/TB1J6KLmZLJ8KJjy0FnXXcFDpXa-3968-2976.jpg)

2、MK3060 communication module

![](https://img.alicdn.com/tfs/TB1HYMxmRfH8KJjy1XbXXbLdXXa-2976-3968.jpg)

3、Three double-head Dupont Lines

4、Ways of connection:

 Connect STM32L475 D0 with MK3060 UART TX, STM32L475 D1 with MK3060 UART RX, and STM32L475 GND with MK3060 GND. Detailed way of connection is shown as followed picture: connect the point with same color in two boards with Dupont Line.

![](https://img.alicdn.com/tfs/TB1k.Sfm3DD8KJjy0FdXXcjvXXa-3968-2976.jpg)



## 2、Connect to Alibaba Cloud IoT suite

How to connect to Alibaba Cloud through mqtt app can refer to [Connect preparations for IoT devices ](https://help.aliyun.com/document_detail/42714.html?spm=5176.doc30530.2.5.eofHfK) and [Connect the devices](https://help.aliyun.com/document_detail/30530.html?spm=5176.doc42714.2.7.LjopzI)

### 2.1、Five steps on the cloud:

1. Register a Alibaba cloud account
2. Start a IoT Suite
3. Create a product, get`ProductKey`
4. Create a device, get `DeviceName` and `DeviceSecret`
5. 
   Define Topic`$(PRODECT_KEY)/$(DEVICE_NAME)/data`,  and set the permission as: Devices are capable of publishing and subscribing

### 2.2、Parameter modification in device side

Source code of mqtt app is `AliOS-Things/example/mqttapp/mqtt-example.c`, credentials for connecting to mqtt server is located at [framework/protocol/linkkit/iotkit/sdk-encap/imports/iot_import_product.h](https://github.com/alibaba/AliOS-Things/blob/master/framework/protocol/linkkit/iotkit/sdk-encap/imports/iot_import_product.h). The three parameters obtained from the cloud ( `ProductKey`，`ProductSecret`,`DeviceName`和`DeviceSecret`) are corresponding to the three macros `PRODUCT_KEY`，`PRODUCT_SECRET`,`DEVICE_NAME`和`DEVICE_SECRET` in the code.

```
#elif  MQTT_TEST
#define PRODUCT_KEY             "b1eszMRbDvz"                                                         
#define DEVICE_NAME             "mqtt_test"
#define DEVICE_SECRET           "CAaQz8Fc1JkFEyuzFhu4NpHSTlRSmRxV"
#define PRODUCT_SECRET          "Fxx6nyYptOugnS6H"
#else
```

## 3、AliOS Things Mqtt app compilation

The current AliOS Things mqtt app compilation command line is:

aos make mqttapp@b_l475e

After the command is executed, the generated bin file and the hex file can be found in `out/mqttapp@b_l475e/binary/`  directory.

## 4、Firmware burning

### 4.1、Burning of MK3060 

Please go to MK3060 [AT firmware](http://developer.mxchip.com/developer/md/bWljby1oYW5kYm9vay9Eb3dubG9hZC8zLTIuTWlDT19BVF92Mi4wX0NNRC5tZA) download website officially provided by Mxchip, and select the firmware corresponding to MOC108/EMW3060 model. Way of burning can refer to "1. Update application firmware" in [MK3060 firmware burning guidance](http://developer.mxchip.com/handbooks/63).

### 4.2、Burning of STM firmware

Use [ST-LINK](http://www.st.com/content/st_com/en/products/development-tools/software-development-tools/stm32-software-development-tools/stm32-programmers/stsw-link004.html) to burn the firmware. Steps of burning is shown as followed picture:

![](https://img.alicdn.com/tfs/TB1QprXm46I8KJjy0FgXXXXzVXa-937-241.jpg)

## 5、Configuration of network and data connection

After the above 4 steps, the corresponding mqttapp binary has been burned into stm32L475 development board, and the port print is started as followed:

Wifi module can be connected to corresponding AP through command line.

`netmgr connect *ssid* *password* *open|wep|wpa|wpa2*`

When devices can connect to the network, mqttapp will start running. The following picture is mqtt run log:

![](https://img.alicdn.com/tfs/TB1_9CBm8DH8KJjSspnXXbNAVXa-1708-284.jpg)

Log information that can be further obtained on the cloud is shown as below:

![](https://img.alicdn.com/tfs/TB1kk_ImZLJ8KJjy0FnXXcFDpXa-1875-833.jpg)


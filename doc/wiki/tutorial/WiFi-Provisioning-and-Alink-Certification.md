EN| [中文](WiFi-Provisioning-and-Alink-Certification.zh) 

When WiFi and protocol stack porting have been completed, you can start network provisioning test and certification.

Alinkapp (located in`example/alinkapp/`) can be used for Alink test and certification. (Note: Alinkapp needs the support of WiFi HAL and socket. Before using this app, please make sure WiFi HAL and socket porting have already been completed.) 

You can follow the following steps to use alinkapp and see how it can achieve smart network provision and authentication:

## Setting before network provision 
Before starting network provision test, you need to install “阿里智能” APP on your mobile phone, and set it.

- Download DemoAPP, address: https://open.aliplus.com/download/.

  Click “接入阿里智能-厂测包”, and then download and install “开放平台厂测包”.


- Open the app and select "开启配网模组测试列表" in "切换环境“ to open the entrance for module authentication and provision. Close the app and log in again.

![配网条件预置截图](https://img.alicdn.com/tfs/TB1zAlEgS_I8KJjy0FoXXaFnVXa-634-1004.png)

## Smart network provision
With smart network provision, you can carry out network provision, activation and binding, and connect your device to Alibaba cloud.

**Note**: If network provision has already been done before, please clean the environment first. Steps to clean up the environment: 1. run "netmgr clear" to clear AP information; 2. restart the device.

- Open provision application in your device (refer to example/alinkapp).

- Give power to your device. Enter provision mode by command "netmgr start" or pressing key.

- Start provision app in your mobile phone. Select “添加设备”->“分类查找”->“模组认证”->”V3配网”, input available ssid and password, and click ”搜索设备”.

- When your device has successfully connected to network, the app will enter device activation interface.

- Start activation instruction by command "active_alink" or pressing key.

- When activation is completed, the device will be bound to your mobile phone, which means the success of smart network provision.



![一键配网步骤截图](https://img.alicdn.com/tfs/TB1QFiggILJ8KJjy0FnXXcFDpXa-1968-942.png)

## Hot spot provision
Through hot spot provision test, the validity of WiFi porting and provision logic can be further verified. Please follow the following steps:

**Note**: If network provision has already been done before, please clean the environment first. Steps to clean up the environment: 1. run "netmgr clear" to clear AP information; 2. restart the device.

- Set your device to provision mode (the same as smart provision).
- Open mobile data in your mobile phone.
- Open the provision app, input ssid and a wrong password on "搜索网络" interface. After about 120 seconds, it will enter “配网失败” interface.
- Select “热点模式”.
- Under hot spot mode, select your device, and send ssid and password to it.
- The same as smart provision, when your device has successfully connected to network, you can activate it by inputting command or pressing key.

![热点配网步骤截图](https://img.alicdn.com/tfs/TB1ApiggILJ8KJjy0FnXXcFDpXa-1442-1098.png)

## 2pps and 5pps test
If smart provision and hot spot provision test are completed, WiFi porting is basically verified. Through performance tests such as 2pps and 5pps, you can further validate it. The following are the steps of 2pps and 5pps tests:

- Make sure your device has completed network provision and binding.
- Select “认证” in the home page of provision app to enter certification interface.
- Select “新建认证测试” and select the device to be tested.
- Configure mailbox and accompany device according to screen tips (you can just select "下一步").
- In "测试选择" interface, select “性能测试”.
- In "用例列表" interface, select “低频指令（2pps）压测设备响应成功率[Release]” to start 2pps test. As for 5pps test, select “高频指令（5pps）压测设备响应成功率[Release]”
- The test will last for about 12 hours.

![2pps和5pps测试步骤截图](https://img.alicdn.com/tfs/TB1fnpLgMvD8KJjy0FlXXagBFXa-2004-770.png)
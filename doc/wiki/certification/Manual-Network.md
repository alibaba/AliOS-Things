EN|[中文](Manual-Network.zh)

# AliOS Things Network Self Test Guide

## Download the app used for test

Refer to [APP Introduction](https://living.aliyun.com/doc#muti-app.html)

## Settings before test

### Set permission

<span data-type="color" style="color:#F5222D;">Allow all the permission of the APP, or it may lead to failure of network distribution.</span>
Settings->Security->Permission->DemoApp->allow all the permission (The steps are different in different mobile phone)  
As follows:![](assets/awss_1.png)

### Sign up a test account 

1. Sign up a test account after installation（you can just use your phone number）.

As follows：![](assets/awss_2.png)

### Log in 

<span data-type="color" style="color:#F5222D;">One account can only be used in one phone at the same time.</span>
If you log in successfully, the interface will be as follows:![](assets/awss_3.png)

## Network Configuration

You can add devices to your personal account through following ways.

1. SmartConfig
2. Hot spot connection
3. Zero configuration
4. WiFi connection
5. Scan to add

### SmartConfig

**Prerequisite**：Your mobile phone should connect to the router your device is to be connected to 

##### IOS SmartConfig

<span data-type="color" style="color:#F5222D;">IOS will try SmartConfig by default. If it failed, you could try again or start hot spot connection.</span>

1.Click the button to add device：![](assets/awss_4.png)

2.Select device type （<span data-type="color" style="color:#F5222D;">Please choose correct type, or it may lead to error.</span>），click to add. 
![](assets/awss_5.png)

3.Enter wifi password.<span data-type="color" style="color:#F5222D;">(Make sure it is correct, or it may lead to error.）</span>  Enter the next step.
![](assets/awss_6.png)

4.Enter network mode. You can make sure it in demoapp. 
![](assets/awss_7.png)

5.Start network connection.
![](assets/awss_8.png)

6.You can bind your device when you connect to network successfully. 

![](assets/awss_9.png)

##### Android SmartConfig

<span data-type="color" style="color:#F5222D;">Android will try hot spot connection by default. So you could start smart connection only when you failed in hot spot connection. </span>
Step 1: Close mobile data to make hot spot connection fail. 
Step 2:  
 1.Select “Try another way” when hot spot connection failed.

![](assets/awss_10.png)

 2.Enter connection mode again, and you should confirm it in your mobile phone.
![](assets/awss_11.png)

 3.Start connecting.  
![](assets/awss_12.png)

 4.Bind your device when you connect to network successfully. 

![](assets/awss_13.png)

#### Hot spot connection

**Prerequisites**:

1.Insert SIM card to your mobile phone and open mobile data.   

 2.Connect your phone to the 手机连接需要配网的wifi网络 

##### IOS Hot spot connection:

<span data-type="color" style="color:#F5222D;">IOS will try SmartConfig by default. So you could start hot spot connection only when you failed in smart connection. </span>
Setp 1. Not start connection mode until hot spot connection start.
Step 2. Close the WLAN first and open it again when smart connection failed.
 1.Select “Try another way” when smart connection failed.
![](assets/awss_14.png)

 2.Enter connection mode again, and you should confirm it in your mobile phone.
![](assets/awss_15.png)

 3.Open aha hot spot according to instruction.
![](assets/awss_16.png)

 4.Connect to aha and return to demoapp to start next step.
![](assets/awss_17.png)

 5.Connect your device to the hot spot, and wait to get device information in your phone.
![](assets/awss_18.png)

 6.Start connection when your phone get device information.
![](assets/awss_19.png)

 7.Connect your phone to WiFi again when your device connect to network successfully.
![](assets/awss_20.png)

8.Get connection success information in your phone.

![](assets/awss_21.png)

##### Android Hot spot connection:

<span data-type="color" style="color:#F5222D;">Android's default connection mode is hot spot connection, so it will automatically open aha（Users should permit and open mobile data.）</span>

1.Enter correct device type and wifi password. Enter connection mode. Android phone will automatically open aha and switch to wifi:

![](assets/awss_22.png)

2.Your phone will automatically connect to the app

![](assets/awss_23.png)
![](assets/awss_24.png)

3.Bind your device after it connect to network successfully. 

![](assets/awss_23.png)

#### Zero configuration

The device to be connected gets SSID and password from connected device, and connect to WiFi. 
**Prerequisite**:
 You should bind a device supporting zero configuration under your account.

##### Steps:

<span data-type="color" style="color:#F5222D;">Operation of zero configuration in android and ios are the same.</span>
 1.Add a device that supports zero configuration through SmartConfig or hot spot connection.
 2.Open the device to be connected.
 3.Add the device.
![](assets/awss_16.png)

 4.Start network connection.
![](assets/awss_27.png)

 5.Bind the device after successful connection.

![](assets/awss_28.png)

#### Router connection 

**Prerequisites**:

1. The device should support router connection 


2. You need a router that has connection function（H3C）
3. Your account should have bound H3C router and your phone connect to its WiFi.

##### Steps:

 1.Connect your phone to the wifi from H3C, and click "Add/Bind"
![](assets/awss_29.png)

 2.Open the device, and wait the device to be found in device list.

![](assets/awss_30.png)

 3.Choose the device found in the App, and click to add it.
 4.Enter network mode and start network configuration.
![](assets/awss_31.png)

5. Bind the device after successful connection.

   ​                                          ![](assets/awss_32.png)

#### Bind within a LAN

You can bind the devices within same LAN directly. 

#### Common reasons of connection failure:

 1.False device type. 
 2.False WiFi password. 
 3.Your device is not in connection mode. 
 4.No permission in your phone to open hot spot and WiFi.
 5.Mobile data in your phone hasn't been opened.
 6.Others are connecting, which may lead to disturbance.
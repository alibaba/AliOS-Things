目录

* [1. 简介](#1-简介)
* [2. 基于 AliOS Things 开发](#2-使用alios-things-sdk开发)
   * [2.1 安装 IDE](#21-安装ide)
   * [2.2 获取 SDK](#22-获取sdk) 
   * [2.3 导入工程](#23-导入工程)
   * [2.4 SDK编译](#24-sdk编译)
   * [2.5 固件下载](#25-固件下载) 
* [3. 使用 TI SDK 开发](#3-使用ti-sdk开发)
   * 略
* [4. IoT示例使用](#4-iot示例使用)
     * [4.1 准备工作](#41-准备工作)
     * [4.2 启动配网](#42-启动配网)
     * [4.3 控制设备](#43-控制设备)


## 1. 简介
本 sdk 提供一个基于 MSP-EXP432P401R 开发板，通过与 Wi-Fi 模块进行 AT 指令串口通信，实现与阿里云 SDS 直连的 IoT 物联网典型开发应用示例源代码。

下面来讲解如何在您的 PC 上安装 CCS 开发环境，并在 CCS 中进行编译，下载本 sdk 工程。 



## 2. 基于 AliOS Things 开发

### 2.1 安装IDE
点击 [CCS 下载](http://processors.wiki.ti.com/index.php/XMS432_Support#Software_Compatibility)，进入开发工具下载列表页面。

在 “IDE Compatibility” 列表中，选择 “MSP432401R REVC” 选项中，适用您 PC 系统类型的 CCS 版本。如： Windows 系统 PC 开发环境，请下载： CCS Windows 对应的 CCS 6.1.1 .zip 文件。


![下载ccs](https://github.com/neooxu/IOT-MSP432/blob/master/image/download_ccs.png)


**注意：**下载需先登录。如无账号，请先注册，登录后，才能下载。

下载完成后，解压缩，双击 “ccs_setup_6.1.1.00022.exe” 启动安装。 具体步骤：

1. 进入 “Processor Support” 页面, 请务必勾选 “MSP Ultra Low Power MCUS”，然后其它均默认，点击 “next” 即可。
2. 进入 “CCS Installation” 页面，启动安装，安装过程需耗费几分钟时间，请耐心等待。（请务必保证您的电脑保持联网状态。） 
3. 安装进程结束后，建议选择生成桌面快捷方式，以便开发时快速进入。



### 2.2 获取SDK
请点击进入 [AliOS Things 仓库](https://github.com/alibaba/AliOS-Things/tree/dev_msp432)，默认分支：dev_msp432 ，您可以：

* 如已安装 Git 软件，使用 git 命令克隆到 PC 本地：`git clone https://github.com/alibaba/AliOS-Things.git`
* 如未安装 Git 软件，可确认当前在 `dev_msp432` 分之后，直接下载压缩包文件 PC 本地，点击仓库文件列表右上方， “Download ZIP",下载到本地后解压缩即可。

> 如遇网络问题，请使用 AliOS Things 国内镜像  
> https://gitee.com/alios-things/AliOS-Things.git



### 2.3 导入工程

打开CCS IDE,选择 “Project”-->"Import CCS Project"-->"Browse",找到相应的alios-things 目录位置，如： `F:\Git\AliOS-Things\platform\mcu\msp432\IOT-MSP432\examples\MSP432P4xx\mxchip\iot_sdk`。请记得一定不要勾选 “Copy projects into workspace”。

![选择路径](https://github.com/neooxu/IOT-MSP432/blob/master/image/choose_project.png)

导入工程后，IDE 显示如下图：

![导入alios things](https://github.com/neooxu/IOT-MSP432/blob/master/image/import_alios.png)


### 2.4 编译SDK
点击 CCS 的编译图标， 如下图：

![编译图标](https://github.com/neooxu/IOT-MSP432/blob/master/image/compiler.png)


编译成功后，显示：

![编译结果](https://github.com/neooxu/IOT-MSP432/blob/master/image/alios_compile.png) 


### 2.5 固件下载

请通过 USB 线连接开发板至PD端USB口，确保设备正常供电。如下图：

![设备供电](https://github.com/neooxu/IOT-MSP432/blob/master/image/device_power_on.png) 

点击下载 图标，如下图：

![下载图标](https://github.com/neooxu/IOT-MSP432/blob/master/image/download.png)


下载过程，如下图：

![下载过程](https://github.com/neooxu/IOT-MSP432/blob/master/image/alios_download.png)


下载成功后，自动跳入调试模式。

![CCS下载](https://github.com/neooxu/IOT-MSP432/blob/master/image/alios_debug.png) 

至此，基于AliOS Things 的 IOT 示例源代码已编译下载到 TI 开发板中。



## 3. 使用TI SDK开发

不属于 AliOS Things 技术范畴，略。  
详见 https://github.com/neooxu/IOT-MSP432#3-使用ti-sdk开发

## 4. IoT示例使用

本示例工程提供一个典型的 IoT 应用示例。 系统结构组成如下图：

![IOT典型应用系统组成](https://github.com/neooxu/IOT-MSP432/blob/master/image/demo_frame.png)

TI 的 MSP432P401R 单片机， 通过串口连接 EMW3080 Wi-Fi 模块，并与之进行 AT 指令通信，实现与阿里云 SDS 的通信，同时手机 APP 端可监控开发板的外部设备。

使用具体步骤如下：


### 4.1 准备工作

### 4.1.1 手机 APP 端

1.下载安装APP: 点击 [示例 APP 下载页面](https://www.pgyer.com/TIAnd)，或扫描下方二维码，请根据手机系统类型选择下载。

![app下载](https://github.com/neooxu/IOT-MSP432/blob/master/image/app_address.png)  

2.使用手机号码，进行 APP 用户账号注册，验证，登录。

![app注册登录](https://github.com/neooxu/IOT-MSP432/blob/master/image/app_userid.png)

3.请确保手机已经成功连接至现场路由器。




### 4.1.2 开发板设备端

1. 请使用 microUSB 连接线为开发板供电，并确认红色电源灯常亮，保证供电正常。

![设备供电](https://github.com/neooxu/IOT-MSP432/blob/master/image/device_power_on.png) 


### 4.2 启动配网
打开手机 APP, 点击右上角 “+” 号，选择设备 “TI开发板”，根据提示进行操作。

1. 长按开发板上盖板的 USER 按键，恢复设备出厂设置，此时开发板 OLED 屏最后一行将打印显示："Restore default",继而跳变至 “Wi-Fi config....” 等待手机配网中；
2. APP 配网输入界面中，设置正确的路由器用户名和密码，启动配网，此后开发板 OLED 屏最后一行将显示 设备的联网状态，
    1.  “Wi-Fi connected” 代表成功获取路由器的 SSID 和 密码
    2.  “Cloud connected” 表示与云服务器连接成功，此时需进行身份认证；
3. 当 APP 提示身份验证时，请短按上盖板的 USER 按键，完成身份认证；
4. 以上步骤完成后，APP 会自动跳转设备列表页面，显示在线设备；
5. 点击设备图标，进入设备控制页面。

![恢复出厂](https://github.com/neooxu/IOT-MSP432/blob/master/image/reset_device.png)——> ![启动配网](https://github.com/neooxu/IOT-MSP432/blob/master/image/net_config.png)

![身份验证](https://github.com/neooxu/IOT-MSP432/blob/master/image/id_ensure.png)  ——> ![设备列表](https://github.com/neooxu/IOT-MSP432/blob/master/image/device_list.png)



### 4.3 控制设备
进入设备控制页面，可对各外部设备参数进行监测或控制，或在线调试。

1. 监测：
   1. 温湿度值：设备向云端上报数据
   2. 开关状态：可通过拨动开发板上的拨码开关体验。 开关1：S1， 开关2： S2。 高电平为关，低电平为开。
2. 控制：
   1. RGB灯颜色，饱和度，亮度（通过滑动色彩环 或 滑动条来调整）。
3. 在线调试：
   1. APP 调试界面向设备发送字符串，会在 OLED 屏最后一行打印出来（注意屏幕打印不支持中文）。
   2. APP 发送数据会同步显示在接收区，带来 echo 回声体验效果。


![设备监控](https://github.com/neooxu/IOT-MSP432/blob/master/image/device_control.png) ——>  ![在线调试](https://github.com/neooxu/IOT-MSP432/blob/master/image/app_debug.png)


## 结束语

以上为具体开发流程，请仔细参考，祝学习愉快，谢谢。
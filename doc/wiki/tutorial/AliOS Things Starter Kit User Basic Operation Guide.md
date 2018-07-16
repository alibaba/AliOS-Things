![](https://i.imgur.com/Q8Jcei1.png)

# AliOS Things Starter Kit User Basic Operation Guide
---

## 感谢选择AliOS Things Starter Kit  
本手册主要用于检测开发板器件功能是否完好，以及相关功能简介。

## 一、了解您的开发板  
![](https://i.imgur.com/vhB78y0.png)

USB：可以用来外部供电，程序下载  
LCD：显示sensor、audio、版本等相关信息  
功能键WiFi：LCD屏幕切换  
功能键A：Audio录音功能  
功能键B：Audio播放录音功能  
Reset：程序复位按键  

## 二、开机
USB口插入USB可以直接开机，开机后Power灯为绿色常亮，Programming灯为黄色常亮
LCD屏幕显示开机画面，开机完成后屏幕显示如下图所示：

![](https://i.imgur.com/lwpf3IW.png)


## 三、LCD显示及按键操作
1、基本信息页  
&nbsp;显示有硬件版本、软件版本，SD卡、Sensor数据上传、WiFi SSID。  
![](https://i.imgur.com/9y7jAPw.png)

&nbsp;硬件版本：A10_1_11 
&nbsp;软件版本：设备出厂的软件版本。图片版本仅供参考，实际版本可能不一致。  
&nbsp;Slogan：厂商  
&nbsp;WiFi SSID：WiFi连接状态，未连接时显示not connected；
连接后显示具体的WiFi名称。

2、Sensor检测页面   
&nbsp;按WiFi键，可以将页面切换到Sensor数据页面，页面显示如下图所示：
  
![](https://i.imgur.com/sSA1VGG.png)


&nbsp;Step：计步器
&nbsp;Acc：加速度传感器，旋转或者移动开发板数据会有变化  
&nbsp;Als：光感传感器数据，遮挡传感器数值根据遮挡来变化   
&nbsp;Proximity：距离传感器，遮挡传感器数值根据遮挡来变化  

备注：此时Accelerometer和Light & Proximity灯均会点亮。

![](https://i.imgur.com/ruh3anZ.png)

3、Als曲线图  
继续按WiFi键，可以将页面切换到Als曲线图，页面显示如下图所示：

![](https://i.imgur.com/9lQUvdw.png)  

页面曲线随着实际数据有相应的变化（可以挪动开发板检测一下）。

备注：此页面只有Accelerometer灯点亮，Light & Proximity灯为熄灭状态。

![](https://i.imgur.com/DflGDIA.png)


4、Audio检测页面
按WiFi键，可以将页面切换到Audio检测页面，可以按照文字指示操作录音和播放。

![](https://i.imgur.com/hz4TEL7.png)  

插入耳机，按A键进入录音页面，可以持续录音5S左右。录音结束后可以按B键进入播放页面，播放的内容与录制的一致。

![](https://i.imgur.com/F4l0LE8.png) ![](https://i.imgur.com/gEloBNs.png)

                                                                               
## 四、WIFI一键配网  
可以使用AT命令（AT+WJAP=SSID,KEY）一键配网，具体操作如下:  
1、查看该设备的COM口（以WIN7为例）  
  计算机右键选择设备管理器，查看端口，如下图所示端口为COM49

![](https://i.imgur.com/3GirbuQ.png)


2、安装并打开CRT工具，创建Serial登录  
新建会话，协议选择Serial，输入任意名称。  
点击串行，端口选择设备管理器中看到的COM49，波特率为115200，然后保存设置。

![](https://i.imgur.com/r6R1ORf.png)
![](https://i.imgur.com/A53kVG5.png)

3、会话中输入AT指令一键配网  
输入AT返回OK，证明AT指令流程OK;然后输入AT+WJAP=SSID,KEY一键配网。

![](https://i.imgur.com/0CE6XiF.png)





备注：1、SSID和KEY指的是实际需要连接WiFi的SSID和密码。如Notion_WiFi,1234567890。
2、输入AT命令后等待3S左右可以配网成功，成功后LCD上WIFI SSID显示为实际连上
的SSID名称。
3、不支持中文SSID且SSID名称不能大于24个字符，否则不会显示具体的SSID名称。

下图为WiFi配网成功后的LCD屏幕显示；

![](https://i.imgur.com/C6IRlOK.png)    


## 五、Reset

当开发板出现异常或者需要手动复位时，可以按Reset按键复位程序
![](https://i.imgur.com/b5JK2Mt.png)


备注：此项操作后，您个人设置的一些参数会被清零。

#NOTE:
购买开发板AliOS Things Starter Kit和Developer kit，请扫描天猫旗舰店二维码：

![](https://i.imgur.com/VxHonLB.png)

获取在线技术支持，请扫描技术支持群：

![](https://i.imgur.com/uyT4v0Y.png)
![](https://i.imgur.com/rqjjjA0.jpg)
![](https://i.imgur.com/Q8Jcei1.png)

# AliOS Things Developer Kit User Basic Operation Guide
---

## 感谢选择AliOS Things Developer Kit  
本手册主要用于检测开发板器件功能是否完好，以及相关功能简介。

## 一、了解您的开发板  
![](https://i.imgur.com/woYva7j.png)

USB1：可以用来外部供电，程序下载  
USB2：硬盘存储功能  
LCD：显示Sensor、Audio、Camera、版本等相关信息  
功能键A：升级音频模块  
功能键M：Home键功能，返回主页面  
功能键B：LCD屏幕切换  
SIM Card：SIM卡卡槽，可插入SIM卡  
SD card：SD卡卡槽，卡插入SD卡  
Reset：程序复位按键

## 二、开机
USB1口插入USB可以直接开机，开机后Power灯为绿色常亮，Programming灯为黄色常亮
LCD屏幕显示开机画面，开机完成后屏幕显示如下图所示：  
![](https://i.imgur.com/Arpyh1o.png)


## 三、LCD显示及按键操作
1、基本信息页  
&nbsp;显示有硬件版本、软件版本，SD卡、Sensor数据上传、WiFi SSID。  
![](https://i.imgur.com/eFkH8sE.png)

&nbsp;硬件版本：A20_1_12  
&nbsp;软件版本：图片版本仅供参考，实际版本可能不一致  
&nbsp;SD卡：插入SD卡，显示的状态为READY,拔掉SD卡后显示的状态为NOT EXIST  
&nbsp;Sensor data upload：数据上传云端  
&nbsp;WiFi SSID:显示搜索到的SSID名称，按A键可以刷新SSID  

2、Sensor检测页面1  
&nbsp;按B键，可以将页面切换到Sensor数据页面1，页面显示如下图所示：  
![](https://i.imgur.com/0nYxQ8u.png)


&nbsp;acc：加速度传感器  
&nbsp;mag: 地磁传感器  
&nbsp;gyro：陀螺仪  
&nbsp;旋转或者移动开发板，这些Sensor的数据会有变化。

3、Sensor检测页面2
继续按B键，可以将页面切换到Sensor数据页面2，页面显示如下图所示：  
![](https://i.imgur.com/59p54rW.png)

&nbsp;Barometer：气压数据  
&nbsp;Temperature：温度数据  
&nbsp;Humidity：湿度数据  
&nbsp;Als：光感传感器数据，遮挡传感器数值根据遮挡来变化  
&nbsp;Proximity：距离传感器，遮挡传感器数值根据遮挡来变化


4、Camera检测页面  
&nbsp;继续按B键，可以将页面切换到Camera页面，摄像头成像完整，比例正确。  
![](https://i.imgur.com/uw6MZIY.png)  


5、音频回路检测页面  
&nbsp;按B键，可以将页面切换到音频回路检测页面，该模块需要手动升级，按照LCD显示的文字操作，可以一键升级该模块。  
![](https://i.imgur.com/mI1cegV.png) ![](https://i.imgur.com/SfznaDV.png)
                                        
                                                                               
## 四、Reset  
&nbsp;当开发板出现异常或者需要手动复位时，可以按Reset按键复位程序。  
![](https://i.imgur.com/Fd6Tx2b.png)


备注：此项操作后，您个人设置的一些参数会被清零。

#NOTE:
购买开发板AliOS Things Starter Kit和Developer kit，请扫描天猫旗舰店二维码：

![](https://i.imgur.com/VxHonLB.png)

获取在线技术支持，请扫描技术支持群：

![](https://i.imgur.com/uyT4v0Y.png)
![](https://i.imgur.com/rqjjjA0.jpg)
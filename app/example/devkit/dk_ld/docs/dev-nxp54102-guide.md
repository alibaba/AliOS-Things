# 准备
**为了更快上手，使用前请先完成[开发准备](dev-prepare-guide.md) 。**

# __背景__
软件：[AliOS Things](https://github.com/alibaba/AliOS-Things)  
硬件：LPCXpresso54102开发板 + MK3080 Wifi模组 （<span data-type="color" style="color:rgb(38, 38, 38)"><span data-type="background" style="background-color:rgb(255, 255, 255)">Arduino对接开发板</span></span>）、 HTS221传感器模组  
目标：实现开发板与云端的交互，包括传感器数据上云显示和云端控制LED开关  

# 环境
#### USB驱动
下载并安装[LPCXpressoLink2 USB driver package (REV 2.0.2)](https://www.nxp.com/search?keyword=link2%20usb%20driver%20package)   
驱动安装成功以后，电脑连接开发板Link USB，会识别端口设备“LPC-Link2 UCom Port”  

#### 烧录工具
推荐使用NXP官方烧录工具[Flash Magic](http://www.flashmagictool.com/)，使用方法如下  



![flashmagic使用.PNG | left | 593x632](https://img.alicdn.com/tfs/TB16d7xXjTpK1RjSZKPXXa3UpXa-593-632.png)  

 
*Step1*
从 Device Database中选择 "LPC54102J512"
COM Port选择开发板连接的端口 "LPC-Link2 UCom Port"
Baud Rate设置 "115200"
Interface设置 "None(ISP)"
Oscillator(MHz)设置 "12.000"

*Step2*
勾选 "Erase blocks used by Firmware" 
* * 
*Step3* 
*选择编译生成的 hex文件* 
* *
*Step4* 
*如图*
* * 
*Step5*
*按住开发板 SW2按键不放，然后按 Reset键重启，开发板进入下载模式。*  
点击 "start" 开始烧录，烧录完成后提示 Finished，最后重启开发板。  


# __硬件__
#### 硬件接线



![开发板接线图.png | center | 764x356](https://img.alicdn.com/tfs/TB1VO7wXmzqK1RjSZFjXXblCFXa-1920-894.png)  



# 软件
#### 代码下载
*git clone https://github.com/alibaba/AliOS-Things.git 
*
*cd AliOS-Things 
*
*git checkout developer*
 
#### 设置三元组
请参考[《开发准备》](dev-prepare-guide)“LD创建产品和设备”章节创建并设置三元组 

#### 编译 
方法1：*aos make ldapp@lpcxpresso54102*
方法2：参考[《开发准备》](dev-prepare-guide)3.1节，使用IDE环境进行编译 
 
#### 烧录
使用Flash Magic把编译生成的ldapp@lpcxpresso54102.hex文件烧录到开发板，烧录成功后重启开发板  


# 运行
#### 配网
系统启动后，通过串口终端输入*netmgr connect <ssid> <password>*命令配网，配网成功后串口终端会输出如下日志  


![配网成功.PNG | left | 514x107](https://img.alicdn.com/tfs/TB1FlAwXXzqK1RjSZSgXXcpAVXa-662-138.png)  



#### 温湿度显示
配网成功后ldapp会每隔1秒采集一次温度和湿度，并将数据更新到云端显示  


![温湿度云端显示.PNG | left | 486x262](https://img.alicdn.com/tfs/TB1q5MyXcbpK1RjSZFyXXX_qFXa-804-434.png)  


#### LED开关控制
进入在线调试界面，选择调试功能：LED开关(cmd_led)， 方法：设置。cmd_led设置1，则控制LED点亮；设置0，则LED熄灭。   


![LED设备调试.PNG | left | 824x299](https://img.alicdn.com/tfs/TB1le.yXbvpK1RjSZFqXXcXUVXa-1677-608.png) 


# 附录
#### 参考资料
[LPCXpresso54102开发板用户手册](https://www.nxp.com/docs/en/user-guide/UM10850.pdf)

[LPCXpresso54102开发板原理图](https://www.nxp.com/downloads/en/schematics/LPCX54102_Schematic_RevA.pdf)

[HTS221传感器芯片手册](https://www.st.com/resource/en/datasheet/hts221.pdf)

[HTS221传感器模块接线图](https://www.st.com/resource/en/data_brief/steval-mki141v2.pdf)


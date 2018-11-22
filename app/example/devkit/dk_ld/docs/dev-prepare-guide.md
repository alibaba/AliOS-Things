## 约定
云端：指各种云平台，这里使用阿里云

设备端：本地硬件设备，这里指各种开发板

Link Develop：阿里云提供的物联网一站式开发平台，详细查看[链接](https://linkdevelop.aliyun.com/developGuide#index.html)

Arduino接口：Arduino标准接口，本文开发板或外接wifi模组均follow Arduino接口，详细查看[链接](https://www.arduino.cc/en/main/arduinoBoardUno?setlang=cn)

IDE：集成开发环境，这里指使用VSC + alios-studio + aos-cube, 支持开发编译、运行调试，详细查看[链接](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-Studio)

AliOS Things：AliOS 家族旗下的、面向 IoT 领域的、高可伸缩的、开源的物联网操作系统，具体细节移步[github](https://github.com/alibaba/AliOS-Things)查看

## **本文目的**
本系列文档主要讲述基于AliOS Things的几种开发板上进行IOT设备端的开发，结合Link Develop平台，实现本地设备端和云端的数据交互，如设备端温湿度数据上云，云端下发命令控制设备端板载资源(如LED灯的开/关)等。让您对IOT设备端的开发流程、与云端交互等能快速熟悉入门。

## 开发准备
### 1) 软硬件环境搭建
硬件：开发板 **[1]** ，wifi模组 **[2]**，温湿度传感器**[3]**    
软件：AliOS-Things代码 **[4]**， IDE搭建  

其中，下图1为接线示意图，wifi模组通过Arduino接口直接插在MCU开发板，sensor经过I2C(Adruino接口)最终连接到MCU端的I2C总线上。硬件搭建具体细节请参考下述**[1]**选择对应的开发板文档进行，目标是将开发板、wifi模组和传感器均连接正确。  

**[1]**开发板：支持 [STM32L476rg-nucleo开发板](dev-stm32l476-guide.md)、[NXP LPCX54102开发板](dev-nxp54102-guide.md)、[TI MSP432P411开发板](dev-timsp432p411-guide.md)  
**[2]**wifi模组：本文以MK3080为例  
**[3]**温湿度sensor：本文以ST HTS221为例  
**[4]**AliOS Things代码:   github developer分支，[https://github.com/alibaba/AliOS-Things/tree/developer](https://github.com/alibaba/AliOS-Things/tree/developer)，git安装可参考[这里](http://git.oschina.net/progit/1-%E8%B5%B7%E6%AD%A5.html#1.4-%E5%AE%89%E8%A3%85-Git)，代码获取方式：
AliOS Things 项目主页 https://github.com/alibaba/AliOS-Things  
使用Git获取代码：  
```git
git clone https://github.com/alibaba/AliOS-Things.git
```
Note: 如遇网络问题可使用国内镜像地址替代  
```
git clone https://gitee.com/alios-things/AliOS-Things.git
```
推荐使用开发者分支:
```
git checkout -b developer origin/developer
```
       
![imag](https://img.alicdn.com/tfs/TB1rno3wkUmBKNjSZFOXXab2XXa-1357-617.png)  
图1  MCU/WIFI/SENSOR接线示意图



### 2) IDE环境搭建
AliOS Studio是基于VSC(Visual Studio Code)开发的功能强大跨平台的集成开发环境(IDE)，结合aos-cube强大的命令，能帮助用户快速高效地完成基于AliOS Things的代码编辑、编译、烧录、调试等一系列开发工作。  
IDE环境搭建包括：VSC + alios-studio + aos-cube, 详细查看[这里](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-Studio)  

#### 2.1) 下载/安装VSC
[https://code.visualstudio.com/](https://code.visualstudio.com/)，可支持LINUX、MAC OS、WINDOWS。
#### 2.2) 安装alios-studio插件和C/C++插件
步骤：查看-->扩展，或点击VSC左边"扩展"快捷键(如下图2所示)，在商店里面搜索后进行安装。除了"扩展"用来安装第三方插件外，常见的还有："资源管理器"可打开代码路径用于编辑(打开上述已下载的AliOS Things代码)；"调试"用于在线调试；
![VSC安装alios-studio插件|center](https://img.alicdn.com/tfs/TB1FqdXwA7mBKNjSZFyXXbydFXa-984-678.png)  
图2 VSC安装alios-studio插件

在完成2.1)和2.2)后，重启VSC**[6]**，此时会弹出图3，"Would you like to install aos-cube now?"点击 "Yes"，过一会将会提示安装成功，此时安装的是虚拟环境，即在~/.aos/python-venv建立虚拟环境(依赖下述2.3节中已完成)。
**[6]**Tips: 重启VSC可使用快捷键，shift+ctrl+P, 选择重启窗口(Reload)。 


![image.png | left | 826x230](https://img.alicdn.com/tfs/TB1ArWtwoOWBKNjSZKzXXXfWFXa-1359-379.png)
图3 重启后aos-cube安装



**2.3) 安装python和aos-cube**
   aos-cube是AliOS Things基于Python开发的项目管理工具包，依赖 Python 2.7 版本（64bits，2.7.14已验证）。具体安装步骤请参考[安装python和aos-cube](install-guide-aos-cube.md)。
   同时针对开发者的实际情况，还给出一种简单方便且不影响当前系统环境的方法----基于虚拟环境virtualenv的方法，以便备用。如何确认安装成功，在IDE中输入 `aos --version` 如下图。
![image.png](https://img.alicdn.com/tfs/TB1OUR8wXooBKNjSZPhXXc2CXXa-1171-506.png)
图4 IDE中aos-cube命令


#### 3）IDE使用
IDE提供了编译、烧录、串口监控、创建工程、调试等功能或相应的接口，目标是让开发者可以省去搭建开发环境、研究一堆工具等时间，直接上手开发调试。各功能分别介绍，等同的命令方式如下表**[1]**所示，即开发者可使用IDE也可直接使用命令。
这里我们基于stm32l476rg-nucleo 开发板来介绍IDE的使用，其他板子可以此类推。也可参考1分30秒的视频: [基于IDE开始AliOS Things开发](http://v.youku.com/v_show/id_XMzU3OTE2MzI1Ng==.html)，视屏中的开发板是StarterKit(基于stm32L433)


| IDE | aos-cube | comments |
| --- | --- | --- |
| 编译 | aos make example@board | example和board按实际选择 |
| 烧录 | aos upload example@board | 依赖当前out目录的实际路径 |
| 串口监控 | aos monitor \$PORT \$BAUD | Port口和波特率，如COM3 115200 |
| 新建工程 | aos new \$prj\_name |  |
| 在线调试 | - | 详见下面描述 |

表1 IDE功能和aos-cube命令对应关系

**3.1) 编译**
在 Visual Studio Code 中打开下载好的AliOS Things代码目录(上述2.2节打开源码)，其中VSC下方的小图标从左至右功能分别是 `编译` `烧录` `串口工具` `创建工程` `清除`。如图所示，![image.png | left | 320.9142857142857x24](https://img.alicdn.com/tfs/TB1FJ0mwBjTBKNjSZFuXXb0HFXa-468-35.png)。点击左侧的选择`ldapp@stm32l476rg-nucleo` 编译目标，格式遵循 `应用名字@目标板名字` 的规则，点击它可以依次选择应用和目标板，如下图7所示。编译目标确定以后，点击![image | left](https://img.alicdn.com/tfs/TB1r2dHwrArBKNjSZFLXXc_dVXa-24-22.png) 开始编译。编译过程如果发现缺少 toolchain 则会自动下载解压到正确位置。
![image.png](https://img.alicdn.com/tfs/TB1C_8ewkZmBKNjSZPiXXXFNVXa-1412-1025.png)  
图5 IDE中选择编译目标


<strong>3.2) 烧录</strong><strong><span data-type="background" style="background-color:#FADB14">[7]</span></strong>
1. 通过 USB Micro 线缆连接好开发板(如stm32l476rg-nucleo)与电脑  
2. 点击下方工具栏![image.png | left | 24x24](https://img.alicdn.com/tfs/TB11eorwkomBKNjSZFqXXXtqVXa-27-27.png)闪电图标完成固件烧录，如下图6.  
>注: aos upload目前仅支持: esp32/esp8266/mk3060和stm32系列(默认)，如需增加可在开源代码进行适配，具体代码地址：\$aos/build/site\_scons/scons\_upload.py        
**小技巧（Linux）：**  
Linux中操作串口可能会有权限问题，可以采用下述方法  
* `$ sudo usermod -a -G dialout $(whoami)`,添加当前用户到 dialout 组，提供直接使用串口能力。  
* `$lsubs` 找到厂商ID。如：`Bus 002 Device 008: ID 1366:0105 SEGGER`,厂商ID为1366，新建`/etc/udev/rules.d/99-stlink-v2.rules`文件，在文件里面添加规则:`SUBSYSTEM=="usb", ATTR{idVendor}=="1366", MODE="666", GROUP="plugdev"`  



![image.png | left | 826x482](https://img.alicdn.com/tfs/TB1AHUVwlsmBKNjSZFsXXaXSVXa-1746-1020.png)    
图6 IDE中烧录功能  


**3.3) 串口监控**  
点击下方工具栏插头图标打开串口，连接目标板，第一次连接会提示填写串口设备名和波特率，再次点击![image.png | left | 30.96774193548387x24](https://img.alicdn.com/tfs/TB1wThewkZmBKNjSZPiXXXFNVXa-40-31.png)可以看到串口监控界面，也可以在这里输入命令进行交互。  
>注：这里如果打开串口出错，请注意你的用户是否有串口访问权限，**按`shift + ctrl + P`输入` >config serial monitor`后填写正确的port口和波特率，选择 '任务'--> '终止任务'可以停用需要停用的任务。  


![image.png | left | 826x520](https://img.alicdn.com/tfs/TB1VaILwnqWBKNjSZFAXXanSpXa-1623-1021.png)    

图7 IDE中串口监控功能



**3.4) 在线调试**
细节参考 [Starter-Kit-Tutorial#调试](https://github.com/alibaba/AliOS-Things/wiki/Starter-Kit-Tutorial#%E8%B0%83%E8%AF%95)或视频[使用 AliOS Studio 开始 AliOS Things 调试](http://v.youku.com/v_show/id_XMzU3OTE5ODE1Ng==.html)  
目前仅支持stm32系列，如需增加可在代码中增加相应的工具配置(.vscode/launch.json)  

## LD创建产品和设备
**4.1) 创建产品和设备**
LD平台: linkdevelop.aliyun.com，需求：  
a) report\_sensor, 设备端上报到云端，包括humi和temp。  
b) cmd\_led, 云端发送命令控制设备端LED灯开/关。  

**创建产品和自定义功能**：点击设备开发-->产品开发-->新增产品，本示例创建设备端如下图10。创建完成后，<span data-type="color" style="color:rgb(55, 61, 65)"><span data-type="background" style="background-color:rgb(255, 255, 255)">完成产品创建--&gt;进入开发, </span></span>在自定义功能那项选择'新增'，添加自定义功能1(设备端sensor数据上报)如图11和图12，此处是指本地设备端的温湿度数据。再新增自定义功能2，用于"云端控制设备端LED灯"，见图13.  
**创建设备**：点击 产品开发-->设备开发 --> 新增测试设备 -->输入设备名称，确认后即可完成设备的创建(图14)，随后生成三元组(图15)。  

![image.png | left | 800x391.93548387096774](https://img.alicdn.com/tfs/TB1FlRewrZnBKNjSZFhXXc.oXXa-1488-729.png)   
图10 LD设备产品创建


![image.png](https://img.alicdn.com/tfs/TB1GcRfwyMnBKNjSZFzXXc_qVXa-745-633.png)  
图11 定义设备端sensor数据上报功能（1/2）



![image.png](https://img.alicdn.com/tfs/TB1IEJ8wXooBKNjSZPhXXc2CXXa-1549-566.png)  
图12 定义设备端sensor数据上报功能（2/2）

![image.png | left | 400x332.6145552560647](https://img.alicdn.com/tfs/TB15YlBwrsrBKNjSZFpXXcXhFXa-742-617.png)  
图13 定义云端控制设备端LED灯 

![image.png | left | 826x358](https://img.alicdn.com/tfs/TB1V_4ewkZmBKNjSZPiXXXFNVXa-1880-814.png)  
图14 创建测试设备

![image.png | left | 420x280.1902173913044](https://img.alicdn.com/tfs/TB1zrIUwXkoBKNjSZFEXXbrEVXa-736-491.png)  
图15 三元组

__4.2) 三元组和标识符__
a) 将上面生成的实际三元组替换"PRODUCT\_KEY", "DEVICE\_NAME", "DEVICE\_SECRET"( 代码路径：example/ldapp/mqtt-example.c)。  
b) **标识符**需要与代码中对应起来，代码位置如：example/ldapp/mqtt-example.c，对应功能属性(图16)。  
#define PROP_POST_FORMAT_HUMITEMP    "{\"report_sensor\":{\"Humi\":%f,\"Temp\":%f}}"  
#define PROP_SET_FORMAT_CMDLED       "\"cmd_led\":"  

![image.png | left | 826x358](https://img.alicdn.com/tfs/TB1VTNewkZmBKNjSZPiXXXFNVXa-1467-488.png)    
图16 设备功能属性--标识符  


## 设备端开发调试
详细内容请进入查看，支持列表如下：  

[STM32L476rg-nucleo开发板](dev-stm32l476-guide.md)  

[NXP LPCX54102开发板](dev-nxp54102-guide.md)  

[TI MSP432P411开发板](dev-timsp432p411-guide.md)  
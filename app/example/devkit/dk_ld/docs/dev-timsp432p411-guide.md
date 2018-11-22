# 准备
__为了更快上手，使用前请先完成__[开发准备](dev-prepare-guide.md)__。__
## 背景

软件：[Alios Things](https://github.com/alibaba/AliOS-Things)

硬件：[TI MSP432P4111 launch pad](http://www.ti.com/tool/msp-exp432p4111) + mk3080 Wifi模组 、HTS221传感器模组

目标：实现开发板与云端的交互，包括传感器数据上云显示和云端控制LED开关

## 环境

### USB转串口驱动

下载并安装：[XDS-110 emulator](http://processors.wiki.ti.com/index.php/XDS_Emulation_Software_Package)

驱动安装完成以后可以看到两个串口:



![image | left](https://img.alicdn.com/tfs/TB1yv7AXkvoK1RjSZFNXXcxMVXa-394-98.png)  



其中Application/User UART为调试串口

### 烧录工具

推荐使用TI官方烧录工具： [UniFlash](http://processors.wiki.ti.com/index.php/Category:CCS_UniFlash)，使用方法如下：



![image | left](https://img.alicdn.com/tfs/TB1cSkxXmrqK1RjSZK9XXXyypXa-1296-763.jpg)  


*Step 1*

打开Uniflash会自动识别已经插入的开发板，点击__<span data-type="color" style="color:#2F54EB">Start</span>__



![image | left](https://img.alicdn.com/tfs/TB1y67xXmzqK1RjSZPcXXbTepXa-1330-939.jpg)


*Step 2*

勾选*Erase and download necessary  segments only* 

*Step 3*

*Browse* 选择编译生成的bin文件

Step 4

点击 *Load Image*

## 硬件连接



![image | left](https://img.alicdn.com/tfs/TB1A.3xXgDqK1RjSZSyXXaxEVXa-992-744.jpg) 


## 软件

### 代码下载

```bash
git clone https://github.com/alibaba/AliOS-Things.git
git checkout developer

Note: 如遇网络问题可使用国内镜像地址替代，如下：
git clone https://gitee.com/alios-things/AliOS-Things.git
```

### 设置本设备的三元组

请参考[《开发准备》](dev-prepare-guide)"LD创建产品和设备"章节创建并设置三元组。

### 编译

方法1：*aos make ldapp@msp432p4111launchpad*

方法2：参考[《开发准备》](dev-prepare-guide)，使用IDE环境进行编译

### 烧录

使用UniFlash将编译生成的ldapp@msp432p4111launchpad.bin文件烧录到开发板

## 运行

### 配网

系统启动后，通过串口终端输入*netmgr connect  *命令配网，配网成功后串口终端会输出如下日志



![image | left](https://img.alicdn.com/tfs/TB1CPIzXbvpK1RjSZPiXXbmwXXa-662-138.png)


### 温湿度云端显示

配网成功后ldapp会每隔1秒采集一次温度和湿度，并将数据更新到云端显示



![image | left](https://img.alicdn.com/tfs/TB1W6QyXXzqK1RjSZSgXXcpAVXa-804-434.png)


### LED开关控制

进入在线调试界面，选择调试功能：LED开关(cmd\_led)， 方法：设置。cmd\_led设置1，则控制LED1点亮；设置0，则LED1熄灭。



![image | left](https://img.alicdn.com/tfs/TB1PGcyXgHqK1RjSZFPXXcwapXa-1677-608.png)


## 附录

相关硬件参考资料

[MSP432P4111 LaunchPad User's Guide](http://www.ti.com/lit/ug/slau747a/slau747a.pdf)

[HTS221传感器芯片手册](https://www.st.com/resource/en/datasheet/hts221.pdf)

[HTS221传感器模块接线图](https://www.st.com/resource/en/data_brief/steval-mki141v2.pdf)

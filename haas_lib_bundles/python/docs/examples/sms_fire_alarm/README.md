# 基于4G-Cat1的火焰检测短信报警系统

## 1、简介
&emsp;&emsp;
本案例通过火焰传感器实时监测是否产生明火，当一段时间连续监测到明火会向用户发送3条报警短信。


### 1.1、原理
&emsp;&emsp;
火焰传感器通过检测火焰中产生的红外线来监测是否着火。远红外火焰传感器能够探测到波长在700纳米～1000纳米范围内的红外光，探测角度为60，其中红外光波长在880纳米附近时，其灵敏度达到最大。远红外火焰探头将外界红外光的强弱变化转化为电流的变化，通过A/D转换器后可以监测。


### 1.1、器件说明
&emsp;&emsp;
市面上买到的有3根引脚的也有4根引脚的， 本案例适用4个引脚的。
<div align="center">
<img src=./../../images/sms_fire_alarm/1.png width=80%/>
</div>
<br>
 
<div align="center">
<img src=./../../images/sms_fire_alarm/2.png width=80%/>
</div>

&emsp;&emsp;
对于4引脚的传感器，以下有几点需要说明：

 - **电源指示LED**--显示是否通电 	
 - **模拟信号输出**--输出模拟电压值 	
 - **红外线越强模拟信号输出越小，反之越大**
 - **灵敏度调节电位器**--用来调节判断阈值

	- **模拟信号输出**大于该阈值（红外线弱，没火），**开关信号输出**为1（高电平），**开关指示LED**灯灭
	- **模拟信号输出**小于该阈值（红外线强，着火），**开关信号输出**为0（低电平），**开关指示LED**灯亮
 

### 1.2、本案例需要的硬件
&emsp;&emsp;
器材|数量
|--|--|
HaaS506开发板|1
火焰传感器|1
SIM卡|1
杜邦线|若干

### 1.3、硬件连接图
&emsp;&emsp;
<div align="center">
<img src=./../../images/sms_fire_alarm/3.png width=80%/>
</div>
 
## 2、设备端开发

### 2.1、开发环境
&emsp;&emsp;
在进行下一步之前请确保HaaS506开发环境已经搭建完毕。详情请参考[HaaS506 快速开始](https://haas.iot.aliyun.com/haasapi/index.html#/Python/docs/zh-CN/startup/HaaS506_startup)的说明。
<br>

### 2.2、创建解决方案

&emsp;&emsp;
如下图所示，在Haas Studio中创建项目。先选择左侧的“开发板型号”再从右侧的案例中选择“GPS定位和紧急拨号”案例点击“立即创建”即可。

<div align="center">
<img src=./../../images/HaaS_Studio_创建工程示范.png width=100%/>
</div>
<br>

### 2.3、修改代码
&emsp;&emsp;
修改工程里main.py中以下代码段

```python

###############################
# 需要发送短信的目标电话号码，长度为11的字符串格式
tele_phone = 'xxxxxxxxxxx'
###############################

```

## 3、运行结果

### 3.1、串口log
<div align="center">
<img src=./../../images/sms_fire_alarm/4.png width=100%/>
</div>
 
### 3.2、手机接收到的短信。
<div align="center">
<img src=./../../images/sms_fire_alarm/5.png width=100%/>
</div>
 

> **学习物联网，来HaaSEDU就对了**

# HaaS EDU K1硬件介绍
&emsp;&emsp;
HaaS EDU K1是HaaS Education Kit1的缩写，是基于四核高性能MCU-HaaS1000芯片打造的、集颜值和内涵于一身的物联网教育开发板。作为云端钉一体全链路解决方案的软硬件积木平台，深度集成了AliOS Things物联网操作系统、HaaS轻应用、小程序和阿里云物联网平台等技术和服务，让开发者可以轻松的学习和开发云端钉全链路实战项目，解决实际场景或孵化创新应用，下图是其全景图：

<div align="center">
<img src=../../images/ext_HaaS_EDU_K1全景图.png  width=100%/>
</div>

HaaS EDU K1 六大特点：
* 高颜值 - 有别于传统的裸板开发板，HaaS EDU K1外观鲜艳靓丽，洋溢青春活力。
* 可移动 - 内置1200mAh锂电池支持移动场景，OLED屏幕和游戏键盘设计提高可玩性。
* 配置丰富 - 采用定制的四核(Cortex-A7双核1GHz和Cortex-M33双核300MHz)高性能MCU-HaaS1000芯片，自带16M Bytes FLASH、16M Bytes PSRAM和2.5M  Bytes SRAM，内置双频Wi-Fi和BT5.0天线，板载丰富的物联网传感器（加速度、陀螺仪、磁力计、温湿度、大气压、环境光和声音等），可开发丰富的AIoT应用场景或解决方案。
* 安全性 - 内嵌的Z8IDA金融级国密芯片，为数据安全保驾护航。
* 方便灵活 - 各接口有明确的标注，操作顺手，仅一条Type C数据线即可完成烧录、调试和充电，非常方便。
* 可扩展 - 开发板的资源都可以灵活配置，30PIN扩展接口和SD卡槽满足更多应用场景需求。

## 1 整机配置
### 1.1 硬件接口
<div align="center">
<img src=../../images/ext_haaseduk1整机接口.png  width=100%/>
</div>

> 如上图所示HaaS EDU K1接口资源丰富，外形大小合理：为94.4mm * 63mm * 20 mm，充分考虑手持携带的便利性。其接口定义如下表：

| 名称 | 数量 | 描述 | 
| :- | :-: |  :- |
| OLED屏幕 | 1 |  1.3寸，分辨率128*64 |
| 环境光传感器 | 1 |  型号：AP3216C |			
| 指示灯 | 4 |  白色电源指示灯，RGB单色可编程 |
| 按键 | 5 | 1个小孔复位键，4个可编程按键 |
| 电源开关 | 1 |  电源ON/OFF拨动开关 |
| TF卡槽 | 1 |  最大支持64GB |			
| USB接口 | 1 |  Type C接口，可充电/烧录/调试 |
| 扩展卡槽 | 1 | 30PIN简牛母座 |
| 蜂鸣器排孔 | 4 | 蜂鸣器及温湿度检测对流孔 |

### 1.2 扩展接口
除已有板载功能之外，还有30PIN扩展接口，最大限度释放HaaS1000芯片的资源，满足开发者更多的应用需求。主要扩展有1路ADC输入、1路USB HOST、1路SWD调试、2路SPK输出、3路MIC输入、8路GPIOs等，其中GPIO_P02和GPIO_P03与主板的传感器一起复用为I2C模式，其他6个GPIOs可随便定义，并且每路GPIO的最大驱动电流是10mA，具体接口定义如下图所示：
<div align="center">
<img src=../../images/ext_haaseduk1扩展接口.png  width=100%/>
</div>

### 1.3 电气性能
| 工作电压 | 3.5v~5v |
| :- | :- |
|充电电流| 450mA，可充电锂电池|
|工作温度|-20～85度|
|环境温度|5～85%RH（无凝结)|

## 2 板载资源
### 2.1 主板功能
<div align="center">
<img src=../../images/ext_haaseduk1主板功能.png  width=100%/>
</div>

如上图所示，HaaS EDU K1主板功能非常丰富，一大部分以板载功能呈现，比如：蓝牙/Wi-Fi、OLED屏幕、传感器等，另外还有30PIN扩展接口可以使用，尽可能释放HaaS1000芯片的开发资源，满足开发者的应用需求。主板大小为89.98mm* 49.98mm ，充分考虑黄金分割比例，使外形更合理美观，具体板载配置如下表：
| 名称 | 描述 |
| :- | :- |
|CPU型号| HaaS1000|
|CPU架构|Dual Cortex-M33,Dual Cortex-A7|
|CPU主频|Cortex-M33 up to 300MHz,Cortex-A7 up to 1GHz|
|片上Flash| 16MB|
|内存|2.5MB SRAM，16MB PSRAM |
|蓝牙|蓝牙5.0，支持BLE mesh|
|Wi-Fi|支持2.4G/5G双频|
|充电管理|可充电锂电池1200mAh，充电电流450mA|
|传感器|内置加速度、陀螺仪、地磁、大气压、温湿度、光照等|

### 2.2 芯片资源
HaaS1000是一颗高度集成的SoC，其主要集成了如下模块：

（1）Cortex-M33双核心和Cortex-A7双核心<br>
（2）支持内部16MB PSRAM，2.5MB SRAM，16MB NOR flash<br>
（3）2.4G/5G双频 Wi-Fi 4（1x1 802.11 a/b/g/n）<br>
（4）双模蓝牙5.0<br>
（5）音频编解码模块<br>
（6）电源管理功能<br>
（7）远程声场的三路模拟麦克风阵列和六路数字麦克风阵列<br>

<div align="center">
<img src=../../images/ext_haas1000芯片框架.png  width=100%/>
</div>

HaaS1000的M33核心可以运行蓝牙协议栈，A7核心可以运行语音处理和AI算法任务。其中Wi-Fi和蓝牙支持共天线和双天线方案，为性能和成本提供了灵活的可选择方案。其高集成度让开发者可以用很少的外围组件来设计完成一个完整的解决方案。HaaS1000芯片CPU及蓝牙/Wi-Fi主要特性如下：

#### 2.2.1 CPU特性
（1）集成了PMU, CODEC, RF, BB, MCU和AP子系统的CMOS 单芯片<br>
（2）MCU 子系统采用300MHz ARM Cortex-M33 Star双核<br>
（3）AP 子系统采用具有NEON功能的1GHz CortexA7双核<br>
（4）片内共享2.5MB SRAM<br>
（5）16MB片上PSRAM<br>
（6）16MB片上QSPI NOR Flash<br>
（7）支持硬件加密引擎<br>
（8）支持 TrustZone<br>
（9）支持安全启动

#### 2.2.2 BT/Wi-Fi
（1）Wi-Fi® IEEE 802.11 a/b/g/n<br>
（2）支持 2.4GHz Wi-Fi<br>
（3）支持 20MHz, 40MHz 带宽模式<br>
（4）Bluetooth® v5.0 双模式<br>
（5）支持 BLE Mesh<br>
（6）A2DP v1.3/AVRCP v1.5/HFP v1.6<br>
（7）支持 Wi-Fi/BT共存<br>

#### 2.2.3 外围接口特性
（1） 支持USB2.0 HS<br>
（2）支持3路UART，最高可到6Mbps<br>
（3）支持2路SPI，最高可到50Mbps，支持串口LCD<br>
（4）支持2路I2C，最高可到1.4Mbps<br>
（5）支持4路PWM<br>
（6）支持3路 10-bit GPADC，7 channels<br>
（7）支持40个GPIO可编程，每路都有独立中断<br>

## 学习指南
为了便于开发者快速上HaaS实战项目，我们还打造了一系列基于HaaS EDU K1的案例，并上传到HaaS技术社区，大家可以下载学习。如需更多技术支持，可加入钉钉开发者群，或者关注微信公众号。
<div align="center">
<img src=../../images/ext_haas社区.png  width=100%/>
</div>

更多技术与解决方案介绍，请访问HaaS官网(http://haas.iot.aliyun.com)。
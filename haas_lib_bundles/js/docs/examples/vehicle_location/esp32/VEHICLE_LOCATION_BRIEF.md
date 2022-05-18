# 车辆定位系统
## 简介
&emsp;&emsp;近年来，全球掀起了一股自动驾驶热潮，无论是传统车企，还是科技企业，甚至很多初创公司都竞相加入这一行业赛道。进入2021年，自动驾驶热度不减，且“吸金”不断，据不完全统计，从今年年初至今，自动驾驶行业投融资事件超过50起，投融资金额近1000亿元，已超过2018年，达到历年最高值。

## 准备

硬件器材：
1. ESP32开发板一套 (NODEMCU32S/ M5STACKCORE2)
2. GNSS定位模块 HT2828Z3G5L
3. 连接线若干

NODEMCU32S硬件连线如下表所示：

<div align="center">
<img src= https://hli.aliyuncs.com/o/config/examples_images/5_3_车辆定位硬件连接_esp32.jpg
 width=60%/>
</div>
<br>

M5STACKCORE2硬件连线如下表所示：

|传感器引脚|板级pin脚|说明|
|----|----|----|
| VCC| 3.3V| GNSS定位模块供电点位 |
| TX | G13 | SHT3X传感器SDA点位 |
| RX | G14 | LED传感器地点位 |
| GND| GND | GNSS定位模块地点位 |

## 立即体验
点击右下方按钮即可立即体验。

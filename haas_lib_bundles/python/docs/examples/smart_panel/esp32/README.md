# 智能面板UI交互案列
&emsp;&emsp;
智能面板是智能家居系统中的核心，不但可以获取展示各种传感器信息，还可以对设备进行控制；本demo是智能面板的案例，大家可以参考本Demo实现智能面板的UI交互。

## 背景知识
&emsp;&emsp;
本Demo图形库使用的是LittleVGL 8.1.0版本，全部API参考：https://docs.lvgl.io/8.1/index.html

## 场景功能拆解
&emsp;&emsp;
实验整体步骤如下：
1. 设备端开发
2. 设备运行

## 准备

1. M5Stack-Core2开发板    一套

涉及到的硬件购买链接如下，仅供参考，不负责商家发货的品质保障等问题！

| 名称 | 数量 | 参考链接 |
| --- | --- | --- |
| M5Stack-Core2开发版 | 1 | [M5Stack-Core2](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-22404213529.17.732749d8usCqYX&id=625561056791) |
| microUSB数据线 | 1 | M5Stack-Core2自带 |

> 功能描述

本Demo共7个界面，包括：桌面，环境温度/湿度，音乐播放，计时器，指北针，噪声监控

## 设备端开发

### 开发环境
&emsp;&emsp;
在进行下一步之前请确保ESP32开发环境已经搭建完毕。详情请参考[esp32开发环境](../../../startup/ESP32_startup.md)的说明。

&emsp;&emsp;
打开VS Code之后在新建一个基于helloworld的python工程，设定好工程名称（“smart_panel”）及工作区路径之后，硬件类型选择m5stackcore2，点击立即创建，创建一个Python轻应用的解决方案。

<div align="center">
<img src=./../../../images/6_smart_panel_create_project.jpg width=100%/>
</div>

&emsp;&emsp;
将[smart_panel](./code/)文件下的所有脚本进行复制到“smart_panel”工程根目录中，连接设备后点击"部署运行"，安装提示进行部署


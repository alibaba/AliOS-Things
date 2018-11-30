
## 简介
本系列文档主要讲述基于AliOS Things的几种开发板上进行IOT设备端的开发，结合Link Develop平台，实现本地设备端和云端的数据交互，如设备端温湿度数据上云，云端下发命令控制设备端板载资源(如LED灯的开/关)等。让您对IOT设备端的开发流程、与云端交互等能快速熟悉入门。

## 支持开发板

[STM32L476rg-nucleo开发板](docs/dev-stm32l476-guide.md)

[NXP LPCX54102开发板](docs/dev-nxp54102-guide.md)

[TI MSP432P411开发板](docs/dev-timsp432p411-guide.md)

[Developerkit开发板](http://aliosthings.io/#/developerkit)

>注：Developerkit不需要外接额外的wifi模组（板载已含BK7231）,开发过程类似(参考STM32L476rg-nucleo开发板)

## 编译
```
aos make ldapp@developerkit
aos make ldapp@stm32l476rg-nucleo
aos make ldapp@lpcxpresso54102
aos make ldapp@msp432p4111launchpad
```

## 烧录运行
具体请查看[开发板](#支持开发板)

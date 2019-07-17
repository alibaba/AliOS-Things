[目录]

[1 准备st-flash工具](#1-准备st-flash工具)

[2 使用st-flash命令/工具烧写固件](#2-使用st-flash工具烧写固件)

​	[MAC系统下烧录](#mac系统下烧录)

​   [LINUX系统烧录](#linux系统烧录)

​	[Windows系统烧录](#windows系统烧录)

​	

# STM32固件烧录指南

文档说明： 

 为了能在STM32上面开发TinyEngine JavaScript应用，需先烧录TinyEngine固件到STM32芯片中。

 本文将介绍如何在MAC系统、Ubuntu系统、Windows系统上烧录TinyEngine固件。



## 1 准备st-flash工具

stm32烧录固件，需要使用st-flash工具，在TinyEngine仓库的【tools/flash-burn-tools】路径下，我们已经

下载了 osx(mac) 、linux64、windows的st-flash可执行文件/安装包。

如下图所示：

```
.
├── WebIDE
├── drivers
└── flash-burn-tools
    ├── linux64
    │   ├── st-flash
    │   ├── st-info
    │   └── st-util
    ├── osx
    │   ├── st-flash
    │   ├── st-info
    │   └── st-util
    └── windows
        └── STM32\ ST-LINK\ Utility\ v4.2.0\ setup.exe
```



* **MAC系统**： 无需安装，直接使用st-flash可执行文件即可。

* **LINUX系统**： 无需安装，直接使用st-flash可执行文件即可。

* **WINDOWS系统**： 点击安装  “STM32\ ST-LINK\ Utility\ v4.2.0\ setup.exe” 文件，并根据提示安装st-link的驱动。安装成功后，插入st-link usb口到PC，查看设备管理器-》端口会出现 “STMicroelectronocs STLink Virtual Com Port”端口。

  

**FAQ**：如未覆盖到您的主机操作系统或者TinyEngine仓库目录下的st-flash执行不了，请参考官方github文档下载st-link源码编译生成st-flash工具使用。

st-flash工具github文档地址：https://github.com/texane/stlink



### 2 使用st-flash工具烧写固件

拷贝对应主机系统的st-flash到待烧录固件的同一目录，然后执行对应st-flash命令即可。

**这里以烧录developerkit的tinyengine固件举例**：

`说明：请根据待烧录固件的实际名称将如下命令的developerkit-xxx.bin替换成firmware下的真实文件名`



#### MAC系统下烧录

* 拷贝tools/flash-burn-tools/osx/st-flash到firmware/stm32/developerkit/目录下。
* 然后进入firmware/stm32/developerkit/下执行如下命令即可：

```
./st-flash --reset write developerkit-xxx.bin 0x8000000
```



#### LINUX系统烧录

* 64bit 系统：拷贝tools/flash-burn-tools/linux64/st-flash到firmware/stm32/developerkit/目录下。
* 32bit 系统：拷贝tools/flash-burn-tools/linux32/st-flash到firmware/stm32/developerkit/目录下。
* 然后进入firmware/stm32/developerkit/下执行如下命令即可：

```
./st-flash --reset write developerkit-xxx.bin 0x8000000
```

#### Windows系统烧录

* 第一步： 双击打开桌面上的 **STM32 ST-LINK Utility** 图标，点击File-》Open files，并选择TinyEngine仓库中firmware里面的固件bin文件。

![](https://img.alicdn.com/tfs/TB1zLCcvSMmBKNjSZTEXXasKpXa-1076-840.jpg )



例如，本例中烧写developerkit，则选择firmware/stm32/developerkit/xxx.bin文件

![](https://img.alicdn.com/tfs/TB1v7z8vOMnBKNjSZFoXXbOSFXa-1452-838.jpg)



* 第二步，连接STM32芯片的ST-LINK usb口到PC，并点击Target->Connect 

![](https://img.alicdn.com/tfs/TB1M6BXv_mWBKNjSZFBXXXxUFXa-1580-920.jpg)



* 第三步，确认Address 为0x08000000,及stlink是否连接成功，连接成功会显示Device信息。

![](https://img.alicdn.com/tfs/TB13M0KvJknBKNjSZKPXXX6OFXa-1336-826.jpg)



* 第四步，点击Target-》Pragram 开始烧录

![](https://img.alicdn.com/tfs/TB1wdwGvLImBKNjSZFlXXc43FXa-1104-874.jpg)



* 最后，等待烧录完成，即进度条跑完。

![](https://img.alicdn.com/tfs/TB12h.5vH3nBKNjSZFMXXaUSFXa-1092-1162.jpg)





 




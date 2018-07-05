EN | [中文](uDevice-Center.zh)

# uDevice Center - an elastic multi-device IoT development platform

### Abstract

There have long been two problems for IoT developers. One is the difficulty in acquiring hardwares and completing set-up path. The other is the lack of tool that can support multi-device joint development. In order to solve them, we proposed uDevice Center to put all the development boards and devices on-line, which can support multi devices as well. 

### Background

IoT development is not an easy task. For beginners, there are firstly many elementary things to do before starting the first line of code. You need to buy development boards, know how to connect all the hardwares, connect them with the computer, download development software and get familiar with it. Besides, all these steps need certain professional knowledge, which may upset many untrained ones.

![img](https://img.alicdn.com/tfs/TB1C2ximiqAXuNjy1XdXXaYcVXa-1282-959.png)

Even for those with experience in embedded system, IoT brings about new problems. Traditionally, one board and one computer are enough for development. However, in terms of IoT, there are numerous devices involved, for example in a smart home. Since there is no platform supporting multi-device development, it wastes great time and energy shuttling among different computers or interfaces. 

In order to solve these two problems, we proposed uDevice Center.

### Introduction of uDevcie Center

uDevice Center solves the above problems by:

1. moving IoT devices on-line as an IoT Elastic Compute Service (ECS).

2.      enabling elastic multi-device development.

#### ECS for IoT

uDevice Center integrates all the development devices and boards on-line, so that developers can view it as an IoT ECS. Developers can start working just by stalling one software, and emphasize more on further work efficiently. The advantages of using uDevice Center include:

·       no need to buy hardwares, simpling the starting process

·       no limitation in time and place

·       various types of development boards to choose from

·       flexible number of development boards that can support

In addition, for development in certain application scenarios (such as smart home), an elastic platform can help developers to utilize public resources (such as labs for smart home) to start validation quickly, reducing development threshold and saving costs. 

![architecture.png](https://img.alicdn.com/tfs/TB1hdFZoBfH8KJjy1XbXXbLdXXa-5791-3120.png)

The architecture of uDevice Center is shown as above. It's mainly composed of three parts: IoT hardwares, uDevice Center infrastructure and AliOS Studio. IoT hardwares in this system are development boards, smart devices and labs provided by Ali and its vendors. uDevice Center infrastructure use standardized way to eliminate the differences in interfaces among different hardwares in the process of programing, debugging, sensing and controlling. It virtualizes all the IoT hardwares in a unified way, and offer them for users. AliOS Studio provides users a friendly platform to use uDevice Center Infrastructure.

Besides, uDevice Center is an open platform that can provide matching softwares to help connect devices (such as chips and modules) to it easily.

#### multi-device joint development 

uDevice Center has been deeply integrated with AliOS Studio Integrated Development Environment (IDE) , providing IoT developers with capability of multi-device joint development. The user interface of uDevice Center is shown as followed: The middle window shows all the connected devices, and each point means one of them; The floating window above point '07d'c' shows the real-time status information of that device; The line connecting each point shows the connecting status in this mesh network; The right window is the Shell interactive interface for device ‘1d1c'.

![image.png](https://img.alicdn.com/tfs/TB16t8ZoBfH8KJjy1XbXXbLdXXa-3999-2499.png)

Capabilities of uDevice Center include:

·     support code edition, compilation and hardware programing for multiple devices

·      support Shell command interface debugging and device control for multiple devices

·       can show real-time status of all the devices 

·       can graphically show the connecting status in uMesh network

All the above can accelerate the development of IoT applications.

### Softwares to install before using uDevice Center

There are three steps before using uDevice Center: 

1. download and install [Visual Studio Code](https://code.visualstudio.com/download)

2. install plug-ins for AliOS Studio: start Visual Studio Code, and install as followed

   ![image.png](https://img.alicdn.com/tfs/TB1KoTCovDH8KJjy1XcXXcpdXXa-3974-2227.png)

3. install AliOS Things code and compiler tool chain [Linux](AliOS-Things-Linux-Environment-Setup) [OSX](AliOS-Things-MAC-Environment-Setup) [Windows](AliOS-Things-Windows-Environment-Setup) (Optional）

### the use of uDevice Center

uDevice Center can be independently viewed as a developing and testing platform for multiple devices or as a component of AliOS Thing integrated development tool. 

Types of development boards that are currently supported:

Espressif-ESP32-DevKitC (Wifi+Bluetooth)

MXCHIP-MK3060 (Wifi)

MXCHIP-EML3047 (LoRA)

STM-Cortex M0, M3, M4, M4

#### get development board

Devices in uDevice Center are allocated by Access Key. Please e-mail  [aliosthings@service.aliyun.com](mailto:aliosthings@service.aliyun.com)  if you need it. The e-mail should contain the following information: 

1. personal information: name, contact information, corporate name (optional)
2. requirement for boards: type, number, utility time, etc.
3. brief introduction of your use

#### used independently as debugging tool 

uDevice Center can be independently used as debugging tool. In such a process, users can edit and compile code in other tools, and use the produced firmware as input in uDevice Center to debug. The detailed process is: generating .bin by using other tools -->programming firmwares by using uDevice Center-->debugging by using uDevice Center

Take MXCHIP3060 dev-board as an example, there are following five steps:
Step 1. edit and compile code with other tool and generate a .bin firmware that can be programed

Step 2. start uDevice Center and input your Access Key

![image.png](https://img.alicdn.com/tfs/TB1rADfoxrI8KJjy0FpXXb5hVXa-3999-2499.png)

![img](https://img.alicdn.com/tfs/TB1Y9j_or_I8KJjy1XaXXbsxpXa-3999-2508.png)

Step 3. get familiar with uDevice Center

![image.png](https://img.alicdn.com/tfs/TB17k_foxrI8KJjy0FpXXb5hVXa-3999-2497.png)

Step 4. program firmware
Right-click on a free MXCHIP3060 board, and select “Program” to program the firmware produced in Step 1 to it. 

Note：Please check the type of dev-board before programing. When firmware is programed to a wrong type of board, it won't work.

![image.png](https://img.alicdn.com/tfs/TB1OhvEovDH8KJjy1XcXXcpdXXa-3999-2499.png)

Step 5. Shell interactive debugging
Right-click on the board programed in step 4, and select “Shell” to start command line interaction with dev-board.

![image.png](https://img.alicdn.com/tfs/TB1sATfoxrI8KJjy0FpXXb5hVXa-3999-2499.png)

#### used as a component of AliOS Thing integrated development tool

As part of AliOS Things integrated development tool, the typical process of using is: set up an AliOS Things project --> edit and compile source code --> program or debug devices in uDevice Cente

Take MXCHIP3060 board as an example, there are following four steps:

Step 1. start a new project, edit and compile source code

![image.png](https://img.alicdn.com/tfs/TB1d24imiqAXuNjy1XdXXaYcVXa-3999-2499.png)

![image.png](https://img.alicdn.com/tfs/TB18Qn7or_I8KJjy1XaXXbsxpXa-3999-2499.png)![image.png](https://img.alicdn.com/tfs/TB1QwdimiqAXuNjy1XdXXaYcVXa-3999-2499.png)Step 2. start uDevice Center: the same operation steps as above 

Step 3. program the firmware 
Right-click on a free MXCHIP3060 board, select “Program Last Build” to program the firmware produced in Step 1 to it.

![image.png](https://img.alicdn.com/tfs/TB13pbEovDH8KJjy1XcXXcpdXXa-3999-2499.png)

Step 4.  Shell interactive debugging: the same operation steps as above 

### Functions that need further improve

1. support GDB code-level debugging
2. support more detailed device status display
3. better control ability on hardwares

### Summary

uDevice Center is an elastic multi-device IoT development platform. It enables developers to develop, test and debug IoT applications more easily. By virtualizing hardwares on-line, it helps new beginners to learn and start it quickly, and help improve the efficiency of professional developers.

### DingTalk group

You are welcomed to join our DingTalk group by scanning the QR code to try and talk about uDevice Center.

![image.png](https://img.alicdn.com/tfs/TB1VuzJovDH8KJjy1XcXXcpdXXa-750-990.jpg)

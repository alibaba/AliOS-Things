EN | [中文](Manual-API.zh)

# AliOS Things Kernel test guide

AliOS Things Kernel test aims to ensure that AliOS Things can be compatible with different hardware platforms. AliOS Things provides Kernel test case set, and users can export Kernel test reports after downloading use case sets to the hardware platform. The detailed test steps are shown below.

## 1. Download test case set

[AliOS Things Kernel test case set](https://github.com/alibaba/AliOS-Things/blob/master/test/testcase/certificate_test) is open-sourced with AliOS Things in GitHub. Users can find it in `{AliOS-Things-Root}/test/testcase/certificate_test` after download AliOS Things source code. The structure of files is: 

```
  .
  |___aos_test.c              # aos_* API test case set
  |___rhino_test.c            # krhino_* API test case set
  |___certificate_test.mk     # The makefile used to compile gcc
  |___cutest                  # API test structure
      |___cut.c
      |___cut.h
```

> `aos_test.c` has same function with `rhino_test.c`, except that `rhino_test.c` targets to MCU only with rhino. Generally, you only need one of them.
>
> You can download Kernel test case set at：[www.github.com/alibaba/AliOS-Things.git](https://www.github.com/alibaba/AliOS-Things.git)

## 2. Execute test case set

### 2.1 Modify test case set source code (aos_test.c/rhino_test.c)

The test case set has the following configuration items. Users can modify them according to actual situation. The configuration items are described below: 

| Item                              | Description                          | Default item |
| :-------------------------------- | :----------------------------------- | :----------- |
| SYSINFO\_MCU                      | MCU type                             | empty (must) |
| SYSINFO\_ARCH                     | MCU structure                        | empty (must) |
| SYSINFO\_BORAD                    | development board type               | empty (must) |
| TEST\_CONFIG\_MM\_ENABLED         | enable memory test                   | 1            |
| TEST\_CONFIG\_MALLOC\_MAX\_SIZE   | maximum size of heap memory          | 1024         |
| TEST\_CONFIG\_MALLOC\_FREE\_TIMES | times of assigning/ releasing memory | 100000       |
| TEST\_CONFIG\_TASK\_ENABLED       | enable task test                     | 1            |
| TEST\_CONFIG\_STACK\_SIZE         | size of task stack \(Byte\)          | 1024         |
| TEST\_CONFIG\_MAX\_TASK\_COUNT    | maximum task number                  | 10           |
| TEST\_CONFIG\_CREATE\_TASK\_TIMES | times of creating/ deleting task     | 10000        |
| TEST\_CONFIG\_TASK\_COMM\_ENABLED | enable communication between tasks   | 1            |
| TEST\_CONFIG\_SYNC\_TIMES         | times of intertask synchronization   | 100000       |
| TEST\_CONFIG\_TIMER\_ENABLED      | enable timer test                    | 1            |
| TEST\_CONFIG\_KV\_ENABLED         | enable KV test                       | 1            |
| TEST\_CONFIG\_KV\_TIMES           | times of KV reading/writing          | 10000        |
| TEST\_CONFIG\_YLOOP\_ENABLED      | enable Yloop test                    | 1            |
| TEST\_CONFIG\_YLOOP\_EVENT\_COUNT | Yloop event number                   | 1000         |
| TEST\_CONFIG\_YLOOP\_LOOP\_COUNT  | maximum Yloop number                 | 10           |

### 2.1.1 gcc compilation

The API test case set uses GCC to compile by default. Users can generate test firmware by executing the following commands. The firmware will be stored under {AliOS-Things-Root}/out directory.

```
$ aos make yts@{board} test=certificate
```

> gcc can't be used for MCU only with rhino.

### 2.1.2 Keil/IAR compilation

API test case set use gcc for compilation by default, but it can be migrated to Keil/IAR project. You can call the function as followed: 

```cpp
/* transplante aos_test.c and cutest or rhino_test.c and cutest to Keil/IAR project and call that interface */
void certificate_test(void);
```

### 2.1.3 Export test report 

When the test firmware is successfully executed in development board, you can use [IoT test tool](http://alios-things.oss-cn-shanghai.aliyuncs.com/AliOSThings/IoT调试测试工具.exe) to export the result as pfd file. This file needs to be submit 将测试结果导出为PDF文档，该文档将作为AliOS Things Kernel in certification application stage.

> You can download IoT test tool at: http://alios-things.oss-cn-shanghai.aliyuncs.com/AliOSThings/IoT%E8%B0%83%E8%AF%95%E6%B5%8B%E8%AF%95%E5%B7%A5%E5%85%B7.exe](http://alios-things.oss-cn-shanghai.aliyuncs.com/AliOSThings/IoT调试测试工具.exe)

 IoT test tool use steps: 

- Select serial port number, baud rate, data bits, stop bits, parity bits.
- Turn on the device. If the device is successfully turned on, `turn on` button will become  `turn off`.
- Restart the device. In normal condition, it will display as the following picture. Wait for the end of the test if you confirm the MCU, ARCH, development board, kernel version are right.

- During the test, test progress, case execution status and configuration of test case set will be displayed. 
- When the test is completed, `export test report` will enable users to export the test report in PDF format, and the report will be uploaded to the Ali cloud and return to the link address. See the following figure

![](assets/certification_api_tool.png)  
![](assets/certification_api_tool2.png)


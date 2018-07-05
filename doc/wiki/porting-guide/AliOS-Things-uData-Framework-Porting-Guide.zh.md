[EN](AliOS-Things-uData-Framework-Porting-Guide)| 中文

#### uData框架简介

uData框架设计之初的思想是基于传统sensorhub概念基础之上的，结合IoT的业务场景和AliOS Things物联网操作系统的特点设计而成的一个面对IoT的感知设备处理框架。uData主要分kernel和framework两层，kernel层主要是负责传感器驱动和相关的静态校准，包括轴向校准等；framework层主要是负责应用服务管理、动态校准管理和对外模块接口等。有关uData的具体设计将不再此文档中展开，下图可作为一个概要参考。本文档主要是介绍如何在现有的AliOS Things物联网操作系统上移植uData软件框架。
![](https://i.imgur.com/iSaUEYR.png)

#### uData框架的目录结构
如下图所示，uData框架在framework和device下面有两部分的软件框架，分别是uData和sensor。
![](https://i.imgur.com/Kw1MSCg.png)

#### uData框架移植集成步骤
##### 1. uData框架之kernel层sensor移植
（1）首先将sensor文件包全部添加到device目录下，可参考上面的文件目录结构图。
（2）请在当前硬件平台芯片的mk中加入sensor的编译信息。一般此mk文件位于platform\mcu\。比如当前的硬件平台是基于STM L4系列芯片的，你可以按照如下图片实例，进行配置sensor信息。

![](https://i.imgur.com/Bf6J54T.png)

    $(NAME)_COMPONENTS += device/sensor /* 把此信息加入到当前的平台mk文件里，请注意大小写 */

（3）请把sensor\_init()添加到aos\_init.c的aos\_kernel\_init(kinit_t *kinit)中。这里需要注意的是，请添加在kernel组件初始化结束之后，比如ota\_service\_init()之后。

```
#ifdef AOS_SENSOR /* 此宏定义在sensor.mk文件中 */
    sensor_init();
#endif
```

##### 2. uData框架之Framework层uData移植
（1）首先将uData文件包全部添加到Famework目录下，可参考上面的文件目录结构图。
（2）请uData\_main()添加到在当前平台的framework的aos\_framework_init()中。

```
#ifdef AOS_UDATA /* 此宏定义在uData.mk文件中定义 */
    uData_main();
#endif
```
（3）请把uData组件添加到所开发的app的mk文件里，确保uData组件可以被编译使用到，可以参考example\uDataapp实例。例如，现在需要开发基于uData的app，可以参考如下：
```
$(NAME)_COMPONENTS := uData /* 请在app的mk文件里加入此模块信息 */
```
#### uData编译方式
aos make ###app@###,前者是输入所开发的app名字，后者是当前的平台信息。比如:
aos make uDataapp@b_l475e,表示在b_l475e平台下编译uDataapp。
编译成功之后，可以在输出的信息中sensor，uData和所开发的组件名字，如下图所示。
![](https://i.imgur.com/YfkUzsI.png)


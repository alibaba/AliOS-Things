EN| [中文](AliOS-Things-uData-Framework-Porting-Guide.zh)

#### Introduction of uData framework

uData is designed based on traditional sensorhub. It is an IoT oriented sensoring device processing framework integrating both characteristics of actual IoT application scenarios and AliOS Things itself. 

There are two layers in uData Framework. One is kernel layer, mainly responsible for sensor driving and related static calibration, including axial calibration. The other is framework layer, mainly responsible for application service management, dynamic calibration management and external module interfaces. The specific design of uData will not be unfolded in this document, but the following diagram can be used as a summary reference. This article will mainly introduce the porting of uDate software framework in AliOS Things IoT OS.

![](https://i.imgur.com/iSaUEYR.png)

#### uData directory architecture

As shown in the following picture, uData has two software directories, uData and sensor. 
![](https://i.imgur.com/Kw1MSCg.png)

#### uData porting steps

##### 1. Porting of sensors in kernel layer

（1）put "sensor" directory under "device" directory. You can refer to the above picture.
（2）add compilation information of sensors in mk file of hardware chips. The mk file is generally stored inplatform\mcu\. Our current hardware platform is based on STM L4 chips, and you can set sensor information as the following picture.

![](https://i.imgur.com/Bf6J54T.png)

```
$(NAME)_COMPONENTS += device/sensor /* add this information to the mk file of current platform. Please pay attention to the case */
```

（3）Please add sensor\_init() to aos\_kernel\_init(kinit_t *kinit) of aos\_init.c. You should add it after the completion of kernel initiation, for example, after ota\_service\_init().

```
#ifdef AOS_SENSOR /* This macro is defined in sensor.mk */
    sensor_init();
#endif
```

##### 2. Porting of framework layer

（1）put "uData" directory under "Framework" directory. You can refer to the above picture.
（2）add "uData\_main()" to "aos\_framework_init()" in framework layer of current platform.

```
#ifdef AOS_UDATA /* This macro is defined in uData.mk */
    uData_main();
#endif
```

（3）add uData components to the mk file of app, and make sure that uData components can be used. You can refer to example\uDataapp. For apps developed based on uData, you can refer to :

```
$(NAME)_COMPONENTS := uData /* Please add module information to the mk file of that app */
```

#### Compilation measures of uData

aos make ###app@###, the former one is the name of app, and the latter one is the information of current platform. For example : 
aos make uDataapp@b_l475e represents compilation of uDataapp under b_l475e platform.
When compilation is completed, you can find the name of snesor, uData and the app you develop in the output information :
![](https://i.imgur.com/YfkUzsI.png)


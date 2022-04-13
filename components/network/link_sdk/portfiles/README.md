`Portfiles`的概念是一组接口, 这些接口描述SDK如何使用当前设备的底层软硬件资源, 这些资源包括

+ 互斥锁
+ 时钟
+ 内存
+ 随机数生成器

---
当SDK被移植到运行不同OS的嵌入式设备上时, 需要变动的代码都只有`portfiles`目录下的C文件, 以下我们提供了一些移植的实例文档

+ [乐鑫ESP8266移植](http://code.aliyun.com/linksdk/docs/wikis/best-practice/ESP8266_Porting)
+ [乐鑫ESP32移植](http://code.aliyun.com/linksdk/docs/wikis/best-practice/ESP32_Porting)
+ [MCU+TCP模组(SIM800C)移植](http://code.aliyun.com/linksdk/docs/wikis/best-practice/SIM800C_TCP_Porting)
+ [MCU+MQTT模组(N720V5)移植](http://code.aliyun.com/linksdk/docs/wikis/best-practice/N720V5_MQTT_Porting)


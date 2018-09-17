阿里云IoT嵌入式 C Link Kit SDK 说明

1. 用途

阿里云IoT嵌入式C Link Kit SDK提供给使用C语言开发产品的设备厂商, 设备商可以使用该SDK将产品接入到阿里云IoT平台, 从而通过阿里云IoT平台对设备进行远程管理.

设备商可以使用该SDK开发诸如灯/插座之类的智能单品, 也可以使用该SDK开发具有子设备接入能力的网关, 比如Zigbee网关.

2. 功能列表

2.l 云端连接模块

    设备可使用MQTT连接阿里云IoT平台
    设备可使用CoAP连接阿里云IoT平台
    设备可使用HTTP/S连接阿里云IoT平台
    设备可使用HTTP2连接阿里云IoT平台
    MQTT连接服务器站点可配置
    云端服务器站点动态选择
    上报SDK版本号支持
    iTLS支持
    智能单品一型一密/动态注册四元组

2.2 设备物模型

    设备属性/事件/服务支持
    TLS静态集成/动态下拉支持

2.3 服务提供模块

    设备OTA
    WiFi配网支持
    WiFi/以太网设备绑定token生成以及通告
    设备reset支持

2.4 子设备管理框架

    添加/删除子设备
    禁用/解禁子设备
    子设备物模型支持, 包括子设备属性/服务/事件的代理

2.5 设备本地控制

    设备本地上线通告
    设备本地属性/事件数据订阅
    设备本地控制命令接收与处理
    子设备本地控制

3. 文档说明

    https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/Linkkit_User_Manual



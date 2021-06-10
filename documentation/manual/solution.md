@page solution 案例开发

[更正文档](https://gitee.com/alios-things/documentation/edit/rel_3.3.0/manual/solution.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

**概述**

AliOS Things Solution是指一系列典型应用场景下的示例程序。Solutions既能快速满足场景基本需求，同时也为开发者提供软件设计思路。

其位于工程根路径下的solutions目录中：

`solutions/`

`├── audio_demo`

`├── auto_demo`

`├── eduk1_demo`

`├── flower_demo`

`├── helloworld_demo`

`├── link_sdk_demo`

`├── link_sdk_gateway_demo`

`├── lora_p2p_demo`

`├── ota_demo`

`├── py_engine_demo`

`└── rfid_demo`

**版权信息**

所有Solution程序都遵循Apache license v2.0许可

**详情**

* helloworld\_demo : 该示例在终端循环打印“Hello World!”字符串。其提供了应用程序运行的必要软件配置，如果您不清楚如何开始AliOS Things的开发，建议您从此示例入手。
* link\_sdk\_demo : 演示AliOS Things应用程序如何快速连接到阿里云IoT平台
* link\_sdk\_gateway\_demo : AliOS Things的网关示例程序
* rfid\_demo : 演示如何基于HaaS100搭建RFID读卡器，读取卡片数据并上传到阿里云IoT平台
* flower\_demo : 演示如何使用HaaS100检测植物周围环境温湿度，并且同步到物联网平台以及钉钉小程序。
* ota\_demo : 演示如何对HaaS100进行远程升级
* auto\_demo : 演示如何基于HaaS100快速开发电动小车，并通过钉钉小程序对其进行控制。
* audio\_demo : HaaS100的音频示例程序，支持本地、云端以及钉钉小程序联动播放。
* py\_engine\_demo : 演示AliOS Things的python rpel模式，用户能够在终端上通过python命令对HaaS100进行实时控制。
* eduk1\_demo : HaaS EDU平台的应用模版，含陀螺仪、红外、海拔等多款典型场景下的传感器应用。如果您不清楚如何开始HaaS EDU平台的开发,建议您从此模版入手。


@subpage audio_demo \n
@subpage auto_demo \n
@subpage eduk1_demo \n
@subpage flower_demo \n
@subpage haas_dev_demo \n
@subpage helloworld_demo \n
@subpage linksdk_demo \n
@subpage linksdk_gateway_demo \n
@subpage lora_p2p_demo \n
@subpage miniapp_agent_demo \n
@subpage ota_demo \n
@subpage py_engine_demo \n
@subpage relay_demo \n
@subpage rfid_demo \n
@subpage ucloud_ai_demo \n

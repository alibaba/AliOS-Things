@page lora_p2p_demo HaaS100实现LoRa点对点通信

[更正文档](https://gitee.com/alios-things/lora_p2p_demo/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

案例简介
====

本方案选取SX1268作为LoRa通信IC, HaaS100以外挂SX1268 SPI模组的方式，实现LoRa数据的收发。

<img src="https://img.alicdn.com/imgextra/i3/O1CN01584e971o2zXPL0DOc_!!6000000005168-0-tps-1288-132.jpg" style="max-width:800px;" />


基础知识
====

LoRa是Semtech公司采用和推广的一种基于扩频技术的超远距离无线传输技术。相较于传统通信技术，LoRa具备低门槛、远距离、长续航的优势，其在数字农业、智慧社区等领域具有很高的实用价值。更多LoRa介绍请参阅：[www.semtech.com/lora](http://www.semtech.com/lora)

物品清单和Checklist
==============

* 两块HaaS100开发板：用户数据收发的主机。
* 两块SX1268的模组：用于收发LoRa数据。
* 若干杜邦线：用于连接HaaS100和SX1268模组

案例实现
====

* 硬件连接

<img src="https://img.alicdn.com/imgextra/i1/O1CN01FEwNgJ1QjXYO3Rm5I_!!6000000002012-0-tps-1266-854.jpg" style="max-width:800px;" />

* 准备两块HaaS100开发板。一块作通信发送方，另一块作通信接收方。
* 准备两块SX1268模组，分别连接到两块HaaS100上。连接线序如下图所示：

<img src="https://img.alicdn.com/imgextra/i1/O1CN01JPy7ir21uex7305ZI_!!6000000007045-0-tps-438-181.jpg" style="max-width:800px;" />

* 软件实现 
  * 软件主要参考Semtech官方驱动，将其移植到AliOS Things, 并做了HaaS100平台相应的适配。
  * 本案例同时包含发送和接收的逻辑，需要按照通信角色修改demo.c 第18行：

<img src="https://img.alicdn.com/imgextra/i2/O1CN013dKGL41fQVKMaOY5A_!!6000000004001-2-tps-756-216.png" style="max-width:800px;" />

<img src="https://img.alicdn.com/imgextra/i4/O1CN01CXUyle1i6IhhbpjZ3_!!6000000004363-2-tps-728-210.png" style="max-width:800px;" />

效果图
===
<img src="https://img.alicdn.com/imgextra/i2/O1CN015ogTbg1e59FJ8nOSy_!!6000000003819-2-tps-2110-618.png" style="max-width:800px;" />

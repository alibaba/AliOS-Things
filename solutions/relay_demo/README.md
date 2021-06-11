@page relay_demo HaaS100实现继电器控制示例

[更正文档](https://gitee.com/alios-things/relay_demo/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

案例简介
====

本方案实现通过HaaS100开发板控制继电器输出状态的功能。

基础知识
====

继电器是一种电控制器件，是当输入量（激励量）的变化达到规定要求时，在电气输出电路中使被控量发生预定的阶跃变化的一种电器。 它具有控制系统（又称输入回路）和被控制系统（又称输出回路）之间的互动关系。 通常应用于自动化的控制电路中，它实际上是用小电流去控制大电流运作的一种“自动开关”。故在电路中起着自动调节、安全保护、转换电路等作用。

物品清单和Checklist
==============

* 一块HaaS100开发板：作为控制继电器的上位机。
* 一个SRD-05VDC-SL-C继电器模块：TTL电平控制的继电器。
* 若干杜邦线：用于连接HaaS100和SRD-05VDC-SL-C。
* 一个BSM-0404RB继电器模块：RS485 modbus协议控制的继电器。
* RS485连接线：用于连接HaaS100和BSM-0404RB。

案例实现
====

* 硬件连接

<img src="https://img.alicdn.com/imgextra/i2/O1CN01DLCjIX1PNj5O6BwDJ_!!6000000001829-0-tps-4096-3072.jpg" style="max-width:800px;" />

* SRD-05VDC-SL-C继电器的CH1、GND、VCC引脚分别与HaaS100的GPIO4_6、GND、VCC3.3连接。
* BSM-0404RB继电器的RS485接口与HaaS100的RS485接口连接。

* 软件实现
  * 本案例包含通过GPIO控制SRD-05VDC-SL-C继电器的逻辑。
  * 本案例包含通过RS485 modbus协议控制BSM-0404RB继电器的逻辑。

效果图
===

<img src="https://img.alicdn.com/imgextra/i4/O1CN01bt0ahl1KLpMjvE5Y8_!!6000000001148-0-tps-1920-1080.jpg" style="max-width:800px;" />

<img src="https://img.alicdn.com/imgextra/i1/O1CN010Ddey81CiiEPwG0vl_!!6000000000115-0-tps-1920-1080.jpg" style="max-width:800px;" />

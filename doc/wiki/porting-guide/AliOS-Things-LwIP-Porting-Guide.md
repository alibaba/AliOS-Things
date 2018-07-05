EN| [中文](AliOS-Things-LwIP-Porting-Guide.zh) 

# Content

  * [1 Porting of NIC driver](#1网卡驱动程序)
  * [2 Porting related to platform](#2平台相关)
  * [3 Porting related to LwIP](#3协议栈配置)
  * [4 Porting with OS](#4与OS的对接)
  * [5 Modification of compilation script](#5编译脚本修改)

------

AliOS Things provides LwIP protocol stack, and developers can complete the porting work according to the following steps.

## 1 Porting of NIC driver  
Porting code of NIC driver can refer to [Code example](https://github.com/alibaba/AliOS-Things/blob/master/kernel/protocols/net/netif/ethernetif.c). 
It mainly concerns the following function modification :

```c
  static void low_level_init(struct netif *netif);
  static err_t low_level_output(struct netif *netif, struct pbuf *p);
  static struct pbuf *low_level_input(struct netif *netif);
```

When the modification is completed, source code needs to be stored in its corresponding platform.

## 2 Porting related to platform   
Porting code related to platform can refer to [Code example](https://github.com/alibaba/AliOS-Things/blob/master/platform/mcu/moc108/include/lwip-2.0.2/port/arch/cc.h). 
The definition mainly includes type definition, size end setting and memory alignment.

If the reference implementation is consistent with your target implementation, you can copy and store it in corresponding platform.

## 3 Porting related to LwIP
Porting code related to LwIP can refer to [Code example](https://github.com/alibaba/AliOS-Things/blob/master/platform/mcu/moc108/include/lwip-2.0.2/port/lwipopts.h).

If the reference configuration is consistent with yours, you can copy and store it in corresponding platform.

## 4 Porting with OS  
Porting with OS has been completed in AliOS Things, and developers can use it directly. [Implement code](https://github.com/alibaba/AliOS-Things/blob/master/kernel/protocols/net/port/sys_arch.c).

## 5 Modification of compilation script
After completing the above modifications, you need to modify the related Makefile in corresponding platform. [Implement code](https://github.com/alibaba/AliOS-Things/blob/master/platform/mcu/moc108/moc108.mk)。
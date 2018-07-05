 [EN](AliOS-Things-WiFi-Porting-Guide) | 中文

# 目录

  * [1 WiFi模块结构体](#1WiFi模块结构体)
  * [2 WiFi事件回调函数](#2WiFi事件回调函数)
  * [3 WiFi接口的实现](#3WiFi接口的实现)
  * [4 注册模块](#4注册模块)
  * [5 接口的使用](#5接口的使用)
  * [6 WiFi移植的验证](#6WiFi移植的验证)
---

在AliOS Things移植的过程中，如果需要支持WiFi功能(例如有配网需求，参考AliOS Things中的netmgr模块)，则需要对WiFi HAL接口进行移植实现。AliOS Things WiFi HAL的接口定义请查看头文件：[wifi_hal.h](https://github.com/alibaba/AliOS-Things/blob/master/include/hal/wifi.h)。

本文将讲述AliOS Things中的WiFi移植要点，以及如何验证WiFi移植工作。

# 1WiFi模块结构体
首先，先了解一下AliOS Things中跟WiFi HAL相关的一个重要结构体 - `hal_wifi_module_t`。WiFi相关的操作和接口都封装在`hal_wifi_module_t`这个结构体中，相关定义在文件[wifi_hal.h](https://github.com/alibaba/AliOS-Things/blob/master/include/hal/wifi.h)中。
  ```c
  struct hal_wifi_module_s {
      hal_module_base_t    base;
      const hal_wifi_event_cb_t *ev_cb;

      int  (*init)(hal_wifi_module_t *m);
      void (*get_mac_addr)(hal_wifi_module_t *m, uint8_t *mac);
      int  (*start)(hal_wifi_module_t *m, hal_wifi_init_type_t *init_para);
      int  (*start_adv)(hal_wifi_module_t *m, hal_wifi_init_type_adv_t *init_para_adv);
      int  (*get_ip_stat)(hal_wifi_module_t *m, hal_wifi_ip_stat_t *out_net_para, hal_wifi_type_t wifi_type);
      int  (*get_link_stat)(hal_wifi_module_t *m, hal_wifi_link_stat_t *out_stat);
      void (*start_scan)(hal_wifi_module_t *m);
      void (*start_scan_adv)(hal_wifi_module_t *m);
      int  (*power_off)(hal_wifi_module_t *m);
      int  (*power_on)(hal_wifi_module_t *m);
      int  (*suspend)(hal_wifi_module_t *m);
      int  (*suspend_station)(hal_wifi_module_t *m);
      int  (*suspend_soft_ap)(hal_wifi_module_t *m);
      int  (*set_channel)(hal_wifi_module_t *m, int ch);
      void (*start_monitor)(hal_wifi_module_t *m);
      void (*stop_monitor)(hal_wifi_module_t *m);
      void (*register_monitor_cb)(hal_wifi_module_t *m, monitor_data_cb_t fn);
      void (*register_wlan_mgnt_monitor_cb)(hal_wifi_module_t *m, monitor_data_cb_t fn);
      int  (*wlan_send_80211_raw_frame)(hal_wifi_module_t *m, uint8_t *buf, int len);
  };
  ```
在平台移植过程中，如何对接这些接口，将在后续章节中叙述。

# 2WiFi事件回调函数
WiFi移植过程中，WiFi事件及回调函数也是很重要的一个内容。AliOS Things中WiFi事件的回调函数在netmgr模块中定义，请参照`framework/netmgr/`。在配网过程中，netmgr负责定义和注册WiFi回调函数；而在WiFi启动和运行的过程中，通过调用回调函数来通知上层应用，以执行相应的动作。这些WiFi事件的回调函数，应该在WiFi网络驱动（通常是HAL层实现或更底层的代码）的**任务上下文**中被触发。例如：
* 在WiFi底层拿到IP后，应执行`ip_got`回调，并将IP信息传递给上层；
* 在WiFi完成信道扫描后，应调用`scan_compeleted`或者`scan_adv_compeleted`回调，将扫描结果传递给上层；
* 在在WiFi状态改变时，应调用`stat_chg`回调。

需要再次强调的是，这些事件回调函数由netmgr配网模块定义并注册，在WiFi底层（如HAL）里面触发调用。

下面是AliOS Things中定义的WiFi事件回调函数和接口，相关定义在文件[wifi_hal.h](https://github.com/alibaba/AliOS-Things/blob/master/include/hal/wifi.h)中。
```c
typedef struct {
    void (*connect_fail)(hal_wifi_module_t *m, int err, void *arg);
    void (*ip_got)(hal_wifi_module_t *m, hal_wifi_ip_stat_t *pnet, void *arg);
    void (*stat_chg)(hal_wifi_module_t *m, hal_wifi_event_t stat, void *arg);
    void (*scan_compeleted)(hal_wifi_module_t *m, hal_wifi_scan_result_t *result,
                            void *arg);
    void (*scan_adv_compeleted)(hal_wifi_module_t *m,
                                hal_wifi_scan_result_adv_t *result, void *arg);
    void (*para_chg)(hal_wifi_module_t *m, hal_wifi_ap_info_adv_t *ap_info,
                     char *key, int key_len, void *arg);
    void (*fatal_err)(hal_wifi_module_t *m, void *arg);
} hal_wifi_event_cb_t;
```
回调函数的定义，请参照netmgr中的`g_wifi_hal_event `。

# 3WiFi接口的实现
在具体的平台移植过程中，用户需要分别实现WiFi模块结构体中对应的接口函数。AliOS Things对WiFi HAL层接口有一层封装，参见`kernel/hal/wifi.c`文件。具体的接口实现，一般在`platform/mcu/xxx/hal/wifi_port.c`中。参考实现：`platform/mcu/esp32/hal/wifi_port.c`。关于WiFi HAL接口的说明，可以参照：[WiFi HAL](AliOS-Things-API-HAL-WiFi-Guide)。
下面对每个接口作一些说明：
## `init`
该接口需要对wifi进行初始化，使wifi达到可以准备进行连接工作的状态，如分配wifi资源、初始化硬件模块等操作。
## `get_mac_addr`
通过该接口可以获取到WiFi的物理硬件地址。注意：**回传的mac地址格式为6个字节二进制值（不含`:`号）**，如
`uint8_t mac[6] = {0xd8,0x96,0xe0,0x03,0x04,0x01};`。
## `set_mac_addr`
通过该接口可以设置WiFi的物理硬件地址。
## `start`
通过该接口可以启动WiFi，根据启动参数不同来区分启动station模式还是AP模式，如station模式下进行连接AP的操作、AP模式下根据参数启动AP功能。在station模式下，该函数触发AP连接操作后即返回。后续底层处理过程中，拿到IP信息后，需要调用`ip_got`回调函数来通知上层获取IP事件。注意：(1) **station模式下启动WiFi连接时，传入的SSID长度不超过32位**；(2) **在连接AP后，WiFi底层需要维护自动重连操作**。
## `start_adv`
该接口类似`start `，但启动的参数更丰富。可选。
## `get_ip_stat`
通过该接口可以获取WiFi工作状态下的IP信息，如IP、网关、子网掩码、MAC地址等信息。
## `get_link_stat`
通过该接口可以获取WiFi工作状态下的链路层信息，如连接信号强度、信道、SSID等信息。
## `start_scan`
该接口启动station模式下的信道扫描。扫描结束后，调用`scan_compeleted`回调函数，将各个信道上扫描到的AP信息通知给上层。需要得到的扫描信息在`hal_wifi_scan_result_t`中定义。注意：**扫描结果存储所需要的内存在底层实现中分配，回调函数返回后再将该内存释放**。
## `start_scan_adv`
该接口与`hal_wifi_start_scan `类似，但扫描的信息更多，如bssid、channel信息等，需要扫描得到的信息在`hal_wifi_scan_result_adv_t`中定义。扫描结束后，通过调用`scan_adv_compeleted`回调函数通知上层。注意：**扫描结果存储所需要的内存在底层实现中分配，回调函数返回后再将该内存释放**。
## `power_off`
该接口对WiFi硬件进行断电操作。
## `power_on`
该接口对WiFi硬件进行上电操作。
## `suspend`
该接口断开WiFi所有连接，同时支持station模式和soft ap模式。
## `suspend_station`
该接口断开WiFi所有连接，支持station模式。
## `suspend_soft_ap`
该接口断开WiFi所有连接，支持soft ap模式。
## `set_channel`
通过该接口可以设置信道。
## `wifi_monitor`
该接口启动监听模式，并且在收到任何数据帧（包括beacon、probe request等）时，调用monitor_cb回调函数进行处理。注意，回调函数是上层通过`register_monitor_cb`进行注册的。注意：**监听模式下，上层cb函数期望处理的包不带FCS域，所以底层的数据包如果带FCS应当先剥离再往上层传递**。
## `stop_wifi_monitor`
该接口关闭侦听模式。
## `register_monitor_cb`
该接口注册侦听模式回调函数，回调函数在底层接收到任何数据帧时被调用。
## `register_wlan_mgnt_monitor_cb`
该接口注册管理帧回调函数（非监听模式下），该回调函数在底层接收到管理帧时被调用。
## `start_debug_mode`
该接口进入调试模式，可选（若模块支持）。
## `stop_debug_mode`
该接口退出调试模式，可选。
## `wlan_send_80211_raw_frame`
该接口可以用于发送802.11格式的数据帧。

参考ESP32平台实现：`platform/mcu/esp32/hal/wifi_port.c`。

# 4初始化WiFi和注册模块
在完成WiFi接口和事件回调的实现后，定义一个`hal_wifi_module_t`的结构体，将各个接口和回调的实现地址赋值给结构体中对应的域。如下：
  ```c
  hal_wifi_module_t sim_aos_wifi_vendor = {
      .base.name           = "alios_wifi_vender_name",
      .init                =  wifi_init,
      .get_mac_addr        =  wifi_get_mac_addr,
      .start               =  wifi_start,
      .start_adv           =  wifi_start_adv,
      .get_ip_stat         =  get_ip_stat,
      .get_link_stat       =  get_link_stat,
      .start_scan          =  start_scan,
      .start_scan_adv      =  start_scan_adv,
      .power_off           =  power_off,
      .power_on            =  power_on,
      .suspend             =  suspend,
      .suspend_station     =  suspend_station,
      .suspend_soft_ap     =  suspend_soft_ap,
      .set_channel         =  set_channel,
      .start_monitor       =  start_monitor,
      .stop_monitor        =  stop_monitor,
      .register_monitor_cb =  register_monitor_cb,
      .register_wlan_mgnt_monitor_cb = register_wlan_mgnt_monitor_cb,
      .wlan_send_80211_raw_frame = wlan_send_80211_raw_frame,
  };
  ```
一般在板级初始化的过程中，先通过`hal_wifi_register_module`接口对WiFi模块进行注册，然后调用`hal_wifi_init`接口对WiFi硬件模块进行初始化。至此，WiFi HAL模块就完成了初始化，可以使用。
  ```c
  void hal_wifi_register_module(hal_wifi_module_t *m);
  int hal_wifi_init(void);
  ```

参考实现：`platform/mcu/esp32/bsp/entry.c`。

# 5接口的使用
需要使用WiFi功能和接口时，可以通过调用下面的函数来获取默认的WiFi模块结构体（第一个被注册的模块）。一般系统中只注册一个WiFi模块，在使用WiFi HAL接口时，通过`hal_wifi_module_t *m`指定所使用的WiFi模块，若为`NULL`，则使用默认的WiFi模块（使用如下接口）。
  ```c
  hal_wifi_module_t *hal_wifi_get_default_module(void);
  ```

# 6WiFi移植的验证
在完成WiFi的移植后，可以通过配网APP来验证移植工作的有效性。配网APP位于`example/netmgrapp/`目录下，验证步骤如下：
* 编译和运行netmgrapp。
* 在cli输入`netmgr connect <AP> <password>`命令。其中`AP`为测试环境下可用的WiFi热点的SSID（**SSID的长度最多不超过32位**），`password`是对应AP的密码。
* 预期结果：设备能成功拿到IP地址。

此外，通过AliOS Things中的Alink测试APP，可以更充分的验证WiFi功能。关于Alink测试和认证，请参考[Alink Smart Config Test and Certification](WiFi-Provisioning-and-Alink-Certification)
EN| [中文](AliOS-Things-WiFi-Porting-Guide.zh) 

# Content

- [1 WiFi module struct](#1WiFi模块结构体)
- [2 WiFi event callback function](#2WiFi事件回调函数)
- [3 implemnet of WiFi interface](#3WiFi接口的实现)
- [4 registration of modules](#4注册模块)
- [5 use of interface](#5接口的使用)
- [6 test of WiFi porting](#6WiFi移植的验证)

------

If support for WiFi function is needed when migrating AliOS Things (for example, needs for network provision. You can refer to netmgr module in AliOS Things), you need to implement the porting of WiFi HAL interfaces. Definition of AliOS Things WiFi HAL interfaces can refer to the header file: [wifi_hal.h](https://github.com/alibaba/AliOS-Things/blob/master/include/hal/wifi.h).

This article will describe the key points in WiFi porting and how to verify it in AliOS Things.

# 1 WiFi module struct

`hal_wifi_module_t` is an important struct in AliOS Things concerning WiFi HAL. 

Operations and interfaces related to WiFi are all encapsulated in this struct. Related definition is stored in  [wifi_hal.h](https://github.com/alibaba/AliOS-Things/blob/master/include/hal/wifi.h).

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

How to connect to these interfaces will be described in later parts.

# 2 WiFi event callback function

WiFi event callback functions in AliOS Things is defined in netmgr modeule, and you can refer to `framework/netmgr/`. In the process of network provision, netmgr defines and registers WiFi event callback functions. In the process of WiFi startup and operation, it calls the upper level application to invoke the corresponding action by calling callback function. The callback functions of these WiFi events should be triggered in the task context of the WiFi network driver. For example:

- When WiFi gets IP,  it will execute ip_got callback, and deliver the IP information to the upper level.

- When WiFi completes channel scan, it will call `scan_compeleted` or `scan_adv_compeleted` to transfer the scan results to the upper layer.

- When WiFi state changes, `stat_chg` will be invoked.



These event callback functions are defined and registered by netmgr network provision module, and calls are triggered in the bottom layer of WiFi (such as HAL).

The following are WiFi event callback functions and interfaces defined in AliOS Things, whose related definition file is store in wifi_hal.h.

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

The definition of callback function can refer to `g_wifi_hal_event ` in netmgr.

# Implement of 3WiFi interface 

When migrating a specific platform, users need to implement the corresponding interface functions in WiFi module struct. The encapsulation for WiFi HAL interface can refer to`kernel/hal/wifi.c`. Specific interface implementation is generally in `platform/mcu/xxx/hal/wifi_port.c`. Reference implementation: `platform/mcu/esp32/hal/wifi_port.c`. The instruction of WiFi HAL interface can refer to: [WiFi HAL](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-API-HAL-WiFi-Guide).

Here are instructions for some interfaces:

## `init`

It can initiate WiFi, and enable WiFi to be prepared for connection work, such as WiFi provision and initialization of hardware modules.

## `get_mac_addr`

You can get address of WiFi hardwares through this interface. Remember that the return MAC address is a 6-byte binary value ( without `:`), such as `uint8_t mac[6] = {0xd8,0x96,0xe0,0x03,0x04,0x01};`.

## `set_mac_addr`

You can set the address of WiFi hardwares through this interface. 

## `start`

You can start WiFi through this interface. It can distinguish station mode from AP mode according to different startup parameter. Under station mode, the function will immediately return after triggering AP connection operation. In subsequent process, after getting IP information, it will call `ip_got` to notify the upper level of IP event. Note: (1) If the WiFi connection is started in station mode, the length of the incoming SSID should be no more than 32 bits. (2) When AP connection is completed, WiFi bottom layer needs to maintain automatic reconnection operation.

## `start_adv`

This interface is similar to `start `, but has more abundant startup parameters. (Optional)

## `get_ip_stat`

You can get the IP information under WiFi working state, such as IP, gateway, subnet mask and MAC address.

## `get_link_stat`

You can get link layer information under WiFi working state through this interface, such as connection signal intensity, channel and SSID.

## `start_scan`

This interface starts the channel scan under station mode. When scan is completed, it will call `scan_compeleted`  to transfer the scanned AP information to the upper layer. The required scan information is defined in `hal_wifi_scan_result_t`. Note: The memory required for the storage of scan results is allocated in the bottom layer, and it will be released when callback function is returned.

## `start_scan_adv`

This interface is similar to `hal_wifi_start_scan `, but has more scanned information, such as bssid and channel. The required scan information is defined in`hal_wifi_scan_result_adv_t`. When scan is completed, it will call `scan_adv_compeleted`  to inform the upper layer. Note: The memory required for the storage of scan results is allocated in the bottom layer, and it will be released when callback function is returned.

## `power_off`

The interface can cut off the power of WiFi hardwares.

## `power_on`

This interface can give power to WiFi hardwares. 

## `suspend`

The interface can cut off all the WiFi connections and can support both station mode and soft ap mode.

## `suspend_station`

The interface can cut off all the WiFi connections and can support station mode.

## `suspend_soft_ap`

The interface can cut off all the WiFi connections and can support soft ap mode.

## `set_channel`

You can set the information channel through this interface.

## `wifi_monitor`

The interface can start monitor mode and call monitor_cb to handle the received data frames (including beacon and probe request). The callback function is registered in the upper level through `register_monitor_cb`. Note: under the monitor mode, the packet that handles cb function expect in the upper layer does not have FCS domain, so you should strip the FCS before passing it to the upper layer.

## `stop_wifi_monitor`

This interface can close the monitor mode.

## `register_monitor_cb`

This interface can register callback functions under monitor mode. This callback function will be called whenever a data frame is received  in the bottom layer. 

## `register_wlan_mgnt_monitor_cb`

This interface can register callback functions ( not in monitor mode). This callback function will be called when a management frame is received  in the bottom layer. 

## `start_debug_mode`

The interface can start debug mode. Optional (if the module is supported).

## `stop_debug_mode`

The interface can close debug mode. Optional.

## `wlan_send_80211_raw_frame`

This interface can send data frame in the format of 802.11.

Refer to implement in ESP32 platform :`platform/mcu/esp32/hal/wifi_port.c`。

# 4 WiFi initiation and registration of modules

When WiFi interface and event callback are completed, define a `hal_wifi_module_t` struct, and assign the address of each interface and callback to its corresponding domain in the struct. As follows:

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

Generally, in the process of board level initialization, you need to first register the WiFi module through`hal_wifi_register_module`, and then call `hal_wifi_init` to initialize WiFi hardware module. 

```c
  void hal_wifi_register_module(hal_wifi_module_t *m);
  int hal_wifi_init(void);
```

Implement example：`platform/mcu/esp32/bsp/entry.c`。

# 5 Use of interface

When WiFi function and interface is needed, you can get the default WiFi module struct by calling the following function. Generally speaking, a system only registers one WiFi module. When using WiFi HAL interface, you can specify the WiFi module by `hal_wifi_module_t *m`. For `NULL` , the default WiFi module is used (use the following interface).

```c
  hal_wifi_module_t *hal_wifi_get_default_module(void);
```

# 6 Test of WiFi porting 

When WiFi porting is completed, you can verify the effectiveness of the it through network provision APP, which is located in`example/netmgrapp/`. Steps of validation are as follows:

- Compile and run netmgrapp.

- Input `netmgr connect <AP> <password>` in cli. `AP` is the SSID of WiFi hot spot available in the test environment (the maximum length of the SSID is 32 bits), and `password` is the password for AP.

- Expected results: the device can get an IP address.


WiFi function can be further verified through Alink in AliOS Things. For Alink test and authentication, please refer to [Alink Smart Config Test and Certification](https://github.com/alibaba/AliOS-Things/wiki/WiFi-Provisioning-and-Alink-Certification)
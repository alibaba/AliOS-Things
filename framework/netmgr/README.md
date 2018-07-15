## netmgr

### Content

- [netmgr](#netmgr)
    - [Content](#content)
    - [Overview](#overview)
        - [Dependent components](#dependent-components)
    - [Requirements](#requirements)
    - [API](#api)
    - [How to use](#how-to-use)
    - [Run](#run)

### Overview

`netmgr` is an component that can easy connect to wifi using cli command. `netmgr` support this commands:
**start network connecting**(SSID and password has stored in [kv](https://yq.aliyun.com/articles/291229)):

```sh
# netmgr start
```

**clear network information:**

```sh
# netmgr clear
```

**connect network with SSID and password**(SSID and password will store in [kv](https://yq.aliyun.com/articles/291229)):

```sh
# netmgr connect <SSID> <passwd>
```

#### Dependent components

`netmgr` dependents on [kv](https://yq.aliyun.com/articles/291229), and [yloop](https://github.com/alibaba/AliOS-Things/wiki/Yloop-Event-Framework).

### Requirements

`netmgr` needs serial to type commands, and needs wifi interface to connect AP, check [wifi porting guide](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-WiFi-Porting-Guide) for more information.

### API

**netmgr_init()**:
<table>
<tr>
<td>prototype</td>
<td>int netmgr_init(void)</td>
</tr>
<tr>
<td>description</td>
<td>netmgr initialization, netmgr_init() must been called before netmgr_start()</td>
</tr>
</table>
    

**netmgr_start()**:
<table>
<tr>
<td>prototype</td>
<td>int netmgr_start(bool autoconfig)</td>
</tr>
<tr>
<td>description</td>
<td>start netmgr service</td>
</tr>
</table>

**netmgr_deinit()**:
<table>
<tr>
<td>prototype</td>
<td>void netmgr_deinit(void)</td>
</tr>
<tr>
<td>description</td>
<td>deinit netmgr</td>
</tr>
</table>

### How to use

usually, `netmgr` started in `application_start()`, like this:

```c
int application_start(int argc, char *argv[])
{
    netmgr_init();
    netmgr_start(false);
    aos_loop_run();
    return 0;
}
```

To learn `netmgr` how to work, check netmgr example: [netmgrapp](../../example/networkapp/README.md).

### Run

when [netmgrapp](../../example/networkapp/README.md) run, type `help` will display netmgr commads:

```sh
......
netmgr: netmgr [start|clear|connect ssid password]
......
```

type `netmgr connect <ssid> <passwd>` will display like this:

```
# netmgr connect YouCanYouUp xiaowen6814
sta_ip_down
Cancelling scan request
SM_DISCONNECT_IND
wpa_dInit
wpa_supplicant_req_scan
Setting scan request: 0.100000 sec
MANUAL_SCAN_REQ

# wpa_supplicant_scan
wpa_drv_scan
wpa_send_scan_req
wpa_driver_scan_cb
wpa_get_scan_rst:1
wpa_supplicant_connect
Cancelling scan request
wpa_driver_associate
get txpwrtab gain:9
rate:0, pwr_gain:22
add extral movement in test
---------SM_CONNECT_IND_ok
wpa_driver_assoc_cb
Cancelling scan request
hapd_intf_add_key CCMP
add sta_mgmt_get_sta
add TKIP
add is_broadcast_ether_addr
ME_SET_CONTROL_PORT_REQ
sta_ip_start
[091950]<I> Got ip : 192.168.31.67, gw : 192.168.31.1, mask : 255.255.255.0
```
# network - 网络配置模块
* 模块功能：  
管理Wi-Fi和以太网

* 参考用法：  

```python
import network
import time

def callback(event_id):
    print(event_id)

nic = network.WLAN(network.STA_IF)
nic.on(callback)
nic.active(True)

if not nic.isconnected():
    nic.connect(ssid, password)
    print("Waiting for connection...")
    while not nic.isconnected():
        time.sleep(1)
print(nic.ifconfig())

```

## 常量说明
|常量|值|说明|
|:-|:-|:-|
|network.STA_IF|1|WLAN STA模式|
|network.AP_IF|2|WLAN AP模式|
|network.STA_STARTED|1|WLAN已启动|
|network.STA_GOT_IP|2|WLAN获得IP|
|network.STA_LOST_IP|3|WLAN已丢失IP|
|network.STA_DISCONNECTED|4|WLAN已断开连接|
|network.STA_CONNECTED|5|WLAN已链接|
|network.STA_STOPED|6|WLAN已停用|
|network.STA_UNKNOWN|7|未知状态|


## 错误码

```C
typedef enum {
    AOS_ERR_WIFI_BASE = 0x3000,         /*!< WiFi ERR NUM BASE */
    AOS_ERR_WIFI_NOT_INIT,              /*!< WiFi driver was not installed by esp_wifi_init */
    AOS_ERR_WIFI_NOT_STARTED,           /*!< WiFi driver was not started by esp_wifi_start */
    AOS_ERR_WIFI_NOT_STOPPED,           /*!< WiFi driver was not stopped by esp_wifi_stop */
    AOS_ERR_WIFI_IF,                    /*!< WiFi interface error */
    AOS_ERR_WIFI_MODE,                  /*!< WiFi mode error */
    AOS_ERR_WIFI_STATE,                 /*!< WiFi internal state error */
    AOS_ERR_WIFI_CONN,                  /*!< WiFi internal control block of station or soft-AP error */
    AOS_ERR_WIFI_NVS,                   /*!< WiFi internal NVS module error */
    AOS_ERR_WIFI_MAC,                   /*!< MAC address is invalid */
    AOS_ERR_WIFI_SSID,                  /*!< SSID is invalid */
    AOS_ERR_WIFI_PASSWORD,              /*!< Password is invalid */
    AOS_ERR_WIFI_TIMEOUT,               /*!< Timeout error */
    AOS_ERR_WIFI_WAKE_FAIL,             /*!< WiFi is in sleep state(RF closed) and wakeup fail */
    AOS_ERR_WIFI_WOULD_BLOCK,           /*!< The caller would block */
    AOS_ERR_WIFI_NOT_CONNECT,           /*!< Station still in disconnect status */
    AOS_ERR_WIFI_POST,                  /*!< Failed to post the event to WiFi task */
    AOS_ERR_WIFI_INIT_STATE,            /*!< Invalod WiFi state when init/deinit is called */
    AOS_ERR_WIFI_STOP_STATE,            /*!< Returned when WiFi is stopping */
    AOS_ERR_WIFI_NOT_ASSOC,             /*!< The WiFi connection is not associated */
    AOS_ERR_WIFI_TX_DISALLOW,           /*!< The WiFi TX is disallowed */
    AOS_ERR_TCPIP_ADAPTER_INVALID_PARAMS,
    AOS_ERR_TCPIP_ADAPTER_IF_NOT_READY,
    AOS_ERR_TCPIP_ADAPTER_DHCPC_START_FAILED,
    AOS_ERR_TCPIP_ADAPTER_NO_MEM
} AOS_NETWORK_ERR_E;
```

## network.WLAN - 创建一个WLAN实例

* 函数原型：
> class network.WLAN(interface_id)

* 参数说明：

interface_id 可选值为 network.STA_IF 或 network.AP_IF

* 返回值：  
返回一个wlan实例

* 参考代码：
```python
wlan = network.WLAN(network.STA_IF)
```

## WLAN.active

* 函数原型：
> WLAN.active(is_active)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|is_active|bool（True, False）|是|激活/反激活|

## WLAN.connect

* 函数功能：  
链接Wi-FI

* 函数原型：
> WLAN.connect(ssid, pwd)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|ssid|string|是|ssid名称|
|pwd|string|是|ssid密码|

## WLAN.disconnect

* 函数功能：  
断开Wi-Fi链接

* 函数原型：
> WLAN.disconnect()

* 参数说明：无

* 返回值：

成功返回0，错误返回错误码

## WLAN.status

* 函数功能：  
获取Wi-Fi状态

* 函数原型：
> WLAN.status()

* 参数说明：无

* 返回值

|定义| 数值 |说明|
|:-|:-|:-|
|network.STA_STARTED|1|WLAN已启动|
|network.STA_GOT_IP|2|WLAN获得IP|
|network.STA_LOST_IP|3|WLAN已丢失IP|
|network.STA_DISCONNECTED|4|WLAN已断开连接|
|network.STA_CONNECTED|5|WLAN已链接|
|network.STA_STOPED|6|WLAN已停用|
|network.STA_UNKNOWN|7|未知状态|

## WLAN.isconnected

* 函数功能：  
查看Wi-Fi是否已链接

* 函数原型：
> WLAN.isconnected()

* 参数说明：无

* 返回值

|类型| 数值 |说明|
|:-|:-|:-|
|bool|True|Wi-Fi已连接|
|bool|False|Wi-Fi未连接|


## WLAN.ifconfig

* 函数功能：  
查看Wi-Fi是否已链接

* 函数原型：
> WLAN.ifconfig([ip, subnet, gateway, dns])

* 参数说明：

如果不传参数，则获取当前IP配置, 如果传入组合参数，则设定IP信息

|参数|类型|必选参数|说明|
|:----|:----|:---:|----|
|ip_info|tuple|否|ip信息|

ip_info数据格式说明：

|参数|类型|说明|
|:----|:----|:----|
|ip|stringip|地址|
|subnet|string|子网掩码|
|gateway|string|网关地址|
|dns|string|dns地址|

* 返回值

成功返回0，错误返回错误码


## WLAN.on

* 函数功能：  
注册回调函数，网络状态发生变化时候，引擎会调用回调函数

* 函数原型：
> WLAN.on(callback)

* 参数说明：

|参数|类型|必选参数|说明|
|:----|:----|:---:|----|
|callback|func|是|设定回调函数|

* callback函数定义
callback(event_type)

* 返回值

成功返回0，错误返回错误码

## WLAN.scan

* 函数功能：  
扫描附近AP（无线路由器）

* 函数原型：
> WLAN.scan()

* 参数说明：

无

* 返回值

返回AP（无线路由器）列表

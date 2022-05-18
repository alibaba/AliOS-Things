# network - 网络管理模块

* 类功能：  
Network是网络连接管理模块，目前支持WIFI, Ethernet, Cellular三种网络模式的Connect, Disconnect，事件侦听，获取状态，网络配置等功能。

## openNetWorkClient - 创建network对象
* 函数功能：  
根据输入的网络类型参数，创建network client对象。

* 函数原型：
> var net = network.openNetWorkClient({devType: 'wifi'});

* 参数说明：  
Object类型，具体描述如下，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|options|Object|是|初始化网络类型，默认为WIFI类型。|

* 返回值：  
成功则返回network对象；失败返回None

## getType - 获取网络连接类型

* 函数功能：  
获取当前配置的网络连接类型

* 函数原型：
> var netType = network.getType();

* 参数说明：  
无

* 返回值：  
String类型，具体如下，

|返回值|类型|说明|
|-----|----|----|
|cellular|String|蜂窝网络，比如4G LTE|
|wifi|String|wifi网络|
|ethnet|String|以太网络|
|unknow|String|未识别的网络类型|

## getStatus - 获取当前网络状态

* 函数功能：  
获取当前网络状态信息。

* 函数原型：
> var netStatus = network.getStatus();

* 参数说明：  
无

* 返回值：  
String类型，具体如下，

|返回值|类型|说明|
|-----|----|----|
|'connect'|String|当前网络处于连接状态|
|'disconnect'|String|当前网络处于断连状态|

## getInfo - 获取当前网络详细信息

* 函数功能：  
获取当前网络详细信息。

* 函数原型：
> var info = network.getInfo();

* 参数说明：  
无

* 返回值：  
Object类型，具体如下，

|返回值|类型|说明|
|-----|----|----|
|ifconfig|Object|wifi或者以太网络信息|
|siminfo|Object|cellular网络，sim卡信息|
|locatorInfo|Object|cellular网络，设备连接基站信息|

* ifConfig属性说明： 

|返回值|类型|说明|
|-----|----|----|
|dhcp_en|Boolean|true：为DHCP动态获取IP模式；false: 静态IP模式。|
|ip_addr|String|IP地址|
|dns_server|String|DNS地址|
|gw|String|网关地址|
|mask|String|子网掩码|
|mac|String|MAC地址|
|rssi|String|WIFI信号强度，值越大，信号越强|

* simInfo属性说明： 

|返回值|类型|说明|
|-----|----|----|
|imsi|String|imsi 国际移动用户识别码|
|imei|String|imei 国际移动设备识别码|
|iccid|String|iccid 集成电路卡识别码|

* locatorInfo属性说明：

|返回值|类型|说明|
|-----|----|----|
|cellid|String|cid 基站编号|
|lac|String|lac 位置区域码|
|mcc|String|mcc 移动国家代码(中国为460)|
|mnc|String|mnc 移动网络号码(中国移动为00， 中国联通为01)|
|signal|Number|接收信号强度值，值越大，信号越强|

## connect - 连接wifi网络
* 函数功能：  
如果是WiFi网络，连接网络。非WiFi网络调用无效，以太网/ Cellular网络的连接动作在驱动层处理，通过事件通知APP。

* 函数原型：
> network.connect({ssid: 'user-ssid', password: 'user-password'});

* 参数说明：
Object类型，具体描述如下，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|ssid|String|是|要连接热点的ssid|
|password|String|是|要连接热点的password|

* 返回值：  
无

## disconnect - 连接wifi网络
* 函数功能：  
如果是WiFi网络，连接网络。非WiFi网络调用无效，以太网/ Cellular网络的连接动作在驱动层处理，通过事件通知APP。

* 函数原型：
> network.disconnect();

* 参数说明：
无

* 返回值：  
无

## setIfConfig - 设置网络参数
* 函数功能：  
如果是WiFi/ 以太网络，可配置静态IP或者动态IP模式。Cellular网络调用无效。

* 函数原型：
> net.setIfConfig({
>      dhcp_en: true,             // true: 动态IP模式(以下参数可忽略）; false: 静态IP模式（以下参数必填）
>      ip_addr: '192.168.124.66',
>      mask: '255.255.255.0',
>      gw: '192.168.124.1',
>     dns_server: '8.8.8.8'
>    });

* 参数说明：  
Object类型，具体描述如下，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|dhcp_en|Boolean|是|true: 动态ip模式；false: 静态ip模式，以下参数无效|
|ip_addr|String|否|dhcp_en为false时，设置本地静态ip值|
|mask|String|否|dhcp_en为false时，设置本地mask值|
|gw|String|否|dhcp_en为false时，设置本地gw值|
|dns_server|String|否|dhcp_en为false时，设置本地dns_server值|

* 返回值：  
无

## getIfConfig - 获取当前网络ifconfig信息

* 函数功能：  
如果是WiFi/ 以太网络，可获取静态IP或者动态IP参数。Cellular网络调用无效。

* 函数原型：
> var config = network.getIfConfig();

* 参数说明：  
无

* 返回值：  
Object类型，具体如下，

|返回值|类型|说明|
|-----|----|----|
|dhcp_en|Boolean|true: 动态ip模式；false: 静态ip模式|
|ip_addr|String|dhcp_en为false时，获取本地静态ip值|
|mask|String|dhcp_en为false时，获取本地mask值|
|gw|String|dhcp_en为false时，获取本地gw值|
|dns_server|String|dhcp_en为false时，获取本地dns_server值|
|mac|String|dhcp_en为false时，获取本地mac值|
|rssi|String|dhcp_en为false时，获取本地rssi值|

## saveConfig - 以文件格式保存wifi配置信息
* 函数功能：  
仅wifi AP模式类型有效，设置AP ssid, pwd, ap_power, channel, sec_type等配置信息。以太网，Cellular网络调用无效。

* 函数原型：
> network.saveConfig();

* 参数说明：
无

* 返回值：  
无

## 使用实例
* 通过WIFI连接路由器
WIFI设备用户需要替换network.connect(...);参数中的ssid和password才可以成功连接路由器。app会监听disconnect和disconnect事件。<br>
-- [HaaS EDU K1 WIFI连接路由器示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/haaseduk1/network/wifi)
<br>
-- [ESP32 WIFI连接路由器示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/haaseduk1/network/wifi)
<br>
* 通过以太网连接路由器
以太网设备只需网线接上即可自动连接路由器，拔掉网线即可断链。app会监听disconnect和disconnect事件。<br> 
-- [HaaS100 以太网连接路由器示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/haaseduk1/network/ethernet)
<br>
* Cellular蜂窝连接基站
4G Cat.1设备上电后会自动连接基站，app会监听disconnect和disconnect事件。<br>
-- [HaaS600/510/531 4G Cat.1连接基站示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/haaseduk1/network/cellular)<br>

## 硬件平台

|硬件平台|是否支持？|说明|
|-----|:---:|----|
|HaaS100|是||
|HaaS200|是||
|HaaS EDU K1|是||
|HaaS600-EC100Y|是||
|HaaS600-EC600S|是||
|HaaS510|是||
|HaaS531|是||
|ESP32|是||

<br>


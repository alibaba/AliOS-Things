# OTA - 远程升级
* 模块功能：
实现设备的远程文件升级、和系统固件升级两个功能

* 注意事项：
需要确保wifi和aliyunIoT连接成功,并且导入ota模块。

请使用下面的代码导入ota模块：

```python
import ota
```
请使用下面的代码连接wifi：

```python
import network
import utime
sta_if = network.WLAN(network.STA_IF)
sta_if.active(True)
sta_if.connect('<AP_name>', '<password>')
utime.sleep_ms(500)
while not sta_if.isconnected():
    utime.sleep_ms(500)
```
直到sta_if.isconnected()返回 True 则表明网络连接成功
  
请使用下面的代码连接aliyunIoT，直到on_connect的回调产生表明aliyunIoT连接成功。
```python
  from aliyunIoT import Device
  import utime

  on_connect_lp_succeed = False
  def on_connect(data):
    global on_connect_lp_succeed
    on_connect_lp_succeed = True
  
  aliyunIoT_Device = Device()
  key_info = {
    'region' : 'cn-shanghai' ,
    'productKey': '<productKey>',
    'deviceName': '<deviceName>' ,
    'deviceSecret': '<deviceSecret>',
    'keepaliveSec': 60
  }
  aliyunIoT_Device.on(Device.ON_CONNECT,  on_connect)
  aliyunIoT_Device.connect(key_info)
  while not on_connect_lp_succeed:
 		usleep.sleep_ms(50)
```
  
# init - 初始化

* 函数功能：  
ota 初始化，建立ota后台服务线程

* 注意事项：  
  
确保wifi连接成功
确保aliyunIoT连接成功

* 函数原型：  
> ota.init(data_handle)

* 参数说明：

| 参数 | 类型 | 必选参数？ | 说明 |
| -------- | -------- | -------- | -------- |
| data_handle     | dictionary     | 是     |ota初始化需要参数     |

data_handle参数说明:


| 参数 | 类型 | 必选参数？ | 说明 |
| -------- | -------- | -------- | -------- |
| device_handle     | int     | 是     | aliyunIoT句柄    |

* 返回值说明:  
 成功返回0， 其他表示失败

* 函数示例：

```python
import ota

data_handle = {}
#aliyunIoT_Device是连接aliyunIoT时Device()返回的句柄
data_handle['device_handle'] = aliyunIoT_Device.getDeviceHandle()
ota.init(data_handle)
```
  
#	report - 上报当前模块版本

* 函数功能：  
上报模块版本号信息

* 注意事项：  

  * 确保wifi连接成功
  * 确保aliyunIoT连接成功
  
* 函数原型：  
> ota.report(report_info)

* 参数说明:  

| 参数 | 类型 | 必选参数？ | 说明 |
| -------- | -------- | -------- | -------- |
| report_info     | dictionary     | 是     | 上报模块版本号需要的信息    |

report_info参数说明:  

| 参数 | 类型 | 必选参数？ | 说明 |
| -------- | -------- | -------- | -------- |
| device_handle     | int     | 是     | aliyunIoT句柄    |
| product_key     | string     | 是     | 上云三元组信息product_key    |
| device_name     | string     | 是     | 上云三元组信息device_name    |
| module_name     | string     | 是     | 待上报模块的名字   |
| module_name     | string     | 是     | 待上报模块的版本号  |

* 返回值说明：  
成功返回0， 其他表示失败。

* 使用示例:  

```python
 report_info = { 
                          "device_handle": aliyunIoT_Device.getDeviceHandle() ,
                          "product_key": ‘<productKey>' ,
                          "device_name": '<deviceName >',
                          "module_name": '<your_module_name>',
                          "version": '<your_module_version>'
                      }   
 ota.report(report_info)

'''
 aliyunIoT_Device是连接aliyunIoT时Device()返回的句柄。
 '<your_module_name>'如果系统固件需要指定成'default', 其他的可以自己定义名字，和云端保持一致即可；
 '<your_module_version>'是当前模块版本号;
 ‘<productKey>’和'<deviceName>'是三元组的productkey和devicename；
 '''
```

 # on  - 回调注册 - 1

* 函数功能：  
注册OTA的on_trigger回调函数，在升级开始时触发

* 注意事项：  

  * 确保wifi连接成功。
  * 确保aliyunIoT连接成功。
  * 确保ota.init执行完成。
  
* 函数原型：  
> ota.on(1, on_trigger)

* 参数说明：
  
| 参数 | 类型 | 必选参数？ | 说明 |
| -------- | -------- | -------- | -------- |
|   on_trigger  | funcCallback     | 是     | 文件升级触发时的回调函数     |

on_trigger函数原型：

```python
def on_trigger(data)
```
on_trigger函数参数说明:

| 参数 | 类型 | 必选参数？ | 说明 |
| -------- | -------- | -------- | -------- |
|   data  | dictionary     | 是     | on_trigger函数触发时传入on_trigger的参数     |

on_trigger函数data参数说明:

| 参数 | 类型 | 必选参数？ | 说明 |
| -------- | -------- | -------- | -------- |
|   url  | string     | 是     | 待升级文件所在url     |
|   length  | int     | 是     | 待升级文件文件长度，单位是字节     |
|   module_name  | string     | 是     | 待升级文件的模块名称    |
|   version  | string     | 是     | 待升级文件的版本号    |
|   hash  | string     | 是     | 待升级文件的哈希值    |
|   hash_type  | string     | 是     | 待升级文件的哈希类型   |

* 返回值说明：  
成功返回0， 其他表示失败。

# on  - 回调注册 - 2
  
* 函数功能：  
注册OTA的on_download回调函数，在download完成时触发

* 注意事项：  

  * 确保wifi连接成功
  * 确保aliyunIoT连接成功
  * 确保ota.init执行完成
  
* 函数原型：  
> ota.on(2, on_download)

* 参数说明：

| 参数 | 类型 | 必选参数？ | 说明 |
| -------- | -------- | -------- | -------- |
|   on_download  | funcCallback     | 是     | 文件下载完成时的回调函数     |

on_download函数原型：
> def on_download(data)

on_download函数参数说明:

| 参数 | 类型 | 必选参数？ | 说明 |
| -------- | -------- | -------- | -------- |
|   data  | int     | 是     | on_download函数触发时传入on_download的参数 ，大于等于0说明download成功，否则失败    |

* 返回值说明：  
成功返回0， 其他表示失败。

 # on  - 回调注册 - 3
  
* 函数功能：    
注册OTA的on_verify回调函数，在verify完成时触发

* 注意事项：  

  * 确保wifi连接成功
  * 确保aliyunIoT连接成功
  * 确保ota.init执行完成
  
* 函数原型：  
> ota.on(3, on_verify)

* 参数说明：  

| 参数 | 类型 | 必选参数？ | 说明 |
| -------- | -------- | -------- | -------- |
|   on_verify  | funcCallback     | 是     | 文件校验完成时的回调函数     |

* on_verify函数原型：
> def on_verify(data)

* on_verify函数参数说明:
| 参数 | 类型 | 必选参数？ | 说明 |
| -------- | -------- | -------- | -------- |
|   data  | int     | 是     | on_verify函数触发时传入on_download的参数 ，大于等于0说明verify成功，否则失败    |

* 返回值说明：  
成功返回0， 其他表示失败。

# download - 文件下载

* 函数功能：  
发起文件下载流程。
  
* 注意事项：  

  * 确保wifi连接成功
  * 确保aliyunIoT连接成功
  * 确保ota.init执行完成
  
* 函数原型：  
> ota.download(data)

* 参数说明：

| 参数 | 类型 | 必选参数？ | 说明 |
| -------- | -------- | -------- | -------- |
| data     | dictionary     | 是     | 发起文件下载流程所需要的信息    |

data参数说明:

| 参数 | 类型 | 必选参数？ | 说明 |
| -------- | -------- | -------- | -------- |
| url     | string     | 是     | 文件所在的url信息    |
| store_path     | string     | 是     | 文件要下载到的文件系统路径    |

* 返回值说明：  
成功返回0， 其他表示失败。

*使用示例：
```python
dl_data = {}
dl_data['url'] = '<your_url>'
dl_data['store_path'] = '<your_file_store_path>'
ota.download(dl_data)
```
url一般从on_trigger回调函数中获取，store_path为你想要下载文件到什么地方。

# verify - 文件校验
   
* 函数功能：    
发起文件校验流程
  
* 注意事项：  

  * 确保wifi连接成功
  * 确保aliyunIoT连接成功
  * 确保ota.init执行完成
  
* 函数原型：  
> ota.verify(data)

* 参数说明：

| 参数 | 类型 | 必选参数？ | 说明 |
| -------- | -------- | -------- | -------- |
| data     | dictionary     | 是     | 发起文件校验流程所需要的信息    |

data参数说明:

| 参数 | 类型 | 必选参数？ | 说明 |
| -------- | -------- | -------- | -------- |
| length     | int     | 是     | 文件长度，单位是字节    |
| store_path     | string     | 是     | 文件所在的文件系统路径    |
| hash_type     | string     | 是     | 文件的哈希类型   |
| hash     | string     | 是     | 文件的哈希值  |

* 返回值说明：  
成功返回0， 其他表示失败。

*使用示例：

```python
param = {}
param['length'] = '<your_file_length>'
param['store_path'] = '<your_file_store_path>'
param['hash_type'] = info['hash_type']
param['hash'] = info['hash']
ota.verify(param)
```

store_path和download传入的store_path要保持一致；length、hash_type和hash一般从on_trigger回调函数参数获取。

# ota - 系统固件升级
在初始化过后，只需要向云端上报一个比云端固件版本低的版本即可触发系统固件升级。

# ota - 文件系统文件升级

```python
info = {
  'url': '',
  'store_path': '<your_store_path>',
  'install_path': '<your_install_path>',
  'length': 0,
  'hash_type':'',
  'hash':''
}

def on_trigger(data):
  global info
  # 保存服务端推送的ota信息
  info['url'] = data['url']
  info['length'] = data['length']
  info['module_name'] = data['module_name']
  info['version'] = data['version']
  info['hash'] = data['hash']
  info['hash_type'] = data['hash_type']
  # 开始ota 包下载
  dl_data = {}
  dl_data['url'] = info['url']
  dl_data['store_path'] = info['store_path']
  ota.download(dl_data)

def on_download(data):
	global info
	if data >= 0:
		print('Ota download succeed')
	# 开始ota包校验
	param = {}
	param['length'] = info['length']
	param['store_path'] = info['store_path']
	param['hash_type'] = info['hash_type']
	param['hash'] = info['hash']
	ota.verify(param)

def on_verify(data):
	global info
	print(data)
	if data >= 0 :
		print('Ota verify succeed')

ota.on(1, on_trigger)
ota.on(2, on_download)
ota.on(3, on_verify)
```

文件系统文件升级需要注册3个函数，在云端触发文件升级后，会触发on_trigger函数，并将文件的一系列信息通过on_trigger参数传入， 我们可以将这些参数保存到全局变量中；在on_trigger触发后，我们在on_trigger回调函数中调用ota.download函数来下载文件，ota.download函数下载完成后，会产生on_download回调，我们根据on_download的传入参数判断下载文件是否成功，如果成功， 我们在on_download回调函数中调用ota.verify来做文件校验；ota.verify函数校验完成后，会产生on_verify回调，我们根据on_verify的传入参数来判断校验文件是否成功。

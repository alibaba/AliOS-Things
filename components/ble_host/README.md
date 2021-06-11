@page ble_host ble_host

[更正文档](https://gitee.com/alios-things/ble_host/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

# 概述
AliOS Things 3.3提供支持符合蓝牙4.0/4.2/5.0核心协议规范的BLE Host软件协议栈组件，方便用户使用蓝牙BLE功能。<br />ble_host组件功能框图如下图红色部分:

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN018Gd9qR2ALhGFNkTT9_!!6000000008187-2-tps-830-730.png#align=left&display=inline&height=461&margin=%5Bobject%20Object%5D&name=image.png&originHeight=922&originWidth=1932&size=259937&status=done&style=none&width=966#align=left&display=inline&height=537&margin=%5Bobject%20Object%5D&originHeight=730&originWidth=830&status=done&style=none&width=611#align=left&display=inline&height=730&margin=%5Bobject%20Object%5D&originHeight=730&originWidth=830&status=done&style=none&width=830" style="max-width:800px;" />
</div>

## 功能支持
ble_host组件主要支持如下功能：

- Generic Access Profile(GAP)角色支持
   - Peripheral&Central
   - Observer&Broadcaster
- Generic Attribute Profile(GATT)连接支持
   - GATT client
   - GATT server
- Security Manager(SM)支持
   - Legacy Pairing
   - 多安全等级设定Security Level 1, 2, 3, 4
   - 安全连接Security Connection
   - LE Privacy(RPA地址生成)
- HCI接口支持
   - 标准HCI接口，支持host-only，host通过HCI硬件接口（以UART为主）和controller对接
   - 虚拟HCI接口，支持host+controller，适合SoC的硬件平台



## 版权说明
> Apache license v2.0



## 目录结构
```tree
|-- ble_profiles             #BLE服务
|-- bt_crypto                #BLE安全
|-- bt_defconfig             #BLE配置项
|-- bt_host                  #BLE Host核心代码
|-- bt_preconfig             #BLE预配置项
|-- bt_shell                 #BLE控制台命令
|-- include                  #头文件
|-- package.yaml             #makefile
|-- README.md                #README文档
|-- SConscript
|-- script
```


## 依赖组件

- osal_aos



# 常用配置


## 配置列表
配置在组件的package.yaml中定义，均已默认打开，用户可根据需求裁剪。

| CONFIG_BT_CENTRAL | BLE Central角色功能开关 |
| :--- | :--- |
| CONFIG_BT_PERIPHERAL | BLE 外设角色功能开关 |
| CONFIG_BT_SMP | BLE SMP功能开关 |
| CONFIG_BT_GATT_CLIENT | BLE GATT Client功能开关 |
| CONFIG_BT_OBSERVER | BLE 广播扫描功能开关 |



# API说明
## API列表
| ble_stack_init | BLE协议栈初始化 |
| :--- | :--- |
| ble_stack_event_register | 注册BLE事件处理函数 |
| ble_stack_adv_start | BLE 广播开始 |
| ble_stack_adv_stop | BLE 广播停止 |
| ble_stack_scan_start | BLE 扫描开始 |
| ble_stack_scan_stop | BLE 扫描停止 |
| ble_stack_gatt_mtu_get | GATT，获取当前连接MTU大小 |
| ble_stack_gatt_registe_service | GATT server，服务注册 |
| ble_stack_gatt_notificate | GATT server，属性notificate方式上报 |
| ble_stack_gatt_indicate | GATT server，属性indicate方式上报 |
| ble_stack_gatt_mtu_exchange | GATT client，协商MTU大小 |
| ble_stack_gatt_discovery | GATT client，发现对端GATT服务 |
| ble_stack_gatt_read | GATT client，读取对端GATT server属性 |
| ble_stack_gatt_read_multiple | GATT client，读取对端GATT server多个属性 |
| ble_stack_gatt_write | GATT client，写入对端GATT server属性 |
| ble_stack_connect | 开始BLE连接 |
| ble_stack_disconnect | 断开BLE连接 |
| ble_stack_connect_param_update | BLE连接参数协商 |
| ble_stack_security | BLE连接安全等级设置 |
| ble_stack_iocapability_set | BLE SMP IO能力配置 |
| ble_stack_smp_cancel | BLE SMP配对请求取消 |
| ble_stack_smp_passkey_entry | BLE SMP passkey输入 |
| ble_stack_smp_passkey_confirm | BLE SMP passkey确认 |
| ble_stack_dev_unpair | PIN SMP配对解绑 |



## API详情


### ble_stack_init
BLE模块初始化。<br />**函数原型**
``` c
int ble_stack_init(init_param_t *param)
```
**输入参数**

| args | description |
| :--- | :--- |
| param.dev_name | 设备名，string类型，不超过28字符 |
| param.dev_addr | 设备mac地址，仅设置为random类型时生效 |
| param.conn_num_max | 最大连接数 |

**返回参数**<br />`0：`成功， `其他值`：失败。<br />

### ble_stack_event_register
注册BLE协议栈事件回调函数。<br />注意：入参不能是一个局部变量。<br />**函数原型**
``` c
int ble_stack_event_register(ble_event_cb_t *callback)
```
**输入参数**

| args | description |
| :--- | :--- |
| callback.callback | 回调函数 |
| callback.next | 无需填写，由API函数处理 |

**返回参数**<br />`0：`成功， `其他值`：失败。<br />

### ble_stack_adv_start
打开BLE广播功能。<br />**函数原型**
``` c
int ble_stack_adv_start(adv_param_t *param)
```
**输入参数**

| args | description |
| :--- | :--- |
| param.type | 广播类型，定义如下<br />    0x00  普通广播<br />    0x01  直连广播<br />    0x02  可连接可扫描广播<br />    0x03  不可连接广播,<br />    0x04  低占空直连广播 |
| param.ad | ADV广播内容数组 |
| param.sd | Scan Response广播内容数组 |
| param.ad_num | ADV广播内容数组大小 |
| param.sd_num | Scan Response广播内容数组大小 |
| param.interval_min | 广播发送间隔最小值 |
| param.interval_max | 广播发送间隔最大值 |
| param.filter_policy | 广播过滤开关，定义如下<br />    0x00  接受所有连接与扫描请求<br />    0x01  接受所有连接请求，接受白名单设备的扫描请求<br />    0x02  接受白名单的连接请求，接受所有扫描请求<br />    0x03  仅接受白名单内的扫描与连接请求 |
| param.channel_map  | 广播信道设置，<br />    bit0: 37信道<br />    bit1: 38信道<br />    bit2: 39信道  |
| param.direct_peer_addr  |  直连广播设备地址，仅在广播类型为直连类型时才生效  |

**返回参数**<br />`0：`成功， `其他值`：失败。<br />

### ble_stack_adv_stop
关闭BLE广播功能。<br />**函数原型**
``` c
int ble_stack_adv_stop()
```
**输入参数**

| args | description |
| :--- | :--- |
| 无 |  |

**返回参数**<br />`0：`成功， `其他值`：失败。<br />

### ble_stack_scan_start
打开BLE扫描功能。<br />**函数原型**
``` c
int ble_stack_scan_start(const scan_param_t *param)
```
**输入参数**

| args | description |
| :--- | :--- |
| param.type | 扫描类型，定义如下<br />    0x00  被动扫描<br />    0x01   主动扫描 |
| param.filter_dup | 重复设备上报过滤开关，定义如下<br />    0x00  关闭过滤<br />    0x01  开启过滤 |
| param.interval  | 扫描间隔，单位为0.625毫秒  |
| param.window  | 扫描窗大小，单位为0.625毫秒  |
| param.scan_filter  | 扫描过滤类型，定义如下<br />    0x00  所有广播均上报<br />    0x01  仅白名单设备的广播上报 |

**返回参数**<br />`0：`成功， `其他值`：失败。<br />

### ble_stack_scan_stop
关闭BLE扫描功能。<br />异步事件，扫描结果在注册的事件回调函数中返回，返回事件EVENT_GAP_DEV_FIND<br />**函数原型**
``` c
int ble_stack_scan_stop()
```
**输入参数**

| args | description |
| :--- | :--- |
| 无 |  |

**返回值**<br />`0：`成功， `其他值`：失败。<br />

### ble_stack_gatt_mtu_get
获取一个连接的当前MTU大小<br />**函数原型**
``` c
int ble_stack_gatt_mtu_get(int16_t conn_handle)
```
**输入参数**

| args | description |
| :--- | :--- |
| conn_handle | 连接句柄，在连接成功后分配 |

**返回值**<br />23-65535: MTU大小<br />

### ble_stack_gatt_registe_service
注册一个GATT服务<br />**函数原型**
``` c
int ble_stack_gatt_registe_service(gatt_service *s, gatt_attr_t attrs[], uint16_t attr_num)
```
**输入参数**

| args | description |
| :--- | :--- |
| s.bt_gatt_attr | 无需填写，由API赋值 |
| s.attr_count | 无需填写，由API赋值 |
| s.node | 无需填写，由API赋值 |
| attrs | 属性列表 |
| attr_num | 属性列表大小 |

**返回值**<br />`0：`成功， `其他值`：失败。<br />

### ble_stack_gatt_notificate
GATT服务某个属性值上报，此上报方式无需GATT Client回复<br />**函数原型**
``` c
int ble_stack_gatt_notificate(int16_t conn_handle, uint16_t char_handle, const uint8_t *data, uint16_t len)
```
**输入参数**

| args | description |
| :--- | :--- |
| conn_handle | 连接句柄，在连接成功后分配 |
| char_handle | 属性句柄，在注册服务时分配 |
| data | 上报数据指针 |
| len | 上报数据长度 |

**返回值**<br />`0：`成功， `其他值`：失败。<br />

### ble_stack_gatt_indicate
GATT服务某个属性值上报，此上报方式GATT Client回复confirm。<br />**函数原型**
``` c
int ble_stack_gatt_indicate(int16_t conn_handle, int16_t char_handle, const uint8_t *data, uint16_t len)
```
**输入参数**

| args | description |
| :--- | :--- |
| conn_handle | 连接句柄，在连接成功后分配 |
| char_handle | 属性句柄，在注册服务时分配 |
| data | 上报数据指针 |
| len | 上报数据长度 |

**返回值**<br />`0：`成功， `其他值`：失败。<br />

### ble_stack_gatt_mtu_exchange
GATT Client功能，确保配置项CONFIG_BT_GATT_CLIENT配置。<br />GATT协商MTU大小，协商大小由配置项CONFIG_BT_L2CAP_RX_MTU与CONFIG_BT_L2CAP_TX_MTU的最小值以及对端设备的回复决定。<br />异步事件，执行结果在注册的事件回调函数中返回，返回事件EVENT_GATT_MTU_EXCHANGE<br />**函数原型**
``` c
int ble_stack_gatt_mtu_exchange(int16_t conn_handle)
```

<br />**输入参数**

| args | description |
| :--- | :--- |
| conn_handle | 连接句柄，在连接成功后分配 |


<br />**返回值**<br />`0：`成功， `其他值`：失败。<br />

### ble_stack_gatt_discovery
GATT Client功能，确保配置项CONFIG_BT_GATT_CLIENT配置。<br />GATT服务或者属性项发现。<br />异步事件，执行结果在注册的事件回调函数中返回，返回事件根据入参不同而不同

**函数原型**
``` c
int ble_stack_gatt_discovery(int16_t conn_handle,
                             gatt_discovery_type_en type,
                             uuid_t *uuid,
                             uint16_t start_handle,
                             uint16_t end_handle)
```
**输入参数**

| args | description |
| :--- | :--- |
| conn_handle | 连接句柄，在连接成功后分配 |
| type | GATT发现类型，定义如下<br />    0x00  发现主服务<br />    0x01  发现包含服务<br />    0x02  发现属性项<br />    0x03  发现属性描述项 |
| uuid | 需要发现的UUID值 |
| start_handle | 发现起始句柄 |
| end_handle | 发现结束句柄 |

**返回值**<br />`0：`成功， `其他值`：失败。<br />

### ble_stack_gatt_read
GATT Client功能，确保配置项CONFIG_BT_GATT_CLIENT配置。<br />GATT读取某个属性项的属性值<br />异步事件，执行结果在注册的事件回调函数中返回，返回事件EVENT_GATT_CHAR_READ_CB<br />**函数原型**
``` c
int ble_stack_gatt_read(int16_t conn_handle, uint16_t attr_handle, uint16_t offset)
```
**输入参数**

| args | description |
| :--- | :--- |
| conn_handle | 连接句柄，在连接成功后分配 |
| attr_handle | 读取的属性项句柄 |
| offset | 读取起始位置 |

**返回值**<br />`0：`成功， `其他值`：失败。<br />

### ble_stack_gatt_read_multiple
GATT Client功能，确保配置项CONFIG_BT_GATT_CLIENT配置。<br />GATT读取多个属性项的属性值<br />异步事件，执行结果在注册的事件回调函数中返回，返回事件EVENT_GATT_CHAR_READ_CB<br />**函数原型**
``` c
int ble_stack_gatt_read_multiple(int16_t conn_handle, uint16_t attr_count, uint16_t attr_handle[])
```
**输入参数**

| args | description |
| :--- | :--- |
| conn_handle | 连接句柄，在连接成功后分配 |
| attr_count | 读取的属性项列表大小 |
| attr_handle | 读取的属性项句柄列表 |

**返回值**<br />`0：`成功， `其他值`：失败。<br />

### ble_stack_gatt_write
GATT Client功能，确保配置项CONFIG_BT_GATT_CLIENT配置。<br />GATT写入某个属性项的属性值<br />异步事件，执行结果在注册的事件回调函数中返回，返回事件EVENT_GATT_CHAR_WRITE_CB<br />**函数原型**
``` c
int ble_stack_gatt_write(int16_t conn_handle, uint16_t attr_handle, uint8_t *data, uint16_t len, uint16_t offset, gatt_write_en type)
```
**输入参数**

| args | description |
| :--- | :--- |
| conn_handle | 连接句柄，在连接成功后分配 |
| attr_handle | 写入的属性项句柄 |
| data | 写入的数据指针 |
| len | 写入的数据长度 |
| offset | 写入到属性项的起始位置 |
| type | 写入类型，定义如下<br />    0x00  带回复写入<br />    0x01  不带回复写入<br />    0x02  带签名写入 |

**返回值**<br />`0：`成功， `其他值`：失败。<br />

### ble_stack_gatt_read_multiple
GATT Client功能，确保配置项CONFIG_BT_GATT_CLIENT配置。<br />此函数用于GATT读取多个属性项的属性值。<br />异步事件，执行结果在注册的事件回调函数中返回，返回事件EVENT_GATT_CHAR_READ_CB<br />**函数原型**
``` c
int ble_stack_gatt_read_multiple(int16_t conn_handle, uint16_t attr_count, uint16_t attr_handle[])
```
**输入参数**

| args | description |
| :--- | :--- |
| conn_handle | 连接句柄，在连接成功后分配 |
| attr_count | 读取的属性项列表大小 |
| attr_handle | 读取的属性项句柄列表 |

**返回值**<br />`0：`成功， `其他值`：失败。<br />

### ble_stack_connect
BLE连接功能，确保配置项CONFIG_BT_CONN配置。<br />此函数用于BLE连接某个设备。<br />异步事件，执行结果在注册的事件回调函数中返回，返回事件EVENT_GATT_CONN_CHANGE<br />**函数原型**
``` c
int ble_stack_connect(dev_addr_t *peer_addr, conn_param_t *param, uint8_t auto_connect)
```
**输入参数**

| args | description |
| :--- | :--- |
| peer_addr.type | 对端设备的地址类型 |
| peer_addr.val | 对端设备的蓝牙地址 |
| param.interval_min | 连接间隔最小值参数，单位是0.625毫秒 |
| param.interval_max | 连接间隔最大值参数，单位是0.625毫秒 |
| param.lantency | 连接延迟参数，单位是连接间隔，注意此时间长度不能超过timeout参数的一半。 |
| param.timeout | 连接超时参数，单位10ms |
| auto_connect | 是否自动连接定义如下<br />    0x00  不自动连接<br />    0x01  自动连接 | |

**返回值**<br />`0：`成功， `其他值`：失败。<br />

### ble_stack_disconnect
BLE连接功能，确保配置项CONFIG_BT_CONN配置。<br />此函数用于BLE断开某个设备的连接。<br />异步事件，执行结果在注册的事件回调函数中返回，返回事件EVENT_GATT_CONN_CHANGE<br />**函数原型**
``` c
int ble_stack_disconnect(int16_t conn_handle)
```
**输入参数**

| args | description |
| :--- | :--- |
| conn_handle | 连接句柄，在连接事件中返回 |

**返回值**<br />`0：`成功， `其他值`：失败。<br />

### ble_stack_connect_param_update
BLE连接功能，确保配置项CONFIG_BT_CONN配置。<br />此函数用于BLE修改某个连接的连接参数。<br />异步事件，执行结果在注册的事件回调函数中返回，返回事件EVENT_GAP_CONN_PARAM_UPDATE<br />**函数原型**
``` c
int ble_stack_connect_param_update(int16_t conn_handle, conn_param_t *param)
```
**输入参数**

| args | description |
| :--- | :--- |
| conn_handle | 连接句柄，在连接事件中返回 |
| param.interval_min | 连接间隔最小值参数，单位是0.625毫秒 |
| param.interval_max | 连接间隔最大值参数，单位是0.625毫秒 |
| param.lantency | 连接延迟参数，单位是连接间隔，注意此时间长度不能超过timeout参数的一半。 |
| param.timeout | 连接超时参数，单位10ms |

**返回值**<br />`0：`成功， `其他值`：失败。<br />

### ble_stack_security
BLE连接功能，确保配置项CONFIG_BT_CONN配置。<br />此函数用于BLE修改某个连接的安全参数。<br />异步事件，执行结果在注册的事件回调函数中返回，返回事件EVENT_GAP_CONN_SECURITY_CHANGE<br />**函数原型**
``` c
int ble_stack_security(int16_t conn_handle, security_en level)
```
**输入参数**

| args | description |
| :--- | :--- |
| conn_handle | 连接句柄，在连接事件中返回 |
| level | 安全等级，定义如下<br />    0x00  无安全，无加密无鉴权<br />    0x01  低安全，等同于0x00<br />    0x02  中安全，有加密无鉴权<br />    0x03  高安全，有加密有鉴权 |

**返回值**<br />`0：`成功， `其他值`：失败。<br />

### ble_stack_iocapability_set
BLE SMP配对功能，确保配置项CONFIG_BT_SMP配置。<br />此函数用于设置BLE设备设备的IO能力，这个设置会在BLE SMP配对被使用。<br />**函数原型**
``` c
int ble_stack_iocapability_set(uint8_t io_cap)
```
**输入参数**

| args | description |
| :--- | :--- |
| io_cap | IO能力，定义如下<br />    bit0-bit2：<br />                  0x01  无输入<br />                  0x02  输入正确/错误<br />                  0x03  键盘输入<br />    bit3-bit4:<br />                  0x00  无输出<br />                  0x01  显示器输出 |

**返回值**<br />`0：`成功， `其他值`：失败。<br />

### ble_stack_smp_cancel
BLE SMP配对功能，确保配置项CONFIG_BT_SMP配置。<br />此函数用于取消当前BLE的SMP配对请求。<br />**函数原型**
``` c
int ble_stack_smp_cancel(int16_t conn_handle)
```
**输入参数**

| args | description |
| :--- | :--- |
| conn_handle | 连接句柄，在连接事件中返回 |

**返回值**<br />`0：`成功， `其他值`：失败。<br />

### ble_stack_smp_passkey_entry
BLE SMP配对功能，确保配置项CONFIG_BT_SMP配置。<br />此函数用于SMP Passkey模式下输入当前passkey，当接收到EVENT_SMP_PASSKEY_ENTER后调用。<br />**函数原型**
``` c
int ble_stack_smp_passkey_entry(int16_t conn_handle, uint32_t passkey)
```
**输入参数**

| args | description |
| :--- | :--- |
| conn_handle | 连接句柄，在连接事件中返回 |
| passskey | 0-999999之间 |

**返回值**<br />`0：`成功， `其他值`：失败。<br />

### ble_stack_smp_passkey_confirm
BLE SMP配对功能，确保配置项CONFIG_BT_SMP配置。<br />此函数用于SMP Passkey模式下确认当前的passkey是否正确，当接收到EVENT_SMP_PASSKEY_CONFIRM后调用。<br />**函数原型**
``` c
int ble_stack_smp_passkey_confirm(int16_t conn_handle)
```
**输入参数**

| args | description |
| :--- | :--- |
| conn_handle | 连接句柄，在连接事件中返回 |

**返回值**<br />`0：`成功， `其他值`：失败。<br />

### ble_stack_dev_unpair
BLE SMP配对功能，确保配置项CONFIG_BT_SMP配置。<br />此函数用于解除某台已SMP配对的设备的配对，如果连接存在则断开连接。<br />**函数原型**
``` c
int ble_stack_dev_unpair(dev_addr_t *peer_addr)
```
**输入参数**

| args | description |
| :--- | :--- |
| peer_addr.type | 地址类型 |
| peer_addr.val | 设备地址 |

**返回值**<br />`0：`成功， `其他值`：失败。<br />

# 使用示例
本用例测试过程中使用到的工具有：

- 串口工具
- NRF Connect(手机)

组件使用示例相关的代码下载、编译和固件烧录均依赖AliOS Things配套的开发工具 **alios-studio** ，所以首先需要参考[《aos-studio使用说明之搭建开发环境》](https://g.alicdn.com/alios-things-3.3/doc/setup_env.html)，下载安装 **alios-studio** 。
待开发环境搭建完成后，可以按照以下步骤进行示例的测试。

## 步骤1 创建或打开工程

**打开已有工程**

如果用于测试的案例工程已存在，可参考[《aos-studio使用说明之打开工程》](https://g.alicdn.com/alios-things-3.3/doc/open_project.html)打开已有工程。

**创建新的工程**

组件的示例代码可以通过编译链接到AliOS Things的任意案例（solution）来运行，这里选择helloworld_demo案例。helloworld_demo案例相关的源代码下载可参考[《aos-studio使用说明之创建工程》](https://g.alicdn.com/alios-things-3.3/doc/create_project.html)。

## 步骤2 添加组件

案例下载完成后，需要在helloworld_demo组件的package.yaml中添加对组件的依赖：

```yaml
depends:
  - ble_host: master
```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install ble_host

```

上述命令执行成功后，组件源码则被下载到了./components/ble_host路径中。

## 步骤4 添加示例

修改solutions/helloworld_demo/helloworld_demo.c，如下：

**头文件修改**
```c
#include <aos/ble.h>
#include <atomic.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/gatt.h>
#include <bluetooth/uuid.h>
```

**代码修改**

```c
#define EXAMPLE_BLE_DEV_NAME        "HaaS BLE"
#define DEVICE_ADDR                 {0xE8,0x3B,0xE3,0x88,0xB4,0xC8}

int application_start(int argc, char *argv[])
{
    int count = 0;
    int ret;
    dev_addr_t addr = {DEV_ADDR_LE_RANDOM, DEVICE_ADDR};
    init_param_t init = {
        .dev_name = EXAMPLE_BLE_DEV_NAME,
        .dev_addr = &addr,   //&addr,
        .conn_num_max = 1,
    };

    printf("nano entry here!\r\n");

#if 1
    /* bt stack init */
    ret = ble_stack_init(&init);
    if (ret) {
        printf("error: ble_stack_init!, ret = %x\r\n", ret);
        return -1;
    }
#endif

    while(1) {
        printf("hello world! count %d \r\n", count++);
        aos_msleep(1000);
    };
}
```


## 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

## 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

## 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

## 步骤8 测试示例

打开串口工具，在串口工具中输入ble，可看到ble的cli命令集。<br />

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN01Vm24En1Hnoi4vJ9lC_!!6000000000803-2-tps-1932-922.png#align=left&display=inline&height=461&margin=%5Bobject%20Object%5D&name=image.png&originHeight=922&originWidth=1932&size=259937&status=done&style=none&width=966#align=left&display=inline&height=293&margin=%5Bobject%20Object%5D&originHeight=922&originWidth=1932&status=done&style=none&width=614#align=left&display=inline&height=922&margin=%5Bobject%20Object%5D&originHeight=922&originWidth=1932&status=done&style=none&width=1932" style="max-width:800px;" />
</div>

**BLE广播与连接测试**

在串口工具中输入ble adv start 020106，可以在串口日志中看到如下打印
```sh
adv_type:0;adv_interval_min:160 (*0.625)ms;adv_interval_max:240 (*0.625)ms
Advertising started
```

<br />此时在手机端打开NRF Connect工具并搜索广播包，可以看到设备。

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01CZZkJl1ThCNOYmP1f_!!6000000002413-2-tps-700-1268.png#align=left&display=inline&height=520&margin=%5Bobject%20Object%5D&name=image.png&originHeight=1268&originWidth=700&size=482965&status=done&style=none&width=287#align=left&display=inline&height=828&margin=%5Bobject%20Object%5D&originHeight=1268&originWidth=700&status=done&style=none&width=457#align=left&display=inline&height=949&margin=%5Bobject%20Object%5D&originHeight=1268&originWidth=700&status=done&style=none&width=524" style="max-width:800px;" />
</div>

<br />点击连接，可以看到Generic Access(GAP)和Generic Attribute(GATT)2个服务，点击GAP服务的Device Name的读取按键（下图红色圈中的箭头），可以读取到数值“HaaS BLE”<br />

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01jK0smX1FJvdIip6ZO_!!6000000000467-2-tps-696-726.png#align=left&display=inline&height=363&margin=%5Bobject%20Object%5D&name=image.png&originHeight=726&originWidth=696&size=250340&status=done&style=none&width=348#align=left&display=inline&height=469&margin=%5Bobject%20Object%5D&originHeight=726&originWidth=696&status=done&style=none&width=450#align=left&display=inline&height=548&margin=%5Bobject%20Object%5D&originHeight=726&originWidth=696&status=done&style=none&width=525" style="max-width:800px;" />
</div>

# 常见问题

Q：此协议栈是否支持蓝牙音乐播放
A：不支持经典蓝牙功能，仅支持BLE功能

Q：常见的GATT Service是否支持
A：支持，包括BAS,DIS,HIDS等等，详情请参考ble_host/ble_profiles文件下的实现。


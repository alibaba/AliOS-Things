## 详细技术说明文档, 请访问[官方WiKi](https://github.com/aliyun/iotkit-embedded/wiki)

## 编译配置项说明

**请先下载最新版本设备端C语言SDK** [SDK下载](https://help.aliyun.com/document_detail/42648.html)

解压之后, 打开编译配置文件`make.settings`, 根据需要编辑配置项:

    FEATURE_MQTT_COMM_ENABLED   = y          # 是否打开MQTT通道的总开关
    FEATURE_MQTT_DIRECT         = y          # 是否打开MQTT直连的分开关
    FEATURE_MQTT_DIRECT_NOTLS   = n          # 是否打开MQTT直连无TLS的分开关
    FEATURE_COAP_COMM_ENABLED   = y          # 是否打开CoAP通道的总开关
    FEATURE_HTTP_COMM_ENABLED   = y          # 是否打开HTTP通道的总开关
    FEATURE_SUBDEVICE_ENABLED   = n          # 是否打开主子设备功能的总开关
    FEATURE_SUBDEVICE_STATUS    = gateway    # 主子设备功能所处的功能状态
    FEATURE_CM_ENABLED          = y          # 是否打开CM功能的总开关

具体含义参见下表:

| 配置选项                    | 含义                                                            |
|-----------------------------|-----------------------------------------------------------------|
| FEATURE_MQTT_COMM_ENABLED   | 是否使能MQTT通道功能的总开关                                    |
| FEATURE_MQTT_DIRECT         | 是否用MQTT直连模式代替HTTPS三方认证模式做设备认证               |
| FEATURE_MQTT_DIRECT_NOTLS   | 使用MQTT直连模式做设备认证时, 是否要关闭MQTT over TLS           |
| FEATURE_COAP_COMM_ENABLED   | 是否使能CoAP通道功能的总开关                                    |
| FEATURE_HTTP_COMM_ENABLED   | 是否使能Https通道功能的总开关                                   |
| FEATURE_SUBDEVICE_ENABLED   | 是否使能主子设备通道功能的总开关                                    |
| FEATURE_SUBDEVICE_STATUS    | 主子设备功能所处的功能状态，取值有网关gateway(gw=1)和子设备subdevice(gw=0) |
| FEATURE_CLOUD_CONN_ENABLED  | 是否打开CLOUD_CONN功能的总开关, CLOUD_CONN支持MQTT/CoAP/HTTP之间的切换与扩展|
| FEATURE_CM_ENABLED          | 是否打开CMP功能的总开关,CMP: connectivity management platform |


## 编译 & 运行
请参考[README.md](https://github.com/aliyun/iotkit-embedded/blob/master/README.md)

## C-SDK提供的功能API说明

以下为华东2站点V2.0+版本的C-SDK提供的功能和对应的API, 用于让用户编写业务逻辑, 封装AT命令时, 也是基于这些API

更加准确详细和权威的描述, 以代码`src/sdk-impl/iot_export.h`, 以及`src/sdk-impl/exports/*.h`中的注释为准

如何使用这些API编写应用逻辑, 以代码`sample/*/*.c`的示例程序为准

### **API列表**
用如下命令, 可以列出当前SDK代码提供的所有面向用户的API函数:

    $ cd src/sdk-impl
    $ grep -o "IOT_[A-Z][_a-zA-Z]*[^_]\> *(" iot_export.h exports/*.h|sed 's!.*:\(.*\)(!\1!'|cat -n

     1  IOT_OpenLog
     2  IOT_CloseLog
     3  IOT_SetLogLevel
     4  IOT_DumpMemoryStats
     5  IOT_SetupConnInfo
     6  IOT_SetupConnInfoSecure
     7  IOT_Cloud_Connection_Init
     8  IOT_Cloud_Connection_Deinit
     9  IOT_Cloud_Connection_Send_Message
    10  IOT_Cloud_Connection_Yield
    11  IOT_CMP_Init
    12  IOT_CMP_Register
    13  IOT_CMP_Unregister
    14  IOT_CMP_Send
    15  IOT_CMP_Send_Sync
    16  IOT_CMP_Yield
    17  IOT_CMP_Deinit
    18  IOT_CMP_OTA_Yield
    19  IOT_CoAP_Init
    20  IOT_CoAP_Deinit
    21  IOT_CoAP_DeviceNameAuth
    22  IOT_CoAP_Yield
    23  IOT_CoAP_SendMessage
    24  IOT_CoAP_GetMessagePayload
    25  IOT_CoAP_GetMessageCode
    26  IOT_HTTP_Init
    27  IOT_HTTP_DeInit
    28  IOT_HTTP_DeviceNameAuth
    29  IOT_HTTP_SendMessage
    30  IOT_HTTP_Disconnect
    31  IOT_MQTT_Construct
    32  IOT_MQTT_ConstructSecure
    33  IOT_MQTT_Destroy
    34  IOT_MQTT_Yield
    35  IOT_MQTT_CheckStateNormal
    36  IOT_MQTT_Subscribe
    37  IOT_MQTT_Unsubscribe
    38  IOT_MQTT_Publish
    39  IOT_OTA_Init
    40  IOT_OTA_Deinit
    41  IOT_OTA_ReportVersion
    42  IOT_OTA_ReportProgress
    43  IOT_OTA_IsFetching
    44  IOT_OTA_IsFetchFinish
    45  IOT_OTA_FetchYield
    46  IOT_OTA_Ioctl
    47  IOT_OTA_GetLastError
    48  IOT_Shadow_Construct
    49  IOT_Shadow_Destroy
    50  IOT_Shadow_Yield
    51  IOT_Shadow_RegisterAttribute
    52  IOT_Shadow_DeleteAttribute
    53  IOT_Shadow_PushFormat_Init
    54  IOT_Shadow_PushFormat_Add
    55  IOT_Shadow_PushFormat_Finalize
    56  IOT_Shadow_Push
    57  IOT_Shadow_Push_Async
    58  IOT_Shadow_Pull
    59  IOT_Gateway_Generate_Message_ID
    60  IOT_Gateway_Construct
    61  IOT_Gateway_Destroy
    62  IOT_Subdevice_Login
    63  IOT_Subdevice_Logout
    64  IOT_Gateway_Yield
    65  IOT_Gateway_Subscribe
    66  IOT_Gateway_Unsubscribe
    67  IOT_Gateway_Publish
    68  IOT_Gateway_RRPC_Register
    69  IOT_Gateway_RRPC_Response



### **必选API**

| 序号  | 函数名                       | 说明                                                               |
|-------|------------------------------|--------------------------------------------------------------------|
|  1    | IOT_OpenLog                  | 开始打印日志信息(log), 接受一个const char *为入参, 表示模块名字    |
|  2    | IOT_SetLogLevel              | 设置打印的日志等级, 接受入参从1到5, 数字越大, 打印越详细           |
|  3    | IOT_CloseLog                 | 停止打印日志信息(log), 入参为空                                    |
|  4    | IOT_DumpMemoryStats          | 调试函数, 打印内存的使用统计情况, 入参为1-5, 数字越大, 打印越详细  |

### **CoAP功能相关**

| 序号  | 函数名                       | 说明                                                                           |
|-------|------------------------------|--------------------------------------------------------------------------------|
|  1    | IOT_CoAP_Init                | CoAP实例的构造函数, 入参为`iotx_coap_config_t`结构体, 返回创建的CoAP会话句柄   |
|  2    | IOT_CoAP_Deinit              | CoAP实例的摧毁函数, 入参为`IOT_CoAP_Init()`所创建的句柄                        |
|  3    | IOT_CoAP_DeviceNameAuth      | 基于控制台申请的`DeviceName`, `DeviceSecret`, `ProductKey`做设备认证           |
|  4    | IOT_CoAP_GetMessageCode      | CoAP会话阶段, 从服务器的`CoAP Response`报文中获取`Respond Code`                |
|  5    | IOT_CoAP_GetMessagePayload   | CoAP会话阶段, 从服务器的`CoAP Response`报文中获取报文负载                      |
|  6    | IOT_CoAP_SendMessage         | CoAP会话阶段, 连接已成功建立后调用, 组织一个完整的CoAP报文向服务器发送         |
|  7    | IOT_CoAP_Yield               | CoAP会话阶段, 连接已成功建立后调用, 检查和收取服务器对`CoAP Request`的回复报文 |

### ** 云端连接Cloud Connection功能相关**

| 序号  | 函数名                       | 说明                                                                           |
|-------|------------------------------|--------------------------------------------------------------------------------|
|  1    | IOT_Cloud_Connection_Init    | 云端连接实例的构造函数, 入参为`iotx_cloud_connection_param_pt`结构体, 返回创建的云端连接会话句柄   |
|  2    | IOT_Cloud_Connection_Deinit  | 云端连接实例的摧毁函数, 入参为`IOT_Cloud_Connection_Init()`所创建的句柄                        |
|  3    | IOT_Cloud_Connection_Send_Message      | 发送数据给云端           |
|  4    | IOT_Cloud_Connection_Yield   | 云端连接成功建立后，收取服务器发送的报文                |

### ** CMP功能相关**

| 序号  | 函数名                       | 说明                                                                           |
|-------|------------------------------|--------------------------------------------------------------------------------|
|  1    | IOT_CMP_Init                 | CMP实例的构造函数, 入参为`iotx_cmp_init_param_pt`结构体，只存在一个CMP实例   |
|  2    | IOT_CMP_Register             | 通过CMP订阅服务                                              |
|  3    | IOT_CMP_Unregister           | 通过CMP取消服务订阅 |
|  4    | IOT_CMP_Send                 | 通过CMP发送数据，可以送给云端，也可以送给本地设备|
|  1    | IOT_CMP_Send_Sync            | 通过CMP同步发送数据   ，暂不支持|
|  2    | IOT_CMP_Yield                | 通过CMP接收数据，单线程情况下才支持                        |
|  3    | IOT_CMP_Deinit               | CMP示例的摧毁函数           |
|  4    | IOT_CMP_OTA_Yield            | 通过CMP完成OTA功能                                                               |


### **MQTT功能相关**

| 序号  | 函数名                       | 说明                                                                           |
|-------|------------------------------|--------------------------------------------------------------------------------|
|  1    | IOT_SetupConnInfo            | MQTT连接前的准备, 基于`DeviceName + DeviceSecret + ProductKey`产生MQTT连接的用户名和密码等 |
|  2    | IOT_MQTT_CheckStateNormal    | MQTT连接后, 调用此函数检查长连接是否正常                                       |
|  3    | IOT_MQTT_Construct           | MQTT实例的构造函数, 入参为`iotx_mqtt_param_t`结构体, 连接MQTT服务器, 并返回被创建句柄 |
|  4    | IOT_MQTT_Destroy             | MQTT实例的摧毁函数, 入参为`IOT_MQTT_Construct()`创建的句柄                     |
|  5    | IOT_MQTT_Publish             | MQTT会话阶段, 组织一个完整的`MQTT Publish`报文, 向服务端发送消息发布报文       |
|  6    | IOT_MQTT_Subscribe           | MQTT会话阶段, 组织一个完整的`MQTT Subscribe`报文, 向服务端发送订阅请求         |
|  7    | IOT_MQTT_Unsubscribe         | MQTT会话阶段, 组织一个完整的`MQTT UnSubscribe`报文, 向服务端发送取消订阅请求   |
|  8    | IOT_MQTT_Yield               | MQTT会话阶段, MQTT主循环函数, 内含了心跳的维持, 服务器下行报文的收取等         |

### **OTA功能相关(模组实现时的可选功能)**

| 序号  | 函数名                       | 说明                                                                                   |
|-------|------------------------------|----------------------------------------------------------------------------------------|
|  1    | IOT_OTA_Init                 | OTA实例的构造函数, 创建一个OTA会话的句柄并返回                                         |
|  2    | IOT_OTA_Deinit               | OTA实例的摧毁函数, 销毁所有相关的数据结构                                              |
|  3    | IOT_OTA_Ioctl                | OTA实例的输入输出函数, 根据不同的命令字可以设置OTA会话的属性, 或者获取OTA会话的状态    |
|  4    | IOT_OTA_GetLastError         | OTA会话阶段, 若某个`IOT_OTA_*()`函数返回错误, 调用此接口可获得最近一次的详细错误码     |
|  5    | IOT_OTA_ReportVersion        | OTA会话阶段, 向服务端汇报当前的固件版本号                                              |
|  6    | IOT_OTA_FetchYield           | OTA下载阶段, 在指定的`timeout`时间内, 从固件服务器下载一段固件内容, 保存在入参buffer中 |
|  7    | IOT_OTA_IsFetchFinish        | OTA下载阶段, 判断迭代调用`IOT_OTA_FetchYield()`是否已经下载完所有的固件内容            |
|  8    | IOT_OTA_IsFetching           | OTA下载阶段, 判断固件下载是否仍在进行中, 尚未完成全部固件内容的下载                    |
|  9    | IOT_OTA_ReportProgress       | 可选API, OTA下载阶段, 调用此函数向服务端汇报已经下载了全部固件内容的百分之多少         |

### **HTTP功能相关**

| 序号  | 函数名                       | 说明                                                                                   |
|-------|------------------------------|----------------------------------------------------------------------------------------|
|  1    | IOT_HTTP_Init                | Https实例的构造函数, 创建一个HTTP会话的句柄并返回                                      |
|  2    | IOT_HTTP_DeInit              | Https实例的摧毁函数, 销毁所有相关的数据结构                                            |
|  3    | IOT_HTTP_DeviceNameAuth      | 基于控制台申请的`DeviceName`, `DeviceSecret`, `ProductKey`做设备认证                   |
|  4    | IOT_HTTP_SendMessage         | Https会话阶段, 组织一个完整的HTTP报文向服务器发送,并同步获取HTTP回复报文               |
|  5    | IOT_HTTP_Disconnect          | Https会话阶段, 关闭HTTP层面的连接, 但是仍然保持TLS层面的连接                           |

### **设备影子相关(模组实现时的可选功能)**

| 序号  | 函数名                          | 说明                                                                            |
|-------|---------------------------------|---------------------------------------------------------------------------------|
|  1    | IOT_Shadow_Construct            | 建立一个设备影子的MQTT连接, 并返回被创建的会话句柄                              |
|  2    | IOT_Shadow_Destroy              | 摧毁一个设备影子的MQTT连接, 销毁所有相关的数据结构, 释放内存, 断开连接          |
|  3    | IOT_Shadow_Pull                 | 把服务器端被缓存的JSON数据下拉到本地, 更新本地的数据属性                        |
|  4    | IOT_Shadow_Push                 | 把本地的数据属性上推到服务器缓存的JSON数据, 更新服务端的数据属性                |
|  5    | IOT_Shadow_Push_Async           | 和`IOT_Shadow_Push()`接口类似, 但是异步的, 上推后便返回, 不等待服务端回应       |
|  6    | IOT_Shadow_PushFormat_Add       | 向已创建的数据类型格式中增添成员属性                                            |
|  7    | IOT_Shadow_PushFormat_Finalize  | 完成一个数据类型格式的构造过程                                                  |
|  8    | IOT_Shadow_PushFormat_Init      | 开始一个数据类型格式的构造过程                                                  |
|  9    | IOT_Shadow_RegisterAttribute    | 创建一个数据类型注册到服务端, 注册时需要`*PushFormat*()`接口创建的数据类型格式  |
| 10    | IOT_Shadow_DeleteAttribute      | 删除一个已被成功注册的数据属性                                                  |
| 11    | IOT_Shadow_Yield                | MQTT的主循环函数, 调用后接受服务端的下推消息, 更新本地的数据属性                |

### **主子设备相关(模组实现时的可选功能)**

| 序号  | 函数名                          | 说明                                                                            |
|-------|---------------------------------|---------------------------------------------------------------------------------|
|  1    | IOT_Gateway_Construct           | 建立一个主设备，建立MQTT连接, 并返回被创建的会话句柄                            |
|  2    | IOT_Gateway_Destroy             | 摧毁一个主设备的MQTT连接, 销毁所有相关的数据结构, 释放内存, 断开连接            |
|  3    | IOT_Subdevice_Login             | 子设备上线，通知云端建立子设备session                                           |
|  4    | IOT_Subdevice_Logout            | 子设备下线，销毁云端建立子设备session及所有相关的数据结构, 释放内存             |
|  5    | IOT_Gateway_Yield               | MQTT的主循环函数, 调用后接受服务端的下推消息                                    |
|  6    | IOT_Gateway_Subscribe           | 通过MQTT连接向服务端发送订阅请求                                                |
|  7    | IOT_Gateway_Unsubscribe         | 通过MQTT连接向服务端发送取消订阅请求                                            |
|  8    | IOT_Gateway_Publish             | 通过MQTT连接服务端发送消息发布报文                                              |
|  9    | IOT_Gateway_RRPC_Register       | 注册设备的RRPC回调函数，接收云端发起的RRPC请求                                  |
| 10    | IOT_Gateway_RRPC_Response       | 对云端的RRPC请求进行应答                                                        |

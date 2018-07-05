# 目录
  * [1 概要](#1-概要)
  * [2 接口实现](#2-接口实现)
    * [2.1 修改设备凭证](#21-修改设备凭证)
    * [2.2 修改TSL信息](#22-修改tsl信息)
    * [2.3 linkkit函数接口](#23-linkkit函数接口)
        * [2.3.1 linkkit服务程序初始化](#231-linkkit服务程序初始化)
        * [2.3.2 创建对象](#232-创建对象)
        * [2.3.3 事件分发处理](#233-事件分发处理)
        * [2.3.4 设置对象的TSL属性](#234-设置对象的tsl属性)
        * [2.3.5 上报设备属性到云端](#235-上报设备属性到云端)
        * [2.3.6 上报事件到云端](#236-上报事件到云端)
  * [3 用例编译](#3-用例编译)
  * [4 用例执行](#4-用例执行)

------
# 1 概要
  使用linkkit接入阿里的 [智能生活开放平台](https://living.aliyun.com/#/)前先确认以下功能已经具备：
1. 设备能够连接到阿里的 [智能生活开放平台](https://living.aliyun.com/#/)
2. 已经通过[智能生活开放平台](https://living.aliyun.com/#/)获取到了设备的激活码，并得到设备联网需要的`ProductKey`、`ProductSecret`、`DeviceName`、`DeviceSecret`。
3. 根据设备的功能定义，获取到物的模型（TSL），即一段json格式的描述信息。

  使用linkkit需要进行如下的几步：
1. 修改设备凭证：`ProductKey`、`ProductSecret`、`DeviceName`、`DeviceSecret`。
2. 设置TSL信息。
3. 实现linkkit函数接口：
    * linkkit服务程序初始化: `linkkit_start()`和`linkkit_ops_t`结构体中的回调函数
        * `on_connect()`
        * `on_disconnect()`
        * `raw_data_arrived()`
        * `thing_create()`
        * `thing_enable()`
        * `thing_disable()`
        * `thing_call_service()`
        * `thing_prop_changed()`
4. 其他会用到的linkkit函数：
    * 事件分发处理： `linkkit_dispatch()`
    * 创建物的对象： `linkkit_set_tsl()`
    * 设置对象的TSL属性： `linkkit_set_value()`
    * 上报设备属性到云端： `linkkit_post_property()`
    * 上报事件到云端：  `linkkit_trigger_event()`
------
# 2 接口实现
## 2.1 修改设备凭证
根据云端产生的设备凭证，修改头文件`framework/protocol/linkkit/iotkit/sdk-encap/imports/iot_import_product.h`下的设备凭证信息。

```c
#define PRODUCT_KEY             "a1BDCKKSpdu"
#define PRODUCT_SECRET          "Rp0aCxyRnd2ecrCX"
#define DEVICE_NAME             "MVwo1x7hB2pyEbXlLkzE"
#define DEVICE_SECRET           "OKHWuIaMzwtJAjUzPRLG1rcYxFo43akU"
```
## 2.2 修改TSL信息

在产品功能定义完成后，平台将自动生成JSON格式的TSL（即 Thing Specification Language），用来描述物的模型。该信息经过“压缩并转义”后，转换成C语言下的json格式，这样可以添加到设备端的TSL信息中。“压缩并转义”的方式可以参考链接：https://www.sojson.com/yasuo.html


在创建物的对象时，需要转义后的TSL信息，其修改可以参考文件：`example/linkkitapp/linkkit_app.c`中的`const char TSL_STRING[]`的定义。

## 2.3 linkkit函数接口
linkkit服务例程初始化需要在配网完成之后，建议注册在事件`CODE_WIFI_ON_GOT_IP`里。linkkit示例分为以下步骤顺序完成：linkkit服务程序初始化，进入事件分发处理，创建对象，设置物对象的TSL属性、上报云端。后面会介绍该过程，具体请参考（`aos\example\linkkitapp`下的用例代码，入口函数为`linkkit_main()`）
### 2.3.1 linkkit服务程序初始化

| 名称 | 描述 |
| :------------ | :-------- |
| 函数 | int linkkit_start(int max_buffered_msg, int get_tsl_from_cloud, linkkit_loglevel_t log_level, linkkit_ops_t *ops, linkkit_cloud_domain_type_t domain_type, void *user_context) |
| 说明 | 启动 linkkit 服务，与云端建立连接并安装回调函数 |
| 返回值| 0: linkkit 服务程序启动成功;  -1: linkkit 服务程序启动失败|

| 参数 | 类型 | 描述 | 备注 |
| :------------ | :-------- | :-------- | :-------- |
|max_buffered_msg | int |消息队列大小 |此参数影响事件消息最大暂存数，频繁业务可适当增大此参数，建议值 [16~32]|
|get_tsl_from_cloud |int |获取 TSL 路径 |0: 从本地获取物模型，非0: 从云端获取物模型|
| log_level |linkkit_loglevel_t| 设置打印的日志等级| [0~5]: 数字越大, 打印越详细|
| ops |linkkit_ops_t* | linkkit 事件回调函数| linkkit 通过回调函数将事件通知给用户 |
| domain_type |linkkit_cloud_domain_type_t| 云服务器域名 |linkkit_cloud_domain_sh: 上海域名linkkit_cloud_domain_usa: 美国域名|
| user_context | void* |用户上下文环境指针 |用户自定义结构体的指针, 回调函数中传出 |

事件类型以及对应的事件回调函数如下所示,没当linkkit处理完相应的事件后，都会执行用户注册的回调函数，具体实现可参考linkkit_app.c内的实现：

```c
typedef enum {
    dm_callback_type_property_value_set = 0,
    dm_callback_type_service_requested,
#ifdef RRPC_ENABLED
    dm_callback_type_rrpc_requested,
#endif /* RRPC_ENABLED */
    dm_callback_type_cloud_connected,
    dm_callback_type_cloud_disconnected,
    dm_callback_type_new_thing_created,
    dm_callback_type_thing_disabled,
    dm_callback_type_thing_enabled,
    dm_callback_type_raw_data_arrived,

    dm_callback_type_number,
} dm_callback_type_t;

static linkkit_ops_t alinkops = {
    .on_connect         = on_connect,
    .on_disconnect      = on_disconnect,
    .raw_data_arrived   = raw_data_arrived,
    .thing_create       = thing_create,
    .thing_enable       = thing_enable,
    .thing_disable      = thing_disable,
    .thing_call_service = thing_call_service,
    .thing_prop_changed = thing_prop_changed,
};
```

### 2.3.2 创建对象
根据设备描述信息（tsl），创建物的对象，并添加到linkkit中。如果创建成功，会返回物的唯一标志符，后续只需要通过该标识符来管理对象。

| 名称 | 描述 |
| :------------ | :-------- |
| 函数 | void* linkkit_set_tsl(const char* tsl, int tsl_len) |
| 说明 | 从本地读取 TSL 文件,生成物的对象并添加到 linkkit 中 |
| 返回值| 0: 成功;  -1: 物的唯一标识符|

| 参数 | 类型 | 描述 | 备注 |
| :------------ | :-------- | :-------- | :-------- |
| tsl | const char* | 设备描述信息( JSON 类型) |tsl 必须和设备三元组所属的产品的 Alink JSON 格式协议相同|
| tsl_len | int | 设备描述信息长度 | - |

代码示例
```c
int linkkit_main()
{
    sample_context_t* sample_ctx = &g_sample_context;
    int execution_time = 0;
    int get_tsl_from_cloud = 0;

    execution_time = execution_time < 1 ? 1 : execution_time;

    memset(sample_ctx, 0, sizeof(sample_context_t));
    sample_ctx->thing_enabled = 1;
    linkkit_start(16, get_tsl_from_cloud, linkkit_loglevel_debug, &alinkops, linkkit_cloud_domain_sh, sample_ctx);
    if (!get_tsl_from_cloud) {
        linkkit_set_tsl(TSL_STRING, strlen(TSL_STRING));
    }
	
    aos_post_delayed_action(1000, linkkit_action, sample_ctx);
    return 0;
}
```

### 2.3.3 事件分发处理
linkkit服务程序初始化之后，需要周期性地进行进行事件分发处理，建议放到任务或delay事件中执行。该函数会根据事件回调的类型来执行相应的回调函数，linkkit_app.c的用例代码中已默认调用了该接口。

| 名称 | 描述 |
| :------------ | :-------- |
| 函数 | int linkkit_dispatch() |
| 说明 | 事件分发函数,触发linkkit_start安装的回调 |
| 返回值| 0: 成功;  -1: 失败|

代码示例
```c
void linkkit_action(void *params)
{
    int ret;
    sample_context_t* sample_ctx = params;

    linkkit_dispatch();

    aos_post_delayed_action(200, linkkit_action, sample_ctx);
}
```
### 2.3.4 设置对象的TSL属性
设置TSL属性，就是给已创建的对象的TSL属性赋值，赋值完成后，可以通过接口`linkkit_post_property`将对象属性上报到云端，也可以通过`linkkit_trigger_event`将事件上报云端。

| 名称 | 描述 |
| :------------ | :-------- |
| 函数 | int linkkit_set_value(linkkit_method_set_t method_set, const void* thing_id, const char* identifier, const void* value, const char* value_str) |
| 说明 | 根据identifier设置物对象的 TSL 属性，如果标识符为struct类型、event output类型或者service output类型，使用点'.'分隔字段；例如"identifier1.identifier2"指向特定的项 |
| 返回值| 0: 物对象 TSL 属性设置成功;  -1: 物对象 TSL 属性设置失败|

| 参数 | 类型 | 描述 | 备注 |
| :---- | :--------  | :---------- | :-------------------- |
|method_set |linkkit_method_set_t|设置 TSL 属性的类型|linkkit_method_set_property_value:设置模型property valuelinkkit_method_set_event_output_value:设置物模型event output valuelinkkit_method_set_service_output_value:设置物模型service output value|
|thing_id | const void* | 物的唯一标识符 |物对象的标识符,可从 thing_create 回调获得linkkit创建的 thing id |
|identifier |const char* |TSL 属性标示符 |物对象 TSL 属性的标识符 |
| value |const void* | 输入的属性值 |和 value_str, 二选一必填 |
| value_str | const char* | 输入的字符串格式的属性值 | 和 value, 二选一必填 |


### 2.3.5 上报设备属性到云端

| 名称 | 描述 |
| :------------ | :--------------------------- |
| 函数 | int linkkit_post_property(const void* thing_id, const char* property_identifier) |
| 说明 | 上报设备属性到云端 |
| 返回值| 0: 上报成功;  -1: 上报失linkkit_post_property败|

| 参数 | 类型 | 描述 | 备注 |
| :---- | :-------- | :---------- | :-------------------- |
|thing_id | const void* | 物的唯一标识符 |物对象的标识符,可从 thing_create 回调获得 linkkit 创建的 thing id |
| property_identifier | const char* | 属性标示符 | 上报的物对象的TSL属性标识符 |

物对象的TSL标识符请参考物的TSL字符串或下图中的“标识符”列
![](https://i.imgur.com/PPX37jK.png)

```c
int linkkit_data_publish(const char* identifier, const void* value, const char* value_str)
{
    int ret = 0;
    ret = linkkit_set_value(linkkit_method_set_property_value,g_thing_id,identifier,value,value_str);
    if(0 != ret) {
        return ret;
    }
    ret = linkkit_post_property(g_thing_id,identifier);
    return ret;
}

int data_publish_test()
{
    int ret = 0;
	
    /*温度值为24.6*/
    ret = linkkit_data_publish("CurrentTemperature",NULL,"24.6");
    if(0 != ret) {
        return ret;
    }
	/*湿度值为43%*/
	ret = linkkit_data_publish("CurrentHumidity",NULL,"43");
    if(0 != ret) {
        return ret;
    }
	return 0;
}
```
### 2.3.6 上报事件到云端

`linkkit_set_value`的第一个参数值选择`linkkit_method_set_event_output_value`，则可以设置完事件（event）的TSL信息，之后可以通过`linkkit_trigger_event`将事件发送到云端。

| 名称 | 描述 |
| :------------ | :--------------------------- |
| 函数 | int linkkit_trigger_event(const void* thing_id, const char* event_identifier) |
| 说明 | 上报设备事件到云端 |
| 返回值| 0: 上报成功;  -1: 上报失败 |

| 参数 | 类型 | 描述 | 备注 |
| :---- | :-------- | :---------- | :-------------------- |
|thing_id | const void* | 物的唯一标识符 |物对象的标识符,可从 thing_create 回调获得 linkkit创建的thing id |
|event_identifier | const char* | 事件标示符 | 上报的事件标识符 |

设备端详细的API使用请参考文档：https://living.aliyun.com/doc#cusllk.html

------
## 3 用例编译
编译前确保`aos\example\linkkitapp`目录下面为修改过的用例（参考第3章介绍）
编译命令为：

```sh
aos make linkkitapp@mk3060
```

> 其中 linkkitapp 为用例名称；mk3060为单板类型


编译后生成的文件为`aos\out\linkkitapp@mk3060\binary\linkkitapp@mk3060.ota.bin`

------
## 4 用例执行
用原始的linkkitapp可执行文件配网成功后，然后替换新的用例执行，在云端可以看到设备已经是在线状态
![](https://i.imgur.com/1e9kOBU.png)
而当设备端上传数据成功后，可以在云端看到设备上传的数据
![](https://i.imgur.com/bKbt6I8.png)


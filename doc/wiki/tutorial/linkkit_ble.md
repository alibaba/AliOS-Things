文档提供了AliOS-Things Linkit协议规范，详细介绍了蓝牙设备发现，设备认证和设备接入云端的数据格式要求和交互流程，实例可直接参阅<Linkkit对接移植事例>部分.
## 蓝牙设备发现
### 广播数据类型
蓝牙广播格式遵循BT 5.0/4.2广播通道数据格式规范，广播数据包MTU为31 Bytes,由若干 AD Structure 组成(参见 Bluetooth 5.0 Core Specification, Volume 3, Part C, Chapter 11)，每一个 AD Structure 结构由 Length、AD Type、AD Data 组成。如下图所示:

![](https://img.alicdn.com/tfs/TB1Kk_IuMmTBuNjy1XbXXaMrVXa-1580-846.png)

## 设备认证和接入
### Linkkit服务接口
Linkkit在BLE协议栈基础上提供了接入云端的API，类RESTFUL形式，定义在 $(aos_root_path)/framework/bluetooth/ais_ilop/ble_app_ali/api_export.h中,
* typedef void (*dev_status_changed_cb) (alink_event_t event);
  * 客户需要**实现**此函数，SDK回调此函数。状态通知：当DEVICE发生状态变化，如蓝牙连接事件，如果客户有LED等状态的更新就在此函数中实现
* typedef void (*set_dev_status_cb) (uint8_t *buffer, uint32_t length);
  * 客户需要**实现**此函数。当APP有控制指令发给客户端时，AIS会调用此函数
* typedef void (*get_dev_status_cb) (uint8_t *buffer, uint32_t length);
  * 客户需要**实现**此函数。当APP需要获取客户端状态时，AIS会调用此函数
* int alink_start(struct device_config *dev_conf);
  * 客户需要调用此接口，启动AIS服务
* int alink_end(void);
  * 客户需要调用此接口，停止AIS服务
* void alink_post(uint8_t *buffer, uint32_t length);
  *客户需要调用此接口，一旦客户有数据更新，需要通过此函数发送给服务器，Callback 函数中通知调用结果，蓝牙采用indicate 的方式post数据，非阻塞
* void alink_post_fast(uint8_t *buffer, uint32_t length);
  * 客户需要调用此接口，一旦客户有数据更新，需要通过此函数发送给服务器,callback 函数中通知调用结果，蓝牙采用notify 的方式post数据，非阻塞
* device_config结构体初始化
  * 客户需要初始化这个结构体，结构体定义如下：
![](https://img.alicdn.com/tfs/TB11wFdu7yWBuNjy0FpXXassXXa-920-746.png)

### Linkkit对接移植事例

如果客户需要对接私有BLE协议栈至Linkkit，需要:
* 调用蓝牙广播接口蓝牙广播参数和数据
* 提供适配Linkkit的BLE协议栈GAP/GATT接口
* 实现Linkkit需要的状态回调函数

在github上下载代码:https://github.com/alibaba/AliOS-Things ，参考AliOS-things目录下example/bluetooth/aisilopapp应用，该事例通过调用实现的Linkkit服务接口，实现了符合Linkkit设备发现，接入和认证过程和服务器通信,具体如下:

#### 蓝牙广播对接
需要在使能BLE广播之前设置广播参数，需要对接客户的GAP API：
* 广播参数设置接口
* 广播数据设置接口
* 广播使能接口（Aos的_bt_le_adv_start()_）

原AOS协议栈广播参数设置：
<pre><code>static struct bt_data ad[3] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0x0a, 0x18),
};

static const struct bt_data sd[] = {
    BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, \
            sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};
</code></pre>


开启广播:
<pre><code>
ad[2].type = 0xFF;                        
ad[2].data_len = length;
ad[2].data = manuf_spec_data_raw;
err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad),
                          sd, ARRAY_SIZE(sd));
</code></pre>

### 蓝牙协议栈GATT/ATT对接
Linkkit服务需要蓝牙协议栈提供以下GATT APIs以便在Linkkit初始化的时候注册,由于默认蓝牙设备端是GATT central,需要对client(手机端)请求(WRITE/READ)通过Notify/Indication回复响应:

* `void bt_conn_cb_register(struct bt_conn_cb *cb);`
* `int bt_conn_disconnect(struct bt_conn *conn, u8_t reason);`
* `int bt_gatt_service_register(struct bt_gatt_service *svc);`
* `int bt_gatt_notify(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                   const void *data, u16_t len);`
* `int bt_gatt_indicate(struct bt_conn *conn,
                     struct bt_gatt_indicate_params *params);`



### Linkkit服务对接
初始化device_config结构体
<code><pre>
    static void ali_lib_init(void)
    {
    ...
    struct device_config init_alink;
    ...
    memset(&init_alink, 0, sizeof(struct device_config));
    init_alink.product_id = 127678;
    init_alink.status_changed_cb = dev_status_changed_handler;
    init_alink.set_cb            = set_dev_status_handler;
    init_alink.get_cb            = get_dev_status_handler;
#ifdef CONFIG_AIS_OTA
    init_alink.enable_ota        = true;
#endif
    init_alink.enable_auth       = true;
    init_alink.auth_type         = ALI_AUTH_BY_PRODUCT_SECRET;
    memcpy(init_alink.bd_addr, bd_addr, BD_ADDR_LEN);                                       

    init_alink.secret_len        = strlen("gjeRzfwWcMg8Rw036OjsPEreGPDMl1wI");
    memcpy(init_alink.secret, "gjeRzfwWcMg8Rw036OjsPEreGPDMl1wI", init_alink.secret_len);

    init_alink.product_key_len   = strlen("a13wpkvcQDQ");
    memcpy(init_alink.product_key, "a13wpkvcQDQ", init_alink.product_key_len);

    init_alink.device_key_len    = strlen("mSLKAvSYdQz5JsshCyTE");
    memcpy(init_alink.device_key, "mSLKAvSYdQz5JsshCyTE", init_alink.device_key_len);
    memcpy(init_alink.version, SOFTWARE_VERSION, SOFTWARE_VERSION_LEN);
    ret = alink_start(&init_alink);
    ...
}

</code></pre>

其中：
**init_alink.product_id， init_alink.secret，init_alink.product_key，init_alink.device_key** 对应产品的三元组序列信息需要参考物联网接入安全认证相关文档或者联系阿里IoT团队获取以便更新。

实现和Linkkit对接API:

<code><pre>
static void dev_status_changed_handler(alink_event_t event);
static void set_dev_status_handler(uint8_t * buffer, uint32_t length);
static void get_dev_status_handler(uint8_t * buffer, uint32_t length);
</code></pre>


完成以上工作后，就可以按照aisilopapp的事例进行和云端对接，数据推送交互。




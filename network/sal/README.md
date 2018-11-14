# AliOS Things SAL Porting Guide

在AliOS Things移植过程中，如果需要支持外接Wifi/BLE等模，且TCP/IP协议栈运行模组侧；则需要SAL和底层模组控制模块进行对接。SAL功能对上层提供标准socket接口，使上层应用不感知TCP/IP协议栈运行在MCU侧还是通讯模组侧。AliOS Things SAL的接口定义请查看头文件：[sal.h](https://github.com/alibaba/AliOS-Things/blob/master/device/sal/include/sal.h)。

本文将讲述AliOS Things 中SAL移植要点。

## 1、SAL模块重要数据结构

首先，先了解一下AliOS Things中跟SAL相关的两个个重要的数据结构: `sal_op_t`和`sal_conn_t`两个结构体。SAL依赖底层的相关操作和接口封装都在`sal_op_t`这个结构体中；`sal_conn_t`结构体为建立连接时，SAL传给底层模组控制模块的相关参数。两个结构体相关定义在文件[sal.h](https://github.com/alibaba/AliOS-Things/blob/master/device/sal/include/sal.h)中。

```
typedef struct sal_op_s {
    char *version; 

    int (*init)(void);

    int (*start)(sal_conn_t *c);
    
    int (*send)(int fd, uint8_t *data, uint32_t len,
                char remote_ip[16], int32_t remote_port);
                
    int (*domain_to_ip)(char *domain, char ip[16]);
    
    int (*close)(int fd, int32_t remote_port);
    
    int (*deinit)(void);
    
    int (*register_netconn_data_input_cb)(netconn_data_input_cb_t cb);
} sal_op_t;
```

```
typedef struct {
    int fd; 
    CONN_TYPE type;
    char *addr; 
    int32_t r_port; 
    int32_t l_port; 
    uint32_t tcp_keep_alive;
} sal_conn_t;
```

## 2、SAL接口的实现

在具体的平台移植过程中，用户需要分别实现SAL模块结构体中对应的接口函数。AliOS Things对SAL层接口有一层封装，参见`device/sal/sal.c`文件。具体的接口实现一般在`device/sal/xxx/`中，其中`xxx`代表模组类型。参考实现：[mk3060.c](https://github.com/alibaba/AliOS-Things/blob/master/device/sal/wifi/mk3060/mk3060.c)。下面对每个接口做一些说明：

### `init`

该接口需要对通信模组进行相关初始化，使通信模组达到可以工作的状态。

### `deinit`

如果有需要，该接口需要提供对通信模组的去初始化操作。

### `start`

该接口需要模组启动一次连接。SAL传给底层的参数为一个结构体指针`sal_conn_t`，该结构体参数说明如下：

>  fd: 每个连接对应SAL socket层的句柄
>
>  type: 建立连接的类型，数据定义参见枚举`CONN_TYPE`
>
>  addr: 对端ip或者域名，例如“192.168.1.1”或者“www.taobao.com”
>
>  r_port: 对端端口号
>
>  l_port: 本地端口号
>
>  tcp_keep_alive: tcp keep alive的时间

因此底层模组控制模块需要维护一套SAL socket 句柄和底层链路的对应关系。可以在发送/关闭连接时可以通过SAL socket句柄查找到对应的底层连接；在接收底层数据时可以根据底层连接找到对应的SAL socket句柄。

### `close`

该接口关闭模组的一个连接。入参说明如下：

> fd: 需要关闭的socket句柄，
>
> remote_port: 对端端口号，该参数为可选参数，小于0时为无效参数

### `send`

该接口通过模块发送数据的接口，该接口为阻塞接口，直到模组通知底层控制模块数据发送成功才会返回。入参说明如下：

> fd: 发送数据所操作的句柄
>
> data: 待发送数据的指针
>
> len: 待发送数据的长度
>
> remote_ip[16], 对端ip地址，该参数为可选参数，入参为NULL时无效
>
> remote_port: 对端端口号，该参数为可选参数，小于0时为无效参数

### `domain_to_ip`

该接口提供获取对应域名ip地址的功能，注意：1、即使该域名对应多个ip，也只会返回一个Ip地址；2、目前该接口只需要支持ipv4。入参说明如下：

> domain: 域名信息
>
> ip[16]: 点格式的ip字符串，目前只支持ipv4，例如:192.168.111.111

### `register_netconn_data_input_cb`

该接口提供一个SAL数据接收函数的回调功能，底层模组控制模块在收到数据后，调用该接口上送到SAL中，SAL会在其中对每个句柄的数据进行管理。SAL提供的数据上送接口实现请见`sal_packet_input`。

> SAL提供的数据上送接口回调：typedef int (*netconn_data_input_cb_t)(int fd, void *data, size_t len, ip_addr_t *addr, u16_t port);
>
> 参数说明如下：
>
> fd: 数据上送需要操作的句柄
>
> data: 接收到的数据（该部分内存由底层自行释放）
>
> len: 接收到的数据长度
>
> addr: 该数据的源地址，为可选参数，可以传入NULL（该部分内存由底层自行释放）
>
> port: 该数据的源端口，为可选参数，可以传入0

## 3、模组注册和初始化SAL

在完成SAL接口对接实现后，定义一个 `sal_op_t`结构体，将各个接口和回调的实现地址赋值给结构体中对应的域。例如：

```
sal_op_t sal_op = {
    .version = "1.0.0",
    .init = sal_wifi_init,
    .start = sal_wifi_start,
    .send = sal_wifi_send,
    .domain_to_ip = sal_wifi_domain_to_ip,
    .close = sal_wifi_close,
    .deinit = sal_wifi_deinit,
    .register_netconn_data_input_cb = sal_wifi_packet_input_cb_register,
};
```

底层模组控制模块需要实现一个函数调用`sal_module_register`对SAL进行模块注册。例如：

```
int mk3060_sal_init(void)
{
    return sal_module_register(&sal_op);
}
```

该模块注册函数需要在[device.c](https://github.com/alibaba/AliOS-Things/blob/master/device/sal/sal_device.c)的`sal_device_init`中进行调用。例如：

```
int sal_device_init()
{
    int ret = 0;

#ifdef DEV_SAL_MK3060
    ret = mk3060_sal_init();
#endif
    if (ret){
        LOGE(TAG, "device init fail ret is %d\n", ret);
    }
    
    return ret;
}

```

## 4、编译底层模组控制模块

在完成底层模组与SAL接口对接后，该部分代码建议的放置路径为`device/sal/xxx/yyy`。其中`xxx`为模组类型，例如wifi、ble、lora等；`yyy`为模组型号例如：mk3060。例如：mk3060的wifi模组代码放置路径为：`device/sal/wifi/mk3060/`。对应模组控制模块代码makefile名称需与模组型号一致为`yyy.mk`，例如：mk3060的makefile文件名为:`mk3060.mk`。

编译时请注意在正常的编译命令后需要指定编译sal和所使用的通信模组信息，例如：`aos make alinkapp@b_l475e sal=1 module=wifi.mk3060`  ，其中`sal=1`表示需要编译sal，`module=wifi.mk3060`

表示所需要连接的模组类型为`wifi.mk3060`，此时AliOS Things的Makefile 体系会自动调用devcie/sal/wifi/mk3060/mk3060.mk 将模组控制模块代码编译进来。




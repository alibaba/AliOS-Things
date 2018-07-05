EN| [中文](AliOS-Things-SAL-Porting-Guide.zh)

# AliOS Things SAL Porting Guide

When migrating AliOS Things, if external modules such as Wifi and BLE are needed and TCP/IP protocol stack runs in module side, SAL and bottom control module needs to be connected. SAL provides a standard socket interface to upper level, making upper applications unaware TCP/IP protocol stack running in MCU side or communication module side. Definition of AliOS Things SAL interfaces can refer to header file: [sal.h](https://github.com/alibaba/AliOS-Things/blob/master/device/sal/include/sal.h).

This article will introduce the porting points of SAL.

## 1、Important data structure in SAL

There are two important data structures associated with SAL in AliOS Things:  `sal_op_t` and `sal_conn_t`. Related operation and interfaces that SAL relies on are encapsulated in `sal_op_t`. `sal_conn_t` is the related parameter that SAL send to bottom control modules when connecting. Definition of this two structure is defined in [sal.h](https://github.com/alibaba/AliOS-Things/blob/master/device/sal/include/sal.h).

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

## 2、Implement of SAL interface

When migrating a specific platform, users need to implement the corresponding interface functions in the SAL. AliOS Things has a layer of encapsulation for SAL interfaces, and you can refer to device/sal/sal.c. The specific interface implementation is generally in device/sal/xxx/, where XXX represents the module type. Reference implementation: [mk3060.c](https://github.com/alibaba/AliOS-Things/blob/master/device/sal/wifi/mk3060/mk3060.c). The following is introduction of each interface :

### `init`

This interface initiates communication modules to reach work mode.

### `deinit`

If needed, the interface can de-initialize communication modules.

### `start`

This interface can start a connection. The parameter that SAL send to the bottom layer is a structure pointer `sal_conn_t`. Parameters of that struct are : 

> fd: connection handles that each connection corresponding to in SAL
>
> type: type of the connection. Data definition is enumerated in `CONN_TYPE`
>
> addr: opposite end IP or domain name, such as "192.168.1.1" or "www.taobao.com"
>
> r_port: opposite end port number
>
> l_port: local port number
>
> tcp_keep_alive : time when  TCP keep alive

So the control module in bottom layer needs to maintain a corresponding relationship between SAL handles and underlying links. When sending or closing connections, you can find the corresponding underlying connections through SAL handles. When receiving data, you can also find corresponding SAL handles based on underlying connections.

### `close`

This module can close connection of modules. Parameters included are :

> fd: socket handles that need to be closed
>
> remote_port: opposite end port number. It is an optional parameter, and it's invalid if less than 0.

### `send`

This interface sends data through modules, and it is a blocking interface. It won't return until underlying control module is informed of success of data sending. 

> fd: handles when sending data
>
> data: pointer to the data to be sent
>
> len: length of data to be sent
>
> remote_ip[16], ip address of opposite side. It is an optional parameter, and it's invalid if it is NULL.
>
> remote_port: opposite end port number. It is an optional parameter, and it's invalid if less than 0.

### `domain_to_ip`

This interface can get corresponding IP address. Note: 1. Even if the domain name can correspond to multiple IP, it will only return to one. 2. At present, the interface only needs IPv4 support. The introduction of the entry is as follows :

> domain: domain information
>
> ip[16]: IP string in point format. It currently only supports IPv4, for example, 192.168.111.111

### `register_netconn_data_input_cb`

This interface can callback SAL data receiving functions. After receiving data, the bottom control module can send them to SAL by calling this interface, and SAL will manage the data of each handle. Interface to send data provided by SAL can refer to `sal_packet_input`。

> Callback of data sending interface provided by SAL：typedef int (*netconn_data_input_cb_t)(int fd, void *data, size_t len, ip_addr_t *addr, u16_t port);
>
> Introduction of data
>
> fd:  handles when sending data
>
> data: received data (memory of this part will be released by the bottom layer) 
>
> len: length of received data 
>
> addr: source address of the data. It is an optional parameter, and can input NULL.
>
> port: source port of that data. It is the source port of that data. It is an optional parameter, and can input 0.

## 3、Module registration and SAL initiation 

After completing SAL porting, you can define a  `sal_op_t` struct and assign the implementation addresses of each interface and callback to its corresponding domain in the structure. For example:

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

Control module in the bottom layer needs to implement a function callback `sal_module_register` to register modules for SAL. For example：

```
int mk3060_sal_init(void)
{
    return sal_module_register(&sal_op);
}

```

This function can be called in `sal_device_init` in [device.c](https://github.com/alibaba/AliOS-Things/blob/master/device/sal/sal_device.c). For example :

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

## 4、Compilation of control module in bottom layer

When porting of SAL interfaces and bottom module has been completed, we suggest you to store this code in `device/sal/xxx/yyy`. `xxx` is module type, such as WiFi, ble, Lora, and `yyy` is module model such as mk3060. For example, mk3060's WiFi module code should be placed in `device/sal/wifi/mk3060/`. 

The name of corresponding control module code makefile needs to be consistent with module model as `yyy.mk`. For example, the makefile file of mk3060 should be named  as `mk3060.mk`.

When compiling, please note that information of SAL and communication module should be specified after normal compilation command. For example, `aos make alinkapp@b_l475e sal=1 module=wifi.mk3060` , where `sal=1` indicates the compilation SAL, and `module=wifi.mk3060`.

The type of module that needs connecting is `wifi.mk3060`. Makefile system of AliOS Things will automatically call  devcie/sal/wifi/mk3060/mk3060.mk to include control module code.
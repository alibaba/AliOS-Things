## simple_mqtt

### Content
- [simple_mqtt](#linkkitapp)
    - [Content](#content)
    - [Overview](#overview)
    - [Requirements](#requirements)
    - [Build](#build)
    - [Run](#run)

### Overview

The simplest sample combination of **AliOS Things** + **Link Develop**, basic mqtt connection.

It initiates the mqtt connection and provide a `pub` command in cli, to publish message to Link Develop, following the corresponding device prototype.
Also, it handles messages from Link Develop, by subscribing to specific topic.

Thus this simple example demonstrate both upstream and downstream flow.

### Requirements

* Register an account on [Link Develop](https://linkdevelop.aliyun.com), then create the device prototype:  
Add two custom attributes, one for upstream, another for downstream, both of them are strings.  
So the final message data params look like below:  
    ```
    "params": {
        "down": "downstream payload",
        "up": "upstream payload"
    }
    ```
![](https://img.alicdn.com/tfs/TB1zaWLJ7KWBuNjy1zjXXcOypXa-2570-1540.png)
* Generate a device according to the prototype, write down the device information: `PRODUCT_KEY`, `PRODUCT_SECRET`, `DEVICE_NAME`, `DEVICE_SECRET`.

### Build

In `simple_mqtt.c`, update these macros according to above information:  
```c
#define PRODUCT_KEY             "b1VxeCKSQeO"
#define DEVICE_NAME             "simple"
#define DEVICE_SECRET           "x9D0k0DjwtYG734unBm0YYsyoMRxgCNR"
```

then build and flash simple_mqtt image for any board, e.g.:  

```sh
aos make simple_mqtt@developerkit
aos upload simple_mqtt@developerkit
```


### Run

Once mqtt ready, like log below:
```
[inf] _TLSConnectNetwork(357):   . Setting up the SSL/TLS structure...
[inf] _TLSConnectNetwork(367):  ok
[inf] _TLSConnectNetwork(402): Performing the SSL/TLS handshake...
[inf] _TLSConnectNetwork(410):  ok
[inf] _TLSConnectNetwork(414):   . Verifying peer X.509 certificate..
[inf] _real_confirm(93): certificate verification result: 0x00
[005598]<V> mqtt service
[005652]<V> subscribe success, packet-id=1
# 
```

Run:

```
# pub [payload]
```

You'll get upstream payload data on Link Develop device log, and receive downstream log if you triggered it.

![](https://img.alicdn.com/tfs/TB1Nvc_JY9YBuNjy0FgXXcxcXXa-2588-1496.png)

serial console log:
```
# pub helloworld-from-my-device
[499130]<V> id: 5, publish msg: {"id":"5","version":"1.0","method":"thing.event.property.post","params":{"up":"helloworld-from-my-device"}}

# [509540]<V> ----
[509540]<V> Topic: '/sys/b1VxeCKSQeO/simple/thing/service/property/set' (Length: 50)
[509550]<V> Payload: '{"method":"thing.service.property.set","id":"64002324","params":{"down":"hi from link develop to device"},"version":"1.0.0"}' (Length: 124)
[509570]<V> ----
```

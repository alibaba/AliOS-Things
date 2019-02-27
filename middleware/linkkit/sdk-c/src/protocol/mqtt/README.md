## Contents

```shell
├── client
│   └── mqtt_client.c
├── CMakeLists.txt
├── Config.in
├── iot.mk
├── iotx_mqtt_config.h
├── iotx_mqtt.h
├── iotx_mqtt_internal.h
├── mqtt.mk
└── MQTTPacket
    ├── MQTTConnectClient.c
    ├── MQTTConnect.h
    ├── MQTTDeserializePublish.c
    ├── MQTTPacket.c
    ├── MQTTPacket.h
    ├── MQTTPublish.h
    ├── MQTTSerializePublish.c
    ├── MQTTSubscribeClient.c
    ├── MQTTSubscribe.h
    ├── MQTTUnsubscribeClient.c
    └── MQTTUnsubscribe.h
```

## Introduction

 Implementation of a deeply customized MQTT client 3.1.1.

### Features

- **Common**.  Fully support for MQTT Client 3.1.1. Minimum memory requirements < 1KB
- **Quick subsciribe**. subscribe effective immediately. 
- **Offline subscirbe**.  subscribe before connect.
- **Compressed topic**. Use md5 to Compress topic string to reduce ram usage.
- **Simple model**. Init mqtt without parameter and no handle is required for use.

### Dependencies

- **hal**. osal and hal to shield different os and hardware
- **infra**. Authentication, net and so on tool set.

## API

Please refer to  [mqtt api](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/MQTT_Provides) for API details.

## Reference
Please refer to  [mqtt connect](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/HTTP_Connect) for example details.


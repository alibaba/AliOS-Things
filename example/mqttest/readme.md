# MQTT 压测使用说明

## 原理
---
1. java客户端
 - java客户端定时向`topic1`发布消息
 - java客户端订阅`topic2`

2. 设备端
 - 设备端订阅`topic3`
 - 收到`topic3`后将payload发布的到`topic4`
 - 设备端只有在收到`topic3`之后才会发布

3. 规则引擎
 - 将`topic1`转发到`topic3`
 - 将`topic4`转发到`topic2`

4. 完整链路：
 - java端`topic1`---->设备端`topic3`,设备端`topic4`--->java端`topic2`
 - java端能够统计发送的`topic1`和订阅的`topic2`数量是否相等

## Usage
---
### 设备端
- linuxhost: `$ mqttest@linuxhost`
- 开发板: 烧录 `mqttest@mk3060.bin`

### java客户端

1. 修改config.properties

```shell
// device name
deviceName=dev_mqtt_pressure_01

// device secret
deviceSecret=DvXBo9foOWMIcWAWi6g76wn7Eu5Qn6UY

// product key
productKey=GJ2uoVqx7ka

// publish topic name, e.g. roductKey/deviceName/data1
pubTopic=data1

// subscribe topic name
subTopic=data2

// pps, in [1, 5]
packagesPerSecond=1

// online/pre/daily, not support now
envirinment=online
```

2. 启动Java SDK 设备
```shell
$ java -jar mqttclient-stress-test-client-java.jar
```
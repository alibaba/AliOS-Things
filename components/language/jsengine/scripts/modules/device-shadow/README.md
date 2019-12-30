# cloud client 设备上云模块

模块让嵌入式设备通过 MQTT 协议，直接连接 Link Developer 平台的上云模块，包括设备的属性上报，事件触发，服务调用

### API 说明：
```

类: deviceShadow

1. bindDevID(config) 绑定设备三元组
  var config = {
    productKey: 'please_input_your_key',
    deviceName: 'please_input_device_name',
    deviceSecret: 'please_input_device_secret',
    domain: 'iot-as-mqtt.cn-shanghai.aliyuncs.com',
    port: 1883
  }

  domain 和 port 是可选项, 默认是上海正式环境

  支持环境如下:
  ${productKey}.iot-as-mqtt.cn-shanghai.aliyuncs.com:1883 //上海正式
  ${productKey}.iot-as-mqtt.ap-southeast-1.aliyuncs.com:1883 //新加坡正式
  ${productKey}.iot-as-mqtt.us-west-1.aliyuncs.com:1883 //美西正式
  ${productKey}.iot-as-mqtt.ap-northeast-1.aliyuncs.com:1883 //日本正式
  ${productKey}.iot-as-mqtt.eu-central-1.aliyuncs.com:1883 //德国正式
  ${productKey}.iot-as-mqtt.us-east-1.aliyuncs.com:1883 //美东正式



2. start(nofity_fn) 启动 deviceShadow 上云模块：
   nofity_fn 为注册的回调函数，启动 deviceShadow 完成时调用，包括成功或者失败
   nofity_fn(err),  err == null 表示连接正常,否则是错误信息
   
3. postProperty(name, value)
  上报属性
  未连云时也可以上报, 设备信息会缓存,等待上云连接成功之后会上报

4. postEvent(name,value)
  上报事件
  未连云时也可以上报, 设备信息会缓存,等待上云连接成功之后会上报

5. addDevSetPropertyNotify(name, ntfy_fn)
  注册云端设置属性的回调处理
  ntfy_fn(val), val云端设定的值

6. addDevCallServiceNotify(name, ntfy_fn) 
  注册云端远程调用服务的回调处理
  cloudCallService(msg, topic), msg是云端下发的信息, topic服务主题

```	
## 示例：
```


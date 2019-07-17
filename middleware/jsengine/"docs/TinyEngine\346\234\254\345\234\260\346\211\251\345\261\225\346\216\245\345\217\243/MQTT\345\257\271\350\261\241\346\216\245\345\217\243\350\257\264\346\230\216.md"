## MQTT对象说明 ##
主要用于设备上云及受云端控制

## 详细API ##

### MQTT.VERSION ###
表示MQTT模块的版本信息
  * String类型

### MQTT.start(config, callback) ###
启动MQTT功能
  * config － Object类型，MQTT配置参数，上云三要素
    * productKey
    * deviceName
    * deviceSecret
  * callback － function(error) {} 类型函数
  * 返回值 － Number类型。 0：正常; 其他：失败

### MQTT.subscribe(topic, callback) ###
订阅主题
  * topic － 需要要订阅的主题，String类型
  * callback － function(msg, topic) {} 类型函数
  * 返回值 － Number类型。 0：成功; 其他：失败

### MQTT.unsubscribe(topic) ###
取消订阅主题
  * topic － 需要取消的主题，String类型
  * 返回值 － number类型。 0：成功； 其他：失败

### MQTT.publish(topic, msg) ###
发布消息
  * topic － 发布的主题，String类型
  * msg － 主题对应的消息，String类型
  * 返回值 － number类型。 0：成功； 其他：失败

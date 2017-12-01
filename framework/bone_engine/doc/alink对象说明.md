# alink对象说明

alink对象是一个全局对象，能够在任意位置访问。

## alink 介绍

alink是指阿里巴巴使用物联网连接协议。

## 属性

## 方法

### **1. alink.getloglevel()**  
获取log输出level等级，返回数字

log level等级说明:
* 0 : AOS_LL_NONE, disable log
* 1 : AOS_LL_FATAL, fatal log will output
* 2 : AOS_LL_ERROR, fatal + error log will output
* 3 : AOS_LL_WARN, fatal + warn + error log will output(default level)
* 4 : AOS_LL_INFO, info + warn + error log will output
* 5 : AOS_LL_DEBUG, debug + info + warn + error + fatal log will output


```
print('alink.getloglevel()=' + alink.getloglevel());
```
输出
```
BoneEngine > alink.getloglevel()=3
```

### **2. alink.setloglevel(level)**  
设置log输出level等级

### **3. alink.getuuid()**  
获取当前设备的UUID，返回一个String

```
print('alink.getuuid()=' + alink.getuuid());
```
输出
```
BoneEngine > uuid=851174F61FE684496564C6CF4EEC13FB
```

### **4. alink.getstatus()**  
获取当前alink模块的状态,返回数字

status说明:
* 0 : stop
* 1 : start
* 2 : connected


### **5. alink.isrunning()**  
获取当前alink模块是否已经运行，返回数字

### **6. alink.start()**  
启动alink模块, 返回数字
* 0 : 成功


### **7. alink.postdata(method, jsonCmd)**  
发送属性上云  
输入参数：
* method : string类型, alink method
* jsonCmd : string类型, json字符串  
  返回值：数字
* 0 ： 成功


```
var cmd = {};
cmd.attrSet = [\"BodyWeight\", \"DateTime\"];
cmd.BodyWeight = {value:57.9};
cmd.DateTime = {value: now() };
alink.postdata('postDeviceData', JSON.stringify(cmd));
```

输出LOG
```
 [   0.000]<W> aos_alink_impl [alink_post_data#131] : method=postDeviceData  jsonBuf = {"attrSet":["BodyWeight","DateTime"],"BodyWeight":{"value":"91.251451"},"DateTime":{"value":"1508407685"}} 

 [   0.000]<D> accs [get_protocol_buff#87] : ------->>> lock <<<------- 

 [   0.000]<D> wsf [wsf_send_async#108] : wsf_send: {"system":{"alink":"1.0","jsonrpc":"2.0","lang":"en","sign":"2cb86d1dd34f8fa3524df8af55a3ee0d","key":"noZXLHemHlRDD4TdIcda","time":""},"request":{"cid":"rtl8188eu 12345678","rspID":-1,"target":"","token":"","uuid":"851174F61FE684496564C6CF4EEC13FB"},"method":"postDeviceData","params":{"attrSet":["BodyWeight","DateTime"],"BodyWeight":{"value":"91.251451"},"DateTime":{"value":"1508407685"}},"id":101}
 [   0.000]<D> wsf [wsf_send_msg#176] : wsf send succeed->  505

 [   0.000]<D> wsf [__send_msg#605] : wsf msg send succeed.id=2
 [   0.000]<D> accs [put_protocol_buff#94] : ------->>> unlock <<<--------

```

### **8. alink.stop()**  
停止alink模块, 返回数字
* 0 : 成功


## 事件

### **1. onStatusChange**  
alink状态变化通知

alink.on(“onStatusChange”, function(status, uuid){  
});

回调函数参数:
* status : alink状态， 与alink.getstatus()一致
* uuid : 设备UUID， 与alink.getuuid()一致


```
function onStatusChange(status, uuid){
    print('call js func onStatusChange');
    print('status =' + status);
    print('uuid=' + uuid); 
}
alink.on(\"onStatusChange\", onStatusChange); 
```

### **2. onSetAttrs**  
alink云端下发`ALINK_SET_DEVICE_STATUS`事件通知

alink.on(“onSetAttrs”, function(jsonBuf){  
});

回调函数参数:
* jsonBuf : alink云端下发`ALINK_SET_DEVICE_STATUS`事件, json字符串


### **3. onGetAttrs**  
alink云端下发`ALINK_GET_DEVICE_RAWDATA`事件通知

alink.on(“onGetAttrs”, function(jsonBuf){  
});

回调函数参数:
* jsonBuf : alink云端下发`ALINK_GET_DEVICE_RAWDATA`事件, json字符串



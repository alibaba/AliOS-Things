# cloud client 设备上云模块

模块让嵌入式设备通过 MQTT 协议，直接连接 Link Developer 平台的上云模块，包括设备的属性上报，事件触发，服务调用

### API 说明：
```

类: cloudClient
	
1. start(config, cb) 启动 cloudClient 模块：

   config 为设备三元组，格式如下：
     productKey: "xxxxxx",
     deviceName: "xxxxxx",
     deviceSecret: "xxxxxx"

   cb 为注册的回调函数，启动 cloudClient 完成时调用，包括成功或者失败
   
2. onPropertySet（cb)	 云端设置设备端的属性  
    cb 为注册的回调函数，云端设置设备端的属性时在本地执行
    
3. postProperty(params, cb) 设备上报属性到云端
    应用程序可以通过调用该函数来通知本地的设备属性已经更新

4. postEvent(eventName, params, cb) 设备上报事件到云端
    应用程序可以通过调用该函数来通知本地的设备已经触发了的事件；
    cb 为注册的回调函数，上报成功或者失败时执行
```	
## 示例：
```


//打开灯
function lightOn() {
  console.log("lightOn");
  cloudClient.postProperty({ LightSwitch: 1 });
}

//关闭灯
function lightOff() {
  console.log("lightOff");
  cloudClient.postProperty({ LightSwitch: 0 });
}

//启动上云模块
cloudClient.start(
//config配置参数
  {
    productKey: "xxxxxx",
    deviceName: "xxxxxx",
    deviceSecret: "xxxxxx"
  },
  
//回调函数cb
  function(err, data) {
    console.log("onStart");
    if (err) {
      return;
    }
//注册响应云端事件回调函数
    cloudClient.onPropertySet(function(msg) {
      if (msg.method === "thing.service.property.set") {
        if (msg.params.LightSwitch === 1) {
          lightOn();
        } else {
          lightOff();
        }
      }
    });
  }
);

//周期性的开关灯并上报到云端
setInterval(function()
{
	if(i)
		lightOn();
	else
		lightOff();
		
		i=(i+1)%2;
	
},4000);


	    

```
该示例每四秒中上报一次灯的开关状态。
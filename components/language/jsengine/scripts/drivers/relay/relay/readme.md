###Driver功能:继电器
继电器是一种电控制器，当输入量发生变化的时候，输出量也会发生变化，在电路中起着自动调节、安全保护、转换电路等作用。


###硬件资源

1.esp32Kit开发板;

2.继电器模块;

3.接线

继电器VCC引脚接esp32Kit 5V引脚；

继电器GND引脚接esp32Kit GND引脚；

继电器IN引脚接esp32Kit IO18引脚；

###Driver配置
```json
{
    "relay":{
      "type":"GPIO",
      "port":18,
      "dir": 0,
      "pull": 1
    }
}

```


###API说明
```
class：relay
param：@id  relay引脚配置信息，对应board.json中的relay;

method:
      get() 获取继电器状态;
      set() 设置继电器状态;

```

###示例
每隔两秒读取继电器的状态，并进行反转控制:

```js
var relay = require('relay/src/index.js');
var handle = new relay('relay');
setInterval(function() {
    var val;
    val = handle.get();
    console.log('relay get:'+val);
    if(val == 0){
    	console.log('relay set 1');
      handle.set(1);
    }else if (val == 1){
    	console.log('relay set 0');
    	handle.set(0);
    }
    
}, 2000);

```





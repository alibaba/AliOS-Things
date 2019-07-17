# BigKey

###Driver功能：按键


###硬件资源

1.ESP32Kit开发板

2.BigKey按键

3.接线

BigKey OUT引脚 接ESP32 IO23 引脚；

BigKey VCC引脚 接ESP32 VCC 引脚；

BigKey GND引脚 接ESP32 GND 引脚；


###Driver配置
esp32Kit示范：

```
{
  "bigkey":{
    "type":"GPIO",
    "port":23,
    "dir":2,
    "pull":1
  }
}
```

###API说明

```
class：bigkey
param：@key_id  按键配置引脚，对应board.json中的bigkey;

method:
      listen() 监听按键;
```



## #示例

```
var BigKey = require('bigkey/src');
var handle = new BigKey('bigkey');
var keycb = function(val){
    console.log('key press:'+val);
};
handle.listen('both',keycb);

```



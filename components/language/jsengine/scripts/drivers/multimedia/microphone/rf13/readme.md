# RF13

###Driver功能：高感度麦克风模块
rf13是一款高感度麦克风模块，这里我们用该模块采集周围环境声音的大小。

###硬件资源

1.esp32Kit开发板;

2.RF13模块;

3.接线

rf13 GND引脚接esp32Kit GND引脚；

rf13 VCC引脚接esp32Kit 3.3V引脚；

rf13 AO引脚接esp32Kit IO34引脚；

###Driver配置

```json
{
    "rf13":{
      "type":"ADC",
      "port":34,
      "sampling":12000000
    }
}
```

###API说明
```
class：rf13
param：@id  adc端口配置信息，对应board.json中的rf13;

method:
      get_sound() 环境音量值;
```

###示例
每隔一秒采集麦克风的声音，并打印传感器的值出来:

```js
var rf13 = require('rf13/src/index.js');
var handle = new rf13('rf13');
var t1 = setInterval(function() {
    var val = handle.get_sound();
    console.log('sound:'+val);
}, 1000);
```
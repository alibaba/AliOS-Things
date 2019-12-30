# TEMT6000

###Driver功能: 模拟光照强度传感器
TEMT6000具有很高的灵敏度，达到570nm，而且对红外光谱具有抑制作用。



###硬件资源

1.Esp32开发板;

2.TEMT模块;

3.接线

TEMT6000模块 OUT引脚接ESP32 34引脚；

TEMT6000模块 VCC引脚接ESP32 VCC3.3引脚；

TEMT6000模块 GND引脚接ESP32 GND引脚；


## Driver配置
esp32Kit示范：

```
  "temt6000":{
    "type":"ADC",
    "port":34,
    "sampling":12000000
  }
```

###API说明
```
class：temt6000
param：@id  adc端口信息，对应board.json中的temt6000;
method:
      getVal() 获取光强值

```


## 示例
每间隔1s读取环境的光强值:

```
console.log('app for temt6000! ');
var temt6000 = require('temt6000/src/index.js');

var handle = new temt6000('temt6000');
var t = setInterval(function() {
  var val = handle.getVal();
  console.log('adc val:'+val);
}, 1000);
```







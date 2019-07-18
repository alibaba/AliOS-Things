# passivebuzzer

###Driver功能：无源蜂鸣器


###硬件资源

1.ESP32Kit开发板

2.无源蜂鸣器模块

3.接线

passivebuzzer +引脚 接ESP32 IO25 引脚；

passivebuzzer GND引脚 接ESP32 GND 引脚；


###Driver配置
esp32Kit示范：

```json
{
    "buzzer":{
      "type":"DAC",
      "port":1,
      "voltage":255
    }
}

```

###API说明

```
class：passivebuzzer
param：@id ‘+’配置引脚，对应board.json中的buzzer;

method:
      once() 鸣响一次;
```



## #示例

```js
var PassiveBuzzer = require('passivebuzzer/src');
var handle = new PassiveBuzzer('buzzer');

setInterval(function() {
    handle.once()
}, 3000);

```
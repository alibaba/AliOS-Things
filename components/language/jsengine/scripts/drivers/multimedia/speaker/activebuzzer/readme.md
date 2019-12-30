# ActiveBuzzer

###Driver功能：有源蜂鸣器


###硬件资源

1.ESP32Kit开发板

2.有源蜂鸣器模块

3.接线

有源蜂鸣器模块 OUT引脚接ESP32 IO25 引脚；
有源蜂鸣器模块 VCC引脚接ESP32 VCC引脚;
有源蜂鸣器模块 GND引脚接ESP32 GND 引脚；


###Driver配置
esp32Kit示范：

```json
{
    "buzzer":{
      "type":"GPIO",
      "port":25,
      "dir":0,
      "pull":1
    }
}

```

###API说明

```
class：activebuzzer
param：@id ‘out’配置引脚，对应board.json中的buzzer;

method:
      open() 打开蜂鸣器;
      close() 关闭蜂鸣器;
      set() 打开/关闭蜂鸣器;

```

## #示例

```js
var ActiveBuzzer = require('activebuzzer/src');
var handle = new ActiveBuzzer('buzzer');

var index = 0;
setInterval(function() {
    index = 1 - index;
    if(0 == index) {
        handle.open();    
    }else{
        handle.close();   
    }
}, 2000);

```
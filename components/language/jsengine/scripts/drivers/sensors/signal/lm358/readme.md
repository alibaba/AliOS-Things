# LM358

###Driver功能: 信号放大模块
LM358是100倍增益模块。

###硬件资源

1.Esp32开发板;

2.LM358模块;

3.蜂鸣器模块;

4.接线
 
 LM358模块 OUT引脚接蜂鸣器 + 引脚；
 
 LM358模块 IN引脚接ESP32 25引脚；
 
 LM358模块 VCC引脚接ESP32 VCC3.3引脚；
 
 LM358模块 GND引脚接ESP32 GND引脚；
 
 蜂鸣器模块 GND引脚接ESP32 GND引脚；



###Driver配置
esp32Kit：

```
  "lm358":{
    "type":"DAC",
    "port":1,
    "voltage":255
  }
```

## #API说明
```
class：lm358
param：@id IN引脚对应的id信息，对应board.json中的lm358;
method:
      setVal() 设置信号值。
      getVal() 获取信号值。

```

###示例
蜂鸣器音量从强到弱进行渐变:

```
var lm358 = require('lm358/src/index.js');
var val = 0;
var handle = new lm358('lm358');
var t = setInterval(function() {
  handle.setVal(val);
  val  = val + 1;
  if (255 == val){
    val = 0;
  }
}, 200);
```















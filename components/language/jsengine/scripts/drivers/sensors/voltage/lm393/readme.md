# LM393

###Driver功能：角度传感器
lm393是一款倾斜检测传感器，通过该驱动可以实时监测该传感器的倾斜度。


###硬件资源

1.Esp32开发板;

2.LM393模块;

3.接线

lm393 GND引脚接esp32 GND引脚；

lm393 VCC引脚接esp32 3.3V引脚；

lm393 DO引脚接esp32  IO14引脚；


###Driver配置
esp32kit示范:

```
{
    "lm393":{
      "type":"GPIO",
      "port":14,
      "dir":1,
      "pull":1
    }
}

```


###API说明
```
class：lm393
param：@id DO引脚对应的id信息，对应board.json中的lm393;
method:
      check() 角度倾斜检测。
```

###示例
实时检测角度:

```
var lm393 = require('lm393/src/index.js');
var handle = new lm393('lm393');
setInterval(function() {
    var val = handle.check();
    console.log('val:'+val);
}, 2000);
```

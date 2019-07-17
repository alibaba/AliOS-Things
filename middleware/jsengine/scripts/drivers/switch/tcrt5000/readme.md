# TCRT5000

###Driver功能: 红外反射传感器

tcrt5000会不断的往外发射红外线，当发射的红外线没有返回时，trct5000的二极管将处在关断状态，否则处于打开状态。


###硬件资源

1.Esp32开发板;

2.TCRT5000模块;

3.接线

tcrt5000模块 DO引脚接ESP32 16引脚；

tcrt5000模块 VCC引脚接ESP32 VCC引脚；

tcrt5000模块 GND引脚接ESP32 GND引脚；



###Driver配置

esp32Kit示范：

```
{
    "tcrt5000.do":{
      "type":"GPIO",
      "port":16,
      "dir":1,
      "pull":1
    }
}

```

###API说明

```
class：tcrt5000
param：@id  Do引脚ID，对应board.json中的tcrt5000.do;
method:
      getVal() 获取Do状态值

```

## 示例
实时获取tcrt5000状态值：

```
var tcrt5000 = require('tcrt5000/src/index.js');
var handle = new tcrt5000('tcrt5000.do')
setInterval(function(){
    var val = handle.getVal();
    console.log('tcrt5000:'+val);
}, 100);
```








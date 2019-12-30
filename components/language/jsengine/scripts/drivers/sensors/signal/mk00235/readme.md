# MK00235

### Driver功能：雨滴检测传感器


### 硬件资源

1.mk00235模块;

2.Esp32Kit开发板；

3.接线

mk00235模块的DO引脚接Esp32Kit IO16

mk00235模块的GND引脚接Esp32Kit GND

mk00235模块的VCC引脚接Esp32Kit 3.3V
### Driver配置

```
{
    "mk00235":{
        "type":"GPIO",
        "port":16,
        "dir":1,
        "pull":1
    }
}

```


### API说明
```
class：mk00235
param：@id  gpio端口对应的id信息，对应board.json中的mk00235;

method:
      OnRain() 监控雨滴;

```


### 示例

```
var MK00235 = require('mk00235/src');
var handle = new MK00235('mk00235');

handle.OnRain(function(val){
    console.log("get rain!");
});


```
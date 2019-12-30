# SW420

##Driver功能: 震动传感器
该模块主要是用来检测环境震动，我们可以通过DO引脚输出高或低电平来确定是否有震动，其检测的阀值是可调节的。



###硬件资源

1.Esp32开发板;

2.SW420模块;

3.接线

sw420模块 DO引脚接ESP32 16引脚；

sw420模块 VCC引脚接ESP32 VCC引脚；

sw420模块 GND引脚接ESP32 GND引脚；



###Driver配置
esp32Kit示范:

```
{
    "sw420.do":{
      "type":"GPIO",
      "port":16,
      "dir":1,
      "pull":1
    }
}

```


###API说明

```
class：sw420
param：@id  Do引脚ID，对应board.json中的sw420.do;
method:
      getVal() 获取Do状态值

```

## 示例
实时获取sw420状态值：

```
var sw420 = require('sw420/src/index.js');
var handle = new sw420('sw420.do');
setInterval(function(){
    var val = handle.getVal();
    console.log('sw420:'+val);
}, 100);
```






# HS0038

## #Driver功能：红外接收传感器
hs0038是一款标准的红外接收头，这里我们会接收红外遥控发出的信号，并对其进行解析。


###硬件资源

1.esp32Kit开发板;

2.hs0038模块;

3.接线

hs0038 GND引脚接esp32Kit GND引脚；

hs0038 VCC引脚接esp32Kit 5V引脚；

hs0038 S引脚接esp32Kit IO18引脚；


###Driver配置
ESP32devkitc示范：

```
{
    "ir":{
      "type":"GPIO",
      "port":18,
      "dir":2,
      "pull":1
    }
}

```

###API说明
```
class：hs0038
param：@id  S引脚对应的id，对应board.json中的ir;
method:
      on() 监听红外信息。

```


###示例

按下遥控器上的对应按键，串口会显示对应的编码：

```
var hs0038 = require('hs0038/src/index.js');
var handle = new hs0038('ir');
handle.on(function(data){
    console.log('code:'+data);
});

```




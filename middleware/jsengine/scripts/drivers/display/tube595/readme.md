# TUBE595

## #Driver功能：tube595数码管驱动
tube595是由74hc595驱动的数码管模块，这里我们会实现4位数码管循环显示1-4的数字的功能。


## #硬件资源

1.esp32Kit开发板;

2.数码管模块;

3.接线

TUBE595 GND引脚接esp32Kit GND引脚；

TUBE595 VCC引脚接esp32Kit 3.3V引脚；

TUBE595 SCLK引脚接esp32Kit IO16引脚；

TUBE595 RCLK引脚接esp32Kit IO17引脚；

TUBE595 IO引脚接esp32Kit IO18引脚；

## #Driver配置
esp32Kit示范：

```
{
    "tube595.sclk":{
      "type":"GPIO",
      "port":16,
      "dir":0,
      "pull":1
    },
    "tube595.rclk":{
      "type":"GPIO",
      "port":17,
      "dir":0,
      "pull":1
    },
    "tube595.io":{
      "type":"GPIO",
      "port":18,
      "dir":0,
      "pull":1
    }
}

```

## #API说明
```
class：tube595
param：@sclk_ID  sclk引脚ID，对应board.json中tube595.sclk;
       @rclk_ID  rclk引脚ID，对应board.json中tube595.rclk;
       @io_ID    io引脚ID，对应board.json中tube595.io;

method:
      set() 设置数码管显示的数字

```



## 示例
循环显示数字1234：

```
var tube595 = require('tube595/src/index.js');
var handle = new tube595('tube595.sclk','tube595.rclk','tube595.io');
var index = 1;
var blink = 1000;
var t = setInterval(function() {
  handle.set(index);
    index += 1;
    if(5 == index){
        index = 1;
    }
}, blink);

```



# GY30

###Driver功能： 数字光强传感器

gy30是一款数字光强传感器，通过I2C进行数据交互.


###硬件资源

1)esp32Kit开发板

2)gy30模块;

esp32Kit GPIO19 接 GY30 模组的SCL引脚;

esp32Kit GPIO18 接 GY30 模组的SDA引脚;

esp32Kit VCC 接 GY30 模组的VCC引脚;

esp32Kit GND 接 GY30 模组的GND引脚; 

esp32Kit GND 接 GY30 模组的ADD引脚; 

###Driver配置
developKit示范：

```
{
    "gy30":{
      "type":"I2C",
      "port":1,
      "address_width":7,
      "freq":100000,
      "mode":1,
      "dev_addr":70
    }
}


```

## #API说明
```
class：gy30
param：@id  i2c端口对应的配置ID，对应board.json中的gy30;

method:
      get_light() 获取光感值;

```


###示范

每隔两秒读取光感传感器gy30的光感值:

```
var gy30 = require('gy30/src/index.js');
var handle = new gy30('gy30');
var t1 = setInterval(function() {
    var lightVal = handle.get_light();
    console.log('lightVal:'+lightVal);
}, 2000);

```







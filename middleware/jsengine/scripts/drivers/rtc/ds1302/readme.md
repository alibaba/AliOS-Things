# DS1302

###Driver功能: RTC时钟芯片
ds1302是一款高精度的时钟芯片，我们会从该传感器中读取时间值，并打印在终端上。


###硬件资源

1.DevelopKit 开发板;

2.DS1302 模块;

3.接线

ds1302 GND引脚接DevelopKit GND引脚；

ds1302 VCC引脚接DevelopKit VCC引脚；

ds1302 SCL引脚接DevelopKit SCL引脚；

ds1302 SDA引脚接DevelopKit SDA引脚；


###Driver配置
DevelopKit示范:

```
{
    "ds1302":{
      "type":"I2C",
      "port":2,
      "address_width":7,
      "freq":100000,
      "mode":1,
      "dev_addr":208
    }
}
```

###API说明

```
class：ds1302
param：@id  i2c端口对应的配置ID，对应board.json中的ds1302;

method:
      getTime() 获取当前时间;
```

###示例

获取当前时间:

```
var ds1302 = require('ds1302/src/index.js');
var handle = new ds1302('ds1302');

setInterval(function() {
    var val = handle.getTime();
    console.log('ds1302->'+val[2]+':' + val[1] +':'+val[0]);
}, 1000);

```


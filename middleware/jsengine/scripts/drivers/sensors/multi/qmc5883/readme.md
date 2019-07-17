# qmc5883

###Driver功能:电子罗盘
qmc5883是一款电子罗盘芯片，我们会读取该芯片的xyz轴的值，并把读取的数据打印在终端上。



###硬件资源

1.DevelopKit开发板;

2.QMC5883模块;

3.接线

qmc5883 GND引脚接DevelopKit GND引脚；

qmc5883 VCC引脚接DevelopKit VCC引脚；

qmc5883 SCL引脚接DevelopKit SCL引脚；

qmc5883 SDA引脚接DevelopKit SDA引脚；




###Driver配置
```
{
    "qmc5883":{
      "type":"I2C",
      "port":1,
      "address_width":7,
      "freq":100000,
      "mode":1,
      "dev_addr":26
    }
}

```

###API说明
```
class：qmc5883
param：@id  i2c端口配置信息，对应board.json中的ps2x;


method:
      getXYZ() 获取xyz轴裸数据;

```

###示例
间隔2s读取qmc5883 xyz轴的数据：

```
var qmc5883 = require('qmc5883/src/index.js');
var handle = new qmc5883('qmc5883');
setInterval(function() {
    var val = handle.getXYZ();
    console.log('qmc5883->x:'+val[0]+'y:' + val[1] +' z:'+val[2]);
}, 1000);

```




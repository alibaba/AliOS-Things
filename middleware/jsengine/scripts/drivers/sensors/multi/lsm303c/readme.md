# LSM303C

###Driver功能: 三轴磁场+三轴加速度传感器
lsm303C是融合了三轴磁场&三轴加速度的传感器，其精度可达±8g (线性加速度)和 ±16 Gauss (磁场)


###硬件资源

1.DevelopKit开发板;
2.lsm303c模块;
3.接线
LSM303C模块 SCL 引脚接 DevelopKit I2C2的SCL引脚；
LSM303C模块 SDA 引脚接 DevelopKit I2C2的SDA引脚；
LSM303C模块 VCC引脚接 DevelopKit VCC引脚；
LSM303C模块 GND引脚接 DevelopKit GND引脚；



###Driver配置
DevelopKit：

```
  "lsm303c":{
    "type":"I2C",
    "port":2,
    "address_width":7,
    "freq":100000,
    "mode":1,
    "dev_addr":58
  }
```


###API说明
```
class：lsm303c
param：@id  i2c端口对应的id信息，对应board.json中的lsm303c;
method:
      getAcc() 获取加速度raw数据;

```

##示例
每间隔3s读取acc的raw数据:

```
var lsm303c = require('lsm303c/src/index.js');
var handle = new lsm303c('lsm303c');
var getVal = function(){
  var temp = handle.getAcc();
  console.log('XL:'+temp[0]);
  console.log('XH:'+temp[1]);
  console.log('YL:'+temp[2]);
  console.log('YH:'+temp[3]);
  console.log('ZL:'+temp[4]);
  console.log('ZH:'+temp[5]);
}

var t = setInterval(getVal, 3000);

```






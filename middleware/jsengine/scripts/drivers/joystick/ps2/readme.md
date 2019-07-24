# ms5661

###Driver功能: PS2摇杆传感器
ps2模块读取xyz三轴的数据，其中xy轴是以模拟量进行输出的，z轴是以数字量进行输出。


###硬件资源

1.esp32Kit开发板;

2.PS2模块;

3.接线

PS2 GND引脚接esp32Kit GND引脚；

PS2 +5V引脚接esp32Kit 5V引脚；

PS2 VRX引脚接esp32Kit IO34引脚；

PS2 VRY引脚接esp32Kit IO35引脚；

PS2 MS引脚接esp32Kit IO12引脚；



###Driver配置

esp32Kit示范:

```json
{
    "ps2x":{
      "type":"ADC",
      "port":34,
      "sampling":12000000
    },
    "ps2y":{
      "type":"ADC",
      "port":35,
      "sampling":12000000
    },
    "ps2z":{
      "type":"GPIO",
      "port":12,
      "dir":2,
      "pull":1
    }
}

```

###API说明
```
class：ps2
param：@x_id  x轴配置信息，对应board.json中的ps2x;
       @y_id  y轴配置信息，对应board.json中的ps2y;
       @z_id  z轴配置信息，对应board.json中的ps2z;

method:
      get_xy() 获取xy轴裸数据;
      onz() 监控z轴action;
```

###示例

JavaScript

```js
console.log('JSEngine Demo run');
console.log('Welcome to use BoneEngine@lite');
console.log('产品名称: ps2遥杆');

var ps2 = require('./src/ps2.js').ps2;

function  zhandle_on(value){
    console.log('press z down!');
}
ps2.onz(zhandle_on);

setInterval(function() {
    var tmp_val = ps2.get_xy();
    console.log('x:'+tmp_val[0] + '  y:'+tmp_val[1]);
}, 100);

```

运行验证

更新代码后，操作ps2模块，能在终端中看到数据的变化：

###硬件资源

1.DevelopKit开发板;

2.ms5661模块;

3.接线
MS5661模块 SCL 引脚接 DevelopKit I2C2的SCL引脚；

MS5661模块 SDA 引脚接 DevelopKit I2C2的SDA引脚；

MS5661模块 VCC引脚接 DevelopKit VCC引脚；

MS5661模块 GND引脚接 DevelopKit GND引脚；

MS5661模块 CSB引脚接 DevelopKit VCC引脚；

###Driver配置
DevelopKit示范：

```json
  "ms5611":{
    "type":"I2C",
    "port":2,
    "address_width":7,
    "freq":100000,
    "mode":1,
    "dev_addr":236
  }
```


###API说明
```
class：mpu9250
param：@id  i2c端口配置信息，对应board.json中的mpu9250;

method:
      getAcc() 获取加速度xyz轴裸数据;
      getGyro() 获取陀螺仪xyz轴裸数据;
```


###示例
监控xyz的action数据:

```js
var ps2 = require('./src/ps2/src/index.js');
var handle = new ps2('ps2x','ps2y','ps2z');
function  zhandle_on(value){
    console.log('press z down!');
}
handle.onz(zhandle_on);

setInterval(function() {
    var tmp_val = handle.get_xy();
    console.log('x:'+tmp_val[0] + '  y:'+tmp_val[1]);
}, 100);
```










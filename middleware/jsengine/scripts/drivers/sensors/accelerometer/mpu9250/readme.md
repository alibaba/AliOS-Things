# MPU9250

###Driver功能: 陀螺仪&加速度计


###硬件资源

1.DevelopKit开发板;

2.MPU9250模块;


3.接线

MPU9250模块 SCL 引脚接 DevelopKit I2C2的SCL引脚；

MPU9250模块 SDA 引脚接 DevelopKit I2C2的SDA引脚；

MPU9250模块 VCC引脚接 DevelopKit VCC引脚；

MPU9250模块 ADD0引脚接 DevelopKit VCC引脚；
 


###Driver配置
DevelopKit示范：

```
  "mpu9250":{
    "type":"I2C",
    "port":2,
    "address_width":7,
    "freq":100000,
    "mode":1,
    "dev_addr":208
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
每间隔2s读取加速度和陀螺仪值:

```
var mpu9250 = require('mpu9250/src/index.js');
var handle = new mpu9250('mpu9250');

var getVal = function(){
  var acc = handle.getAcc();
  console.log('acc.x:'+acc[0] + ' acc.y:'+acc[1] + ' acc.z:'+acc[2]);
  var gyro = handle.getGyro();
  console.log('gyro.x:'+gyro[0] + ' gyro.y:'+gyro[1] + ' gyro.z:'+gyro[2]);
}

var t = setInterval(getVal, 2000);
```







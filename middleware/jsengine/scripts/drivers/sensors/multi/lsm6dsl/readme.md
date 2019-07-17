# LSM6DSL

###Driver功能: 加速度和陀螺仪传感器

lsm6dsl是一款具有数字加速度计和数字陀螺仪功能的低功耗传感器，通过I2C协议进行数据交互。

###硬件资源

DevelopKit开发板上自带有lsm6dsl传感器(图1红色圆圈处)，并连接到stm32的I2C2端口；


###Driver配置
```
{
    "lsm6dsl":{
      "type":"I2C",
      "port":4,
      "address_width":7,
      "freq":400000,
      "mode":1,
      "dev_addr":214
    }
}

```

###API说明
```
class：lsm6dsl
param：@id  i2c端口对应的id信息，对应board.json中的lsm6dsl;
method:
      getAcc() 获取加速度raw数据;
      getGyro() 获取陀螺仪raw数据;

```



###示例
读取加速度和陀螺仪数值：

```
var lsm6dsl = require('lsm6dsl/src/index.js');
var handle = new lsm6dsl('lsm6dsl');

 var accVal;
 var gyroVal;
var lsm6dsl_val=[0,0,0,0,0,0];

var t1 = setInterval(function() {
    var flag = 0;
    accVal = handle.getAcc();
    gyroVal = handle.getGyro();

    if((lsm6dsl_val[0]==accVal[0])&&(lsm6dsl_val[1]==accVal[1])&&(lsm6dsl_val[2]==accVal[2])){
        flag += 1;   
    }
    if((lsm6dsl_val[3]==gyroVal[0])&&(lsm6dsl_val[4]==gyroVal[1])&&(lsm6dsl_val[5]==gyroVal[2])){
        flag += 1;   
    }
    //if(2==flag)return;
    console.log('xacc:'+accVal[0] + 'yacc:'+accVal[1] + 'zacc:'+accVal[2]);
    console.log('xgyro:'+gyroVal[0] + 'ygyro:'+gyroVal[1] + 'zgyro:'+gyroVal[2]);
}, 1000);

```



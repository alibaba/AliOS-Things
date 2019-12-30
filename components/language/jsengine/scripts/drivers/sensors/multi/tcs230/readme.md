# TCS230

###Driver功能：颜色识别器
tcs230是一款颜色识别模块，具有高分辨率、可编程的颜色选择特性。



## #硬件资源

1.esp32开发板;

2.tcs230模块;

3.接线

tcs230 GND引脚接esp32 GND引脚；

tcs230 VCC引脚接esp32 3.3V引脚；

tcs230 S2引脚接esp32  IO5引脚；

tcs230 S3引脚接esp32  IO16引脚；

tcs230 out引脚接esp32  IO17引脚；



###Driver配置
esp32Kit 配置示范:

```
{
    "tcs230.s2":{
      "type":"GPIO",
      "port":5,
      "dir":0,
      "pull":1
    },
    "tcs230.s3":{
      "type":"GPIO",
      "port":16,
      "dir":0,
      "pull":1
    },
    "tcs230.out":{
      "type":"GPIO",
      "port":17,
      "dir":1,
      "pull":1
    }
}
```

###API说明
```
class：tcs230S
param：@s2_id  s2引脚ID，对应board.json中的tcs230.s2;
       @s3_id  s3引脚ID，对应board.json中的tcs230.s3;
       @out_id out引脚ID，对应board.json中的tcs230.out;

method:
      register() 注册颜色值回调;

```

###示例

实时检测颜色值：

```
var tcs230S = require('tcs230/src/index.js');
var handle = new tcs230S('tcs230.s2','tcs230.s3','tcs230.out');

var cbHandle = function(r,g,b){
    console.log('r: '+r+'g: '+g+'b: '+b);
};

handle.register(handle,cbHandle);
```

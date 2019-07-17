# STEPMOTOR

###Driver功能: 步进电机
步进电机是将电脉冲信号转变成角位移或线位移，在该实例中，我们会驱动一个5线4相的步进电机，驱动新派你用的是ULN2003。



###硬件资源

1.Esp32开发板;

2.步进电机模块;

3.ULN2003驱动模块;

3.接线

ULN2003驱动模块 IN1引脚接ESP32 16引脚；

ULN2003驱动模块 IN2引脚接ESP32 17引脚；

ULN2003驱动模块 IN3引脚接ESP32 18引脚；

ULN2003驱动模块 IN4引脚接ESP32 19引脚；

ULN2003驱动模块 VCC引脚接ESP32 5V引脚；

ULN2003驱动模块 GND引脚接ESP32 GND引脚；




###Driver配置
esp32kit 示范：

```
{
    "stepmotor.d":{
      "type":"GPIO",
      "port":16,
      "dir":0,
      "pull":0
    },
    "stepmotor.c":{
      "type":"GPIO",
      "port":17,
      "dir":0,
      "pull":0
    },
    "stepmotor.b":{
      "type":"GPIO",
      "port":18,
      "dir":0,
      "pull":0
    },
    "stepmotor.a":{
      "type":"GPIO",
      "port":19,
      "dir":0,
      "pull":0
    }
}

```

###API说明
```
class：stepmotor
param：@a_id  a相线引脚ID，对应board.json中的stepmotor.a;
       @b_id  b相线引脚ID，对应board.json中的stepmotor.b;
       @c_id  c相线引脚ID，对应board.json中的stepmotor.c;
       @d_id  d相线引脚ID，对应board.json中的stepmotor.d;

method:
      reversRun() 电机反转
      forwardRun() 电机正转

```

###示例

电机反转:

```
var stepmotor = require('stepmotor/src/index.js');
var handle = new stepmotor('stepmotor.a','stepmotor.b','stepmotor.c','stepmotor.d');
var index = 0;
setInterval(function(){
  handle.reversRun(index);
    index = index + 1;
    if(4 == index){
        index = 0;    
    }
}, 1);

```








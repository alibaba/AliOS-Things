# a4950

###Driver功能: 电机驱动模块
a4950是2路输出电机驱动模块，其工作电压在7.6V-30V，工作频率在500hz-300Khz之间。


###硬件资源

1.esp32Kit开发板; 

2.a4950模块;

3.接线

a4950模块的left0引脚接esp32kit的IO5引脚；

a4950模块的left1引脚接esp32kit的IO17引脚；

a4950模块的right0引脚接esp32kit的IO4引脚；

a4950模块的right1引脚接esp32kit的IO16引脚；



###Driver配置

esp32Kit示范:

```
{
  "a4950.left0":{
    "type":"PWM",
    "port":5,
    "freq":1000,
    "duty":0
  },
  "a4950.left1":{
    "type":"PWM",
    "port":17,
    "freq":1000,
    "duty":0
  },
  "a4950.right0":{
    "type":"PWM",
    "port":4,
    "freq":1000,
    "duty":0
  },
  "a4950.right1":{
    "type":"PWM",
    "port":16,
    "freq":1000,
    "duty":0
  }
}

```


###API说明
```


class：a4950
param：@pwmb_id  电机单侧配置引脚，对应board.json中的 
        a4950.left0或a4950.right0;
       @pwma_id  电机单侧配置引脚，对应board.json中的
       a4950.left1或a4950.right1;


method:
      stop() 电机停止;
      forward() 电机正转;
      reverse() 电机反转;
```


###示例

JavaScript

```
var a4950 = require("a4950/src/index.js"); 
var left_motor = new a4950('a4950.left0', 'a4950.left1'); 
var right_motor = new a4950('a4950.right0', 'a4950.right1'); 
left_motor.stop(); 
right_motor.stop();
left_motor.forward(20); 
right_motor.forward(20); 

```











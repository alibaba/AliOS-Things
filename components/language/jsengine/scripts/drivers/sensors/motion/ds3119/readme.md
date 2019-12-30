# ds3119

### Driver功能: 舵机模块
ds3199是一款高精度的舵机模块，其扭矩可以达到20Kg。

### 硬件资源

1.esp32Kit开发板; 

2.ds3199模块;

3.接线

ds3199模块的IN引脚接Esp32Kit的IO15

ds3199模块的Vcc引脚接Esp32Kit的Vcc引脚

ds3199模块的GND引脚接Esp32Kit的GND引脚




### Driver配置

esp32Kit示范:

```
{
  "ds3119":{
    "type":"PWM",
    "port":15,
    "freq":50,
    "duty":0
  }
}

```


###API说明
```


class：ds3119
param：@id  ADC端口配置信息，对应board.json中的battery

method:
      config() 配置舵机的最大、中间、最小位置所在的角度;
      reset() 复位舵机，偏移到中间位置;
      left_turn()向左边偏移一定角度;
      right_turn()向右边偏移一定角度;
      
```


### 示例

JavaScript

```
var ds3119 = require("ds3119/src/index.js"); 
var servo = new ds3119('ds3119'); 

const SERVO_MID = 0;
const SERVO_MIN = -45;
const SERVO_MAX = 45;

servo.config(SERVO_MIN, SERVO_MID, SERVO_MAX); 
servo.reset(); 
servo.left_turn(30); 


```


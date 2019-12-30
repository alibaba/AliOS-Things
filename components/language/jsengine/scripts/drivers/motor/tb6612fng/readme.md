# codemotor

###Driver功能: 编码电机
编码电机自带高精度编码器，马力和性能上都比较优秀，我们会通过esp32来控制电机的正转和反转，注意，驱动电机需要加上电机驱动模块，单纯的依靠esp32的IO能力是无法进行驱动的。

###硬件资源

1.Esp32开发板;

2.编码电机模块;

3.电机驱动模块;

4.接线

电机驱动模块 GND引脚接ESP32 GND引脚；

电机驱动模块 5V引脚接ESP32 5V引脚；

电机驱动模块 AIN0引脚接ESP32 0引脚；

电机驱动模块 AIN1引脚接ESP32 4引脚；

电机驱动模块 PWMA引脚接ESP32 17引脚；

电机模块M-引脚接电机驱动模块的AIN0引脚；

电机模块M+引脚接电机驱动模块的AIN1引脚；



###Driver配置
esp32Kit 示范：

```json
{
    "codedmotor.ain0":{
      "type":"GPIO",
      "port":0,
      "dir":0,
      "pull":1
    },
    "codedmotor.ain1":{
      "type":"GPIO",
      "port":4,
      "dir":0,
      "pull":1
    },
    "codedmotor.pwma":{
      "type":"PWM",
      "port":17,
      "freq":1000,
      "duty":50
    }
}

```

###API说明
```
class：codemotor
param：@ain0_id  ain0引脚对应的配置ID，对应board.json中的codedmotor.ain0;
       @ain1_id  ain1引脚对应的配置ID，对应board.json中的codedmotor.ain1;
       @pwma     pwma引脚对应的配置ID，对应board.json中的codedmotor.pwma;

method:
      getPressure() 获取气压raw值;
      getTemperature() 获取温度raw值;

```


###示例
驱动电机正转和反转:

```js
var codedmotor = require('./src/codemotor/src/index.js');
var handle = new codedmotor('codedmotor.ain0','codedmotor.ain1','codedmotor.pwma');
var duty_val = 0;
var cw_val = 0;
setInterval(function(){
    handle.set(cw_val,duty_val);
    duty_val = duty_val + 10;
    if(duty_val >= 100){
        duty_val = 0;
        cw_val = 1 - cw_val;
    }
    console.log('duty_val:'+duty_val);
}, 1000);

```







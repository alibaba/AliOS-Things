# IRF520

### Driver功能：MOS场效应管




### 硬件资源

1.IRF520模块;

2.Esp32Kit开发板；

3.接线

irf520模块的SIG引脚接Esp32Kit IO16

irf520模块的GND(控制端)引脚接Esp32Kit GND

irf520模块的VIN引脚接外部电源的正极(0-24V)

irf520模块的GND引脚接外部电源的负极

irf520模块的V+和V-接直流电机的引脚

### Driver配置

```
{
    "irf520":{
        "type":"PWM",
        "port":16,
        "freq":5000,
        "duty":0 
    }
}

```


### API说明
```
class：irf520
param：@id  pwm端口对应的id信息，对应board.json中的irf520;

method:
      Control() 调节控制pwm;

```


### 示例

直流电机的转速将随pwm的变化而变化:
```
var IRF520 = require('irf520/src');
var handle = new IRF520('irf520');
var val = 0;
var t = setInterval(function(){
    handle.Control(val);
    val += 1;
    if(val > 99){
        val = 0;
    }
    console.log('pwm:'+val+'%');
},1000);
```
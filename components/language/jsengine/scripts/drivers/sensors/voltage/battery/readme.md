# battery

### Driver功能: 电池电压测量模块
该模块能对输入的电压进行AD采样，然后根据一定的换算关系，计算出当前的电压值。


### 硬件资源

1.esp32Kit开发板; 

2.电压测量模块;

3.接线

电压测量模块的AD引脚接Esp32Kit的IO32

电压测量模块的Vin引脚接外部输出的电压

电压测量模块的Vcc引脚接Esp32Kit的Vcc引脚

电压测量模块的GND引脚接Esp32Kit的GND引脚




### Driver配置

esp32Kit示范:

```
  "battery":{
    "type":"ADC",
    "port":34,
    "sampling":12000000
  }

```


###API说明
```


class：battery
param：@id  ADC端口配置信息，对应board.json中的battery

method:
      config() 配置参考电压以及ADC采样bits;
      read_voltage() 读取当前电压值;
      
```


### 示例

JavaScript

```
var voltage = new battery('battery'); 
voltage.config(3.3, 12);

setInterval(function () {
    var  value = voltage.read_voltage(); 
    console.log('voltage:' + value);  
  }, 2000); 

```


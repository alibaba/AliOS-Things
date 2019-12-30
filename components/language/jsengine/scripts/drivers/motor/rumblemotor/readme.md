# rumblemotor

### Driver功能：震动马达


### 硬件资源

1.震动马达模块;

2.Esp32Kit开发板；

3.接线:

震动马达模块VCC接Esp32Kit的3.3V引脚;

震动马达模块GND接Esp32Kit的GND引脚;

震动马达模块Sig接Esp32Kit的IO18引脚;

### Driver配置

```json
{
    "rumblemotor":{
        "type":"GPIO",
        "port":18,
        "dir": 0,
        "pull": 1
    }
}

```


### API说明
```


class：rumblemotor
param：@id  gpio端口对应的id信息，对应board.json中的rumblemotor;


method:
      Open()  打开震动;
      Close() 关闭震动;

```


### 示例


```js
var RUMBELMOTOR = require('rumblemotor/src');
var handle = new RUMBELMOTOR('rumblemotor');

var index = 0;

var Control = function(){
    index = 1 - index;
    if(0 == index){
        handle.Close();
    }else{
        handle.Open();
    }
};

setInterval(Control,10);
```
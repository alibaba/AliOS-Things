# MQ7

### Driver功能：CO浓度测量驱动




### 硬件资源

1.MQ7模块;

2.Esp32Kit开发板；

### Driver配置

```
{
    "mq7.ao":{
        "type":"ADC",
        "port":34,
        "sampling":12000000
    },
    "mq7.do":{
        "type":"GPIO",
        "port":18,
        "dir": 1,
        "pull": 1
    }
}

```


### API说明
```

ao_id,do_id
class：mq7
param：@ao_id  模拟端口对应的id信息，对应board.json中的mq7.ao;

@do_id  数字量端口对应的id信息，对应board.json中的mq7.do;
method:
      GetHeavy() 获取CO浓度模拟量;
      GetFlags() 获取数字量值;

```


### 示例

每间隔2s读取传感器CO浓度值和数字量值:
```
var MQ7 = require('mq7/src');
var handle = new MQ7('mq7.ao','mq7.do');

setInterval(function() {
    var tmp_val = handle.GetHeavy();
    console.log('heavy:'+tmp_val);
    tmp_val = handle.GetFlags();
    if(0 == tmp_val){
        console.log('Warning! CO gas out of limit!')
    }
}, 2000);
 
```
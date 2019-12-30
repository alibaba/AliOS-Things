# MQ135

### Driver功能：空气质量检测


### 硬件资源

1.MQ135模块;

2.Esp32Kit开发板；

### Driver配置

```
{
    "mq135.ao":{
        "type":"ADC",
        "port":34,
        "sampling":12000000
    },
    "mq135.do":{
        "type":"GPIO",
        "port":18,
        "dir": 1,
        "pull": 1
    }
}

```


### API说明
```


class：mq135
param：@ao_id  模拟端口对应的id信息，对应board.json中的mq135.ao;

@do_id  数字量端口对应的id信息，对应board.json中的mq135.do;
method:
      GetQuarity() 获取空气质量;
      GetFlags() 获取数字量值;

```


### 示例

每间隔2s读取传感器空气质量值和数字量值:
```
var MQ135 = require('mq135/src');
var handle = new MQ135('mq135.ao','mq135.do');

setInterval(function() {
    var tmp_val = handle.GetQuarity();
    console.log('quarity:'+tmp_val);
    tmp_val = handle.GetFlags();
    if(0 == tmp_val){
        console.log('Warning! air  quarity  out of limit!')
    }
}, 2000);
```
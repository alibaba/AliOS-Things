# D5A2

### Driver功能：激光传感器


### 硬件资源

1.D5A2模块;

2.Esp32Kit开发板；

### Driver配置

```
{
    "d5a2":{
        "type":"GPIO",
        "port":18,
        "dir": 0,
        "pull": 1
    }
}

```


### API说明
```


class：d5a2
param：@id  gpio端口对应的id信息，对应board.json中的d5a2;


method:
      Open() 打开激光;
      Close() 关闭激光

```


### 示例

每间隔4秒进行打开关闭操作:
```
var D5A2 = require('d5a2/src');
var handle = new D5A2('d5a2');

var index = 0;

var Control = function(){
    index = 1 - index;
    if(0 == index){
        handle.Close();
    }else{
        handle.Open();
    }
};

setInterval(Control,4000);
```
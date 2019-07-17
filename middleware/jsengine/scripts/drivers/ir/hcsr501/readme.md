# HCR501

###Driver功能: 人体红外检测



###硬件资源

1.esp32Kit开发板;

2.HCR501模块;

3.接线


HCR501模块OUT引脚接esp32Kit IO5引脚；

###Driver配置

```
{
    "hcsr501":{
      "type":"GPIO",
      "port":5,
      "dir": 1,
      "pull": 1
    }
}


```

###API说明
```
class：hcsr501
param：@id  out引脚对应的配置ID，对应board.json中的hcsr501;

method:
      check() 检测人体红外信息;

```


###示例

实时检测环境人体红外信息:

```
var hcsr501 = require('hcsr501/src/index.js');
var handlehcsr = new hcsr501('hcsr501');

var listen = function(){
  var val = handlehcsr.check();
  if(val){
    console.log('get it!');
  }
}
var t = setInterval(listen, 200);


```


# mmc3680kj

###Driver功能：磁力计传感器

mmc3680kj是一款磁力计传感器，通过I2C进行数据交互.



## #硬件资源

DevelopKit开发板上自带有mmc3680kj传感器：


###Driver配置
developKit示范如下：

```
{
    "mmc3680kj":{
      "type":"I2C",
      "port":1,
      "address_width":7,
      "freq":200000,
      "mode":1,
      "dev_addr":96
    } 
}


```

###API说明

```
class：mmc3680kj
param：@c1_id  第一列引脚ID，对应board.json中的matrixkeypad.C1;


method:
      getmemsic() 获取xyz轴的磁力裸数据;

```



###示例
每间隔2s获取传感器磁力值:

```
var mmc3680kj = require('mmc3680kj/src/index.js');
var handle = new mmc3680kj('mmc3680kj');
var t1 = setInterval(function() {
   
   var val = handle.getmemsic();
    if(-1==val[0] && -1==val[1] && -1==val[2]){
        return;   
    }
    console.log('mmc3680kj:x='+val[0]+' y:'+val[1] + ' z:'+val[2]);
}, 2000);

```




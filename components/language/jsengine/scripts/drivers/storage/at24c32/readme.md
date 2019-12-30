# AT24C32

###设备功能: 存储芯片
at24c32是一款存储芯片，它的容量大小是32Kb，它往往用来存储一些小规模的数据，我们会向该芯片中写入数据，并把读取的数据打印在终端上。

###硬件资源

1.DevelopKit开发板;

2.AT24C32模块;

3.接线

at24c32 GND引脚接DevelopKit GND引脚；

at24c32 VCC引脚接DevelopKit VCC引脚；

at24c32 SCL引脚接DevelopKit SCL引脚；

at24c32 SDA引脚接DevelopKit SDA引脚；


###Driver配置

DeveloperKit示范：

```
{
    "at24c32":{
      "type":"I2C",
      "port":2,
      "address_width":7,
      "freq":200000,
      "mode":1,
      "dev_addr":160
    }
}

```

###API说明


```
class：at24c32
param：@id  i2c端口对应的配置ID，对应board.json中的at24c32;
method:
      read_one() 从某个地址读取一个数据;
      write_one() 向某个地址写入一个数据;

```


###示例

该示例实现了往存储芯片写入顺序递增数字，并随后读取出来显示的效果:

```
var at24c32 = require('at24c32/src/index.js');
var handle = new at24c32('at24c32');

var i = 0;
for(i=0;i<16;i++){
    var page = 1;
    var addr = i%256;
    var dest = page << 8;
    dest = dest | addr;
    handle.write_one(i,i+1);
}


for(i=0;i<16;i++){
    var page = 1;
    var addr = i%256;
    var dest = page << 8;
    dest = dest | addr;
    var recv = handle.read_one(i);
    console.log('index:'+i+' data:'+recv);
}

```







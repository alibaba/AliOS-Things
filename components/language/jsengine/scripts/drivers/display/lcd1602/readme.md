# LCD1602

### Driver功能：lcd1602驱动

该模块结合了PCF8574AT扩展芯片，使得用户只需要2个IO就能驱动该LCD。


### 硬件资源

1.LCD1602&PCF8574AT模块;

2.Esp32Kit开发板；

### Driver配置

```js
{
    "lcd1602":{
      "type":"I2C",
      "port":1,
      "address_width":7,
      "freq":100000,
      "mode":1,
      "dev_addr":126
    }
}
```

### API说明

```
class：lcd1602
param：@id  i2c端口对应的id信息，对应board.json中的lcd1602;
method:
      init_lcd() 初始化lcd;
      show_char()显示字符;
```

### 示例

在第一行和第二行分别顺序显示assic字符;

```js
var LCD1602 = require('lcd1602/src');
var handle = new LCD1602('lcd1602');

var cval0 = 65;
var cval1 = 97;
var i = 0;
handle.init_lcd();
for(i=0;i<39;i++){
    cval0 = cval0 + 1;
    handle.show_char(i,0,cval0);
    cval1 = cval1 + 1;
    handle.show_char(i,1,cval1);
}

 
```
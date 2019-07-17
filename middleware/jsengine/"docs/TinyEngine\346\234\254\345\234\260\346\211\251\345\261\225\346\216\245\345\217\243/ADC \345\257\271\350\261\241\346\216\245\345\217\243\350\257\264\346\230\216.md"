# ADC

## 

| API               | 说明                                                         |
| ----------------- | ------------------------------------------------------------ |
|                   |                                                              |
| ADC.open(id)      | 功能：配置和启动adc参数： id:和板级配置文件中的id保持一致； 返回值：成功：回资源handle，失败:-1 |
| ADC.read(handle)  | 功能：获取当前adc值 参数：handle:资源handle，为ADC.open的返回值;返回值：adc值 |
| ADC.close(handle) | 功能：关闭adc 参数：handle:资源handle，为ADC.open的返回值;  返回值：0=ok other=fail; |

### 示例代码

```
/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
var temt6000 = function(id) {
  this.handle = ADC.open(id);
  this.getVal = function() {
    var i = 0;
    var val = 0;
    for (i=0; i<10; i++){
      val += ADC.read(this.handle);
    }
    val = val /10;
    return val;
  };
};
```

## 
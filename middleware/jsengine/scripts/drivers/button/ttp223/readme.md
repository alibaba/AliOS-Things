
## Driver功能
电容触摸按键

## Driver配置
示范如下：
```js
  "ttp223":{
    "type":"GPIO",
    "port":76,
    "dir":2,
    "pull":1
  }
```

## 应用示例

```js
console.log('driver test for ttp223');
var ttp223 = require('ttp223/src/index.js');
var handle = new ttp223('ttp223');
var pressListen = function() {
  console.log('press');
};
handle.on('rising',pressListen);
```
## 运行验证
按键被触摸后，log终端将会打印"press"信息。







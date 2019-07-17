
//hcsr501 人体红外传感器驱动测试程序，可以通过 TinyEngine IDE 工具 创建一个简单的应用，将如下代码直接拷贝到工程中
//去测试；

var hcsr501 = require('hcsr501/src/index.js');

//创建 hcsr501 驱动对象
var handlehcsr = new hcsr501('hcsr501');

var listen = function(){
  
  // 每隔 200ms 检测一次红外传感器状态
  var val = handlehcsr.check();
  if(val){
    console.log('get it!');
  }
}
var t = setInterval(listen, 200);

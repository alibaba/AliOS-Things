
var ttp223 = require('ttp223/src/index.js');

//创建 ttp223 驱动对象
var handle = new ttp223('ttp223');

// 回调处理函数，用户可以自行定制
var pressListen = function() {
  console.log('press');
};

//监听TTP的上升沿变化
handle.on('rising',pressListen);

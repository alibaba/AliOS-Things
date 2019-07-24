
//hcsr501 test code, with JSEngine Embeded JS WorkStation: https://linkdevelop.aliyun.com/deviceapp#/s
var hcsr501 = require('hcsr501/src/index.js');

//create hcsr501 object
var handlehcsr = new hcsr501('hcsr501');

var listen = function () {
  // check sensor status
  var val = handlehcsr.check();
  if (val) {
    console.log('get it!');
  }
}
var t = setInterval(listen, 200);

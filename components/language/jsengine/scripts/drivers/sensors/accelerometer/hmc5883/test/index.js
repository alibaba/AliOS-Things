
var hmc5883 = require('hmc5883/src/index.js');
var handle = new hmc5883('hmc5883');

var getVal = function () {
  var val = handle.getAcc();
  console.log('XH:' + val[0]);
  console.log('XL:' + val[1]);
  console.log('YH:' + val[2]);
  console.log('YL:' + val[3]);
  console.log('ZH:' + val[4]);
  console.log('ZL:' + val[5]);
}

var t = setInterval(getVal, 2000);

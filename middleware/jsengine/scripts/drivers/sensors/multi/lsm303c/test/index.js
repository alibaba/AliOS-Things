
var lsm303c = require('lsm303c/src/index.js');
var handle = new lsm303c('lsm303c');


var getVal = function(){
  var temp = handle.getAcc();
  console.log('XL:'+temp[0]);
  console.log('XH:'+temp[1]);
  console.log('YL:'+temp[2]);
  console.log('YH:'+temp[3]);
  console.log('ZL:'+temp[4]);
  console.log('ZH:'+temp[5]);
}

var t = setInterval(getVal, 3000);




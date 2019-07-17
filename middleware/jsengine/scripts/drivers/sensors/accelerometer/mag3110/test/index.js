
var mag3110 = require('mag3110/src/index.js');
var handle = new mag3110('mag3110');

var getVal = function(){
  var temp = handle.getAcc();
  var check = temp[0] + temp[1] + temp[2] +temp[3] + temp[4] + temp[5];
  if(0 == check)return;

  console.log('xL:'+temp[0]);
  console.log('xH:'+temp[1]);
  console.log('yL:'+temp[2]);
  console.log('yH:'+temp[3]);
  console.log('zL:'+temp[4]);
  console.log('zH:'+temp[5]);
}

var t = setInterval(getVal, 2000);






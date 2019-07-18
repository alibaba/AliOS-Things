var lsm6dsl = require('lsm6dsl/src/index.js');
var handle = new lsm6dsl('lsm6dsl');

var accVal;
var gyroVal;
var lsm6dsl_val = [0, 0, 0, 0, 0, 0];

var t1 = setInterval(function () {
  var flag = 0;
  accVal = handle.getAcc();
  gyroVal = handle.getGyro();

  if ((lsm6dsl_val[0] == accVal[0]) && (lsm6dsl_val[1] == accVal[1]) && (lsm6dsl_val[2] == accVal[2])) {
    flag += 1;
  }
  if ((lsm6dsl_val[3] == gyroVal[0]) && (lsm6dsl_val[4] == gyroVal[1]) && (lsm6dsl_val[5] == gyroVal[2])) {
    flag += 1;
  }
  //if(2==flag)return;
  console.log('xacc:' + accVal[0] + 'yacc:' + accVal[1] + 'zacc:' + accVal[2]);
  console.log('xgyro:' + gyroVal[0] + 'ygyro:' + gyroVal[1] + 'zgyro:' + gyroVal[2]);
}, 1000);

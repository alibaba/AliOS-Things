var adxl345 = require('adxl345/src/index.js');
var handle = new adxl345('adxl345');
var adxl345_val = [0, 0, 0];
var t1 = setInterval(function () {

  var accVal = handle.getAcc();
  if ((adxl345_val[0] == accVal[0]) && (adxl345_val[1] == accVal[1]) && (adxl345_val[2] == accVal[2])) {
    return;
  }
  console.log('xacc:' + accVal[0] + 'yacc:' + accVal[1] + 'zacc:' + accVal[2]);
  adxl345_val[0] = accVal[0];
  adxl345_val[1] = accVal[1];
  adxl345_val[2] = accVal[2];
}, 1000);
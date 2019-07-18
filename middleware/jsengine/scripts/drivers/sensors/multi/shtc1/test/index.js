
var shtc1 = require('shtc1/src/index.js');
var handle = new shtc1('shtc1');

var shtc1_val = [0, 0];
var t1 = setInterval(function () {

  var val = handle.getTempHumi();
  if (val[0] == shtc1_val[0] && val[1] == shtc1_val[1]) {
    return;
  }
  console.log('shtc1:temp=' + val[0] + ' humi:' + val[1]);
  shtc1_val[0] = val[0];
  shtc1_val[1] = val[1];
}, 2000);
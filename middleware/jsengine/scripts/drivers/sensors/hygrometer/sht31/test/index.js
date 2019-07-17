
var sht31 = require('sht31/src/index.js');
var handle = new sht31('sht31');

var getVal = function(){
  var val = handle.getHumTem();
  console.log('tem:'+val[0] + ' hum:'+val[1]);
}

var t = setInterval(getVal, 2000);
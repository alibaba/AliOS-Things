
var ms5611 = require('ms5611/src/index.js');
var handle = new ms5611('ms5611');
handle.reset();
var getVal = function(){
  var temp = handle.getTemperature();
  var pressure = handle.getPressure();
  console.log('temp:'+temp + ' pressure:'+pressure);
}

var t = setInterval(getVal, 1000);


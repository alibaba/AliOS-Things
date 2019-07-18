
var si7021 = require('si7021/src/index.js');
var handle = new si7021('si7021');

var getVal = function () {
  var temp = handle.getTemperature();
  console.log('Temperature:' + temp);
  var humi = handle.getHumi();
  console.log('humi:' + humi);

}

var t = setInterval(getVal, 2000);


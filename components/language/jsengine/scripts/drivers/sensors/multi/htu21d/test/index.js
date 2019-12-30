
var htu21d = require('htu21d/src/index.js');
var handle = new htu21d('htu21d');
handle.reset();

var getVal = function () {
  var temp = handle.getTemperature();
  var hum = handle.getHumidity();
  console.log('temp:' + temp + ' hum:' + hum);
}

var t = setInterval(getVal, 1000);
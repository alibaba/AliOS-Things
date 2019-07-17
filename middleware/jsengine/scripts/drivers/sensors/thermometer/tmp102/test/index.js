
var tmp102 = require('tmp102/src/index.js');
var handle = new tmp102('tmp102');


var getVal = function(){
  var temp = handle.getTemperature();
  console.log('Temperature:'+temp);

}

var t = setInterval(getVal, 2000);





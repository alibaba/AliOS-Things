var bmp280 = require('bmp280/src/index.js');
var handle = new bmp280('bmp280');
var t1 = setInterval(function() {
   
    var pressure = handle.getPressure();
    var temperature = handle.getTemperature();
    console.log('bmp280 pressure raw data:'+pressure);
    console.log('bmp280 temperature raw data:'+temperature);
}, 3000);
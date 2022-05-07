import * as i2c from 'i2c'
import * as sht3x from './sht3x.js'

console.log('Testing sht3x ...');
var i2cDev = i2c.open({
    id: 'sht3x',
    success: function() {
        console.log('i2c: open success')
    },
    fail: function() {
        console.log('i2c: open failed')
    }
});

var sht3xDev = sht3x.SHT3X(i2cDev);
setInterval(function(){
    var temperature = sht3xDev.getTemperature();
    var humidity = sht3xDev.getHumidity();
    console.log('temperature is ', temperature, '°C');
    console.log('humidity is ', humidity, '%H');
    var data = sht3xDev.getTempHumidity();
    console.log('temperature is ', data[0], '°C');
    console.log('humidity is ', data[1], '%H');
}, 5000);

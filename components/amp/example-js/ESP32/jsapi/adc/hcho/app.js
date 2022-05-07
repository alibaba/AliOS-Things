import * as adc from 'adc'
import * as hcho from './hcho.js'

console.log('Testing hcho ...');
var adcDev = adc.open({
    id: 'hcho',
    success: function() {
        console.log('adc: open success');
    },
    fail: function() {
        console.log('adc: open failed');
    }
});

var hchoDev = hcho.Hcho(adcDev);
setInterval(function(){
    var value = hchoDev.getPPM();
    console.log('The hcho ppm value ' + value);
}, 5000);

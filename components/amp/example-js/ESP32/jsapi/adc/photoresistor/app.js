import * as adc from 'adc'
import * as photoresistor from './photoresistor.js'

console.log('Testing photoresistor ...');
var adcDev = adc.open({
    id: 'photoresistor',
    success: function() {
        console.log('adc: open success')
    },
    fail: function() {
        console.log('adc: open failed')
    }
});

var photoresistorDev = photoresistor.PhotoResistor(adcDev);
setInterval(function(){
    var value = photoresistorDev.getLightness();
    console.log('The photoresistor lightness ' + value);
}, 5000);

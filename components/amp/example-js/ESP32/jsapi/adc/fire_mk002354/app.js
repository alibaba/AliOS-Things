import * as adc from 'adc'
import * as fire from './fire_mk002354.js'

console.log('Testing fire ...');
var adcDev = adc.open({
    id: 'fire',
    success: function() {
        console.log('adc: open success')
    },
    fail: function() {
        console.log('adc: open failed')
    }
});

var fireDev = fire.Fire(adcDev);
setInterval(function(){
    var value = fireDev.getVoltage();
    console.log('The fire Voltage ' + value);
}, 5000);

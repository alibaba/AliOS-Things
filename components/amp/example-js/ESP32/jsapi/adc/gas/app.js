import * as adc from 'adc'
import * as gas from './gas.js'

console.log('Testing gas ...');
var adcDev = adc.open({
    id: 'gas',
    success: function() {
        console.log('adc: open success')
    },
    fail: function() {
        console.log('adc: open failed')
    }
});

var gasDev = gas.Gas(adcDev);
setInterval(function(){
    var value = gasDev.getVoltage();
    console.log('The gas Voltage ' + value);
}, 5000);

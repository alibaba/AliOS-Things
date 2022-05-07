import * as gpio from 'gpio'
import * as ir from './ir.js'

console.log('Testing IR detector ...');
var gpioDev = gpio.open({
    id: 'ir',
    success: function() {
        console.log('gpio: open success')
    },
    fail: function() {
        console.log('gpio: open failed')
    }
});

var irDev = ir.IR(gpioDev);
setInterval(function(){
    var value = irDev.irDetect();
    console.log('The ir status ' + value);
}, 5000);

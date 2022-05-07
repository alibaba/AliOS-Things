import * as gpio from 'gpio'
import * as dcmotor from './dcmotor.js'

console.log('Testing dcmotor ...');
var gpioDev = gpio.open({
    id: 'dcmotor',
    success: function() {
        console.log('gpio: open success')
    },
    fail: function() {
        console.log('gpio: open failed')
    }
});

var dcmotorDev = dcmotor.DCMOTOR(gpioDev);
setInterval(function(){
    dcmotorDev.ctrl(1);
    console.log('The dcmotor is started');
    sleepMs(200);
    dcmotorDev.ctrl(0);
    console.log('The dcmotor is stopped');
    sleepMs(200);
}, 5000);


var codedmotor = require('codemotor/src/index.js');
var handle = new codedmotor('codedmotor.ain0','codedmotor.ain1','codedmotor.pwma');
var duty_val = 0;
var cw_val = 0;
setInterval(function(){
    handle.set(cw_val,duty_val);
    duty_val = duty_val + 10;
    if(duty_val >= 100){
        duty_val = 0;
        cw_val = 1 - cw_val;
    }
    console.log('duty_val:'+duty_val);
}, 1000);




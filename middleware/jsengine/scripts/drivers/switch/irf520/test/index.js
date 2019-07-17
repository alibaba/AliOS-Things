var IRF520 = require('irf520/src');
var handle = new IRF520('irf520');
var val = 0;
var t = setInterval(function(){
    handle.Control(val);
    val += 1;
    if(val > 99){
        val = 0;
    }
    console.log('pwm:'+val+'%');
},1000);
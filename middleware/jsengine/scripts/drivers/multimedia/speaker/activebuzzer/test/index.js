var ActiveBuzzer = require('activebuzzer/src');
var handle = new ActiveBuzzer('buzzer');

var index = 0;
setInterval(function() {
    index = 1 - index;
    if(0 == index) {
        handle.open();    
    }else{
        handle.close();   
    }
}, 2000);
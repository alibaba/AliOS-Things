var RUMBELMOTOR = require('rumblemotor/src');
var handle = new RUMBELMOTOR('rumblemotor');

var index = 0;

var Control = function(){
    index = 1 - index;
    if(0 == index){
        handle.Close();
    }else{
        handle.Open();
    }
};

setInterval(Control,10);
var D5A2 = require('d5a2/src');
var handle = new D5A2('d5a2');

var index = 0;

var Control = function(){
    index = 1 - index;
    if(0 == index){
        handle.Close();
    }else{
        handle.Open();
    }
};

setInterval(Control,4000);
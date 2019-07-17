var stepmotor = require('stepmotor/src/index.js');
var handle = new stepmotor('stepmotor.a','stepmotor.b','stepmotor.c','stepmotor.d');
var index = 0;
setInterval(function(){
  handle.reversRun(index);
    index = index + 1;
    if(4 == index){
        index = 0;    
    }
}, 1);
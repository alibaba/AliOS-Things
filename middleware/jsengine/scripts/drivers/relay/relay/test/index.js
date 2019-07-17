var relay = require('relay/src/index.js');
var handle = new relay('relay');
setInterval(function() {
    var val;
    val = handle.get();
    console.log('relay get:'+val);
    if(val == 0){
    	console.log('relay set 1');
      handle.set(1);
    }else if (val == 1){
    	console.log('relay set 0');
    	handle.set(0);
    }
    
}, 2000);
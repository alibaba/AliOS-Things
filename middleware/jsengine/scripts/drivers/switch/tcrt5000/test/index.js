var tcrt5000 = require('tcrt5000/src/index.js');
var handle = new tcrt5000('tcrt5000.do')
setInterval(function(){
    var val = handle.getVal();
    console.log('tcrt5000:'+val);
}, 100);
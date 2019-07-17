var tcs230S = require('tcs230/src/index.js');
var handle = new tcs230S('tcs230.s2','tcs230.s3','tcs230.out');

var cbHandle = function(r,g,b){
    console.log('r: '+r+'g: '+g+'b: '+b);
};

handle.register(handle,cbHandle);









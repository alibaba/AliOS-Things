var FC51 = require('fc51/src');
var handle = new FC51('fc51');

setInterval(function() {
    tmp_val = handle.GetFlags();
    if(0 == tmp_val){
        console.log('Warning! Obstacles..')
    }
}, 100);
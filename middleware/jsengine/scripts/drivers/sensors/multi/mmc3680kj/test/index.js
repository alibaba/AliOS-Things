var mmc3680kj = require('mmc3680kj/src/index.js');
var handle = new mmc3680kj('mmc3680kj');
var t1 = setInterval(function() {
   
   var val = handle.getmemsic();
    if(-1==val[0] && -1==val[1] && -1==val[2]){
        return;   
    }
    console.log('mmc3680kj:x='+val[0]+' y:'+val[1] + ' z:'+val[2]);
}, 2000);
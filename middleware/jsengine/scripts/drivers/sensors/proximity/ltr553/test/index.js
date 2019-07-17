var ltr553 = require('ltr553/src/index.js');
var handle = new ltr553('ltr553');

var t1 = setInterval(function() {
    var val = handle.getPsAls();
    console.log('ltr553:als='+val[0]+' ps:'+val[1]);
}, 2000);
 

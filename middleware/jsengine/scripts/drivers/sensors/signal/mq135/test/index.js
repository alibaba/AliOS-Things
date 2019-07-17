var MQ135 = require('mq135/src');
var handle = new MQ135('mq135.ao','mq135.do');

setInterval(function() {
    var tmp_val = handle.GetQuarity();
    console.log('quarity:'+tmp_val);
    tmp_val = handle.GetFlags();
    if(0 == tmp_val){
        console.log('Warning! air  quarity  out of limit!')
    }
}, 2000);
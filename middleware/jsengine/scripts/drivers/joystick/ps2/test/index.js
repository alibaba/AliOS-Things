var ps2 = require('ps2/src/index.js');
var handle = new ps2('ps2x','ps2y','ps2z');
function  zhandle_on(value){
    console.log('press z down!');
}
handle.onz(zhandle_on);

setInterval(function() {
    var tmp_val = handle.get_xy();
    console.log('x:'+tmp_val[0] + '  y:'+tmp_val[1]);
}, 100);


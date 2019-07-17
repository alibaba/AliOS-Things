var touchkey = require('tkey/src/index.js');
var handle = new touchkey('touchkey.key0','touchkey.key1')
function keyHandle(val){
    console.log('Touch key'+' '+val + 'press');
}
handle.on(keyHandle);
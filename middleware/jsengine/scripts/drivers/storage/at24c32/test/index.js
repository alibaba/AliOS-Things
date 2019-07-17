var at24c32 = require('at24c32/src/index.js');
var handle = new at24c32('at24c32');

var i = 0;
for(i=0;i<16;i++){
    var page = 1;
    var addr = i%256;
    var dest = page << 8;
    dest = dest | addr;
    handle.write_one(i,i+1);
}


for(i=0;i<16;i++){
    var page = 1;
    var addr = i%256;
    var dest = page << 8;
    dest = dest | addr;
    var recv = handle.read_one(i);
    console.log('index:'+i+' data:'+recv);
}
var PL2303 = require('pl2303/src');
var handle = new PL2303('pl2303');

handle.recv(null,null,function(data){
    console.log('recv:'+data);
});

setInterval(function() {
    handle.send('test send uart data\r\n');
}, 2000);
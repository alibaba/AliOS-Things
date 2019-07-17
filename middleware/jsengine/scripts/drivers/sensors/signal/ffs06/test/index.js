var FFS06 = require('ffs06/src');
var handle = new FFS06('ffs06.do');

handle.OnFlame(function(flag){
    console.log('Flame!');
});
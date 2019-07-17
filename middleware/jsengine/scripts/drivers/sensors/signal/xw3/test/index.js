var XW3 = require('xw3/src');
var handle = new XW3('xw3');

handle.OnCollision(function(val){
    console.log("Collision!");
});
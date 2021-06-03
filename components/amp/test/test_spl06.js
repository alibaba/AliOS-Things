/*
Please add this section into app.json when run this script as app.js.
This configuration is designed for haas100 edu k1.
"spl06": {
    "type": "I2C",
    "port": 1,
    "addrWidth": 7,
    "freq": 400000,
    "mode": "master",
    "devAddr": 119
}
*/

console.log('testing spl06...');

var spl06 = require('./spl06.js');

spl06.init("spl06");

var version = spl06.getID();
console.log("spl06 version is: " , version);

spl06.spl06_getdata();

spl06.deinit();
console.log("test spl06 success!");

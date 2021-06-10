
/*
Please add this section into app.json when run this script as app.js.
This configuration is designed for haas100 edu k1.
"qlc5883": {
    "type": "I2C",
    "port": 1,
    "addrWidth": 7,
    "freq": 400000,
    "mode": "master",
    "devAddr": 13
}
*/

console.log('testing qlc5883...');

var qlc5883 = require('./qlc5883.js');
qlc5883.init("qlc5883");

qlc5883.qmc5883l_readHeading();
qlc5883.qmc5883l_readHeading();
qlc5883.qmc5883l_readHeading();
qlc5883.qmc5883l_readHeading();

qlc5883.deinit();
console.log("test qlc5883 success!");





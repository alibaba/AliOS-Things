/*
Please add this section into app.json when run this script as app.js.
This configuration is designed for HaaS EDU K1.
"ap3216c": {
    "type": "I2C",
    "port": 1,
    "addrWidth": 7,
    "freq": 100000,
    "mode": "master",
    "devAddr": 30
}
*/

console.log('testing ap3216c...');

var ap3216c = require('./ap3216c.js');

ap3216c.init("ap3216c");

while (1)
{
    var brightness = ap3216c.ap3216c_read_ambient_light();
    console.log("ap3216c brightness is: " , brightness);

    var ir_data = ap3216c.ap3216c_read_ir_data();
    console.log("ap3216c ir_data is: " , ir_data);

    var ps_data = ap3216c.ap3216c_read_ps_data();
    console.log("ap3216c ps_data is: " , ps_data);

    if ((ps_data >> 15) & 1)
    console.log("near !");
    else
    console.log("far !");

    sleepMs(500)
}

ap3216c.deinit();
console.log("test ap3216c success!");

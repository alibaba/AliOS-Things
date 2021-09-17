/* Please insert below lines to app.json file if you want to enable lm75:

      "I2C0": {
        "type": "I2C",
        "port": 0,
        "addrWidth": 7,
        "freq": 400000,
        "mode": "master",
        "devAddr": 72
      },

*/

var i2c = require('i2c');
var lm75 =

function lm75Init()
{
    lm75 = i2c.open({
        id: 'I2C0'
    });
}

function lm75tmpGet()
{
    var sig = 1;
    var temp;
    var regval = lm75.readMem(0x00, 2);
    var tempAll = (regval[0] << 8) + regval[1];

    if (regval[0] & 0x80 != 0) {
        tempAll = ~(tempAll) + 1;
        sig = -1;
    }

    tempAll = tempAll >> 5;
    temp = tempAll * 0.125 * sig;

    return temp;
}

module.exports = {
    lm75Init,
    lm75tmpGet
}

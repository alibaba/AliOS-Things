var i2c = require('i2c');

var calibParamT1;
var calibParamT2;
var calibParamT3;
var calibParamP1;
var calibParamP2;
var calibParamP3;
var calibParamP4;
var calibParamP5;
var calibParamP6;
var calibParamP7;
var calibParamP8;
var calibParamP9;
var calibParamT_FINE;

const INVALID_TEMP = -273.15;
const INVALID_PRESS = 0;

var sensor = i2c.open({
  id: 'bmp280',
  success: function() {
    console.log('bmp280 sensor open success')
  },
  fail: function() {
    console.log('bmp280 sensor open failed')
  }
});

function bmp280SoftReset()
{
    var regaddr = 0xe0;
    var regval = [0xb6];
    sensor.writeMem(regaddr, regval);
    console.log('bmp280 soft reset');
}

function bmp280PowerEnable()
{
    var regval = sensor.readMem(0xf4, 1);
    regval[0] |= 0x03;
    sensor.writeMem(0xf4, regval);
    console.log('bmp280 power up');
}

function bmp280PowerDisable()
{
    var regval = sensor.readMem(0xf4, 1);
    regval[0] &= 0xfc;
    sensor.writeMem(0xf4, regval);
    console.log('bmp280 power down');
}

function bmp280SetOdr()
{
    var regval = sensor.readMem(0xf5, 1);
    regval[0] &= 0x1f;
    regval[0] |= 0x20;
    sensor.writeMem(0xf5, regval);
    console.log('bmp280 set odr');
}

function bmp280SetWorkMode()
{
    var regval = sensor.readMem(0xf4, 1);
    console.log('bmp280 old work mode ' + regval);
    regval[0] &= 0x03;
    regval[0] |= (0x03 << 5 | 0x03 << 2);
    sensor.writeMem(0xf4, regval);
    regval = sensor.readMem(0xf4, 1);
    console.log('bmp280 new work mode ' + regval);
}

function bmp280ReadCalibParams()
{
    var calibTable = sensor.readMem(0x88, 24);
    console.log('bmp280 calib table ' + calibTable);
    calibParamT1 = (calibTable[1] << 8) | calibTable[0];
    calibParamT2 = (calibTable[3] << 8) | calibTable[2];
    calibParamT3 = (calibTable[5] << 8) | calibTable[4];

    calibParamP1 = (calibTable[7] << 8) | calibTable[6];
    calibParamP2 = (calibTable[9] << 8) | calibTable[8];
    calibParamP3 = (calibTable[11] << 8) | calibTable[10];
    calibParamP4 = (calibTable[13] << 8) | calibTable[12];
    calibParamP5 = (calibTable[15] << 8) | calibTable[14];
    calibParamP6 = (calibTable[17] << 8) | calibTable[16];
    calibParamP7 = (calibTable[19] << 8) | calibTable[18];
    calibParamP8 = (calibTable[21] << 8) | calibTable[20];
    calibParamP9 = (calibTable[23] << 8) | calibTable[22];
}

function bmp280RegRead(reg, len)
{
    var regval = sensor.readMem(reg, len);

    if(!regval) {
        return;
    }

    return regval;
}

function bmp280TemperatureRead()
{
    var temperature = INVALID_TEMP;
    var regval = bmp280RegRead(0xfa, 3);

    if (regval) {
        var uncomp_temp = (regval[0] << 12) | (regval[1] << 4) | (regval[2] >> 4);
        var ta = (((uncomp_temp >> 3) - (calibParamT1 << 1)) * calibParamT2) >> 11;
        var tb = (((((uncomp_temp >> 4) - calibParamT1) * ((uncomp_temp >> 4) - calibParamT1)) >> 12) * calibParamT3) >> 14;
        calibParamT_FINE = ta + tb;
        temperature = ((ta + tb) * 5 + 128) >> 8;
        temperature /= 100;
    } else {
        console.error('Failed to get temperature');
    }

    return temperature;
}

function bmp280PressureRead()
{
    var pressure = INVALID_PRESS;
    var regval;

    // get t_fine value first
    if (bmp280TemperatureRead() == INVALID_TEMP) {
        console.error('Failed to get temperature');
        return pressure;
    }

    regval = bmp280RegRead(0xf7, 3);
    if (regval) {
        var uncomp_press = (regval[0] << 12) | (regval[1] << 4) | (regval[2] >> 4);
        var var1 = (calibParamT_FINE >> 1) - 64000;
        var var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * calibParamP6;
        var2 = var2 + ((var1 * calibParamP5) << 1);
        var2 = (var2 >> 2) + (calibParamP4 << 16);
        var1 = (((calibParamP3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) + ((calibParamP2 * var1) >> 1)) >> 18;
        var1 = ((32768 + var1) * calibParamP1) >> 15;
        pressure = ((1048576 - uncomp_press) - (var2 >> 12)) * 3152;

        if (var1 != 0) {
            if (pressure < 0x80000000) {
                //pressure = (pressure << 1) / var1;
                pressure = (pressure / var1) << 1;
            } else {
                pressure = (pressure / var1) << 1;
            }

            var1 = (calibParamP9 * (((pressure >> 3) * (pressure >> 3)) >> 13)) >> 12;
            var2 = ((pressure >> 2) * calibParamP8) >> 13;
            pressure = (pressure + ((var1 + var2 + calibParamP7) >> 4))
        }
    } else {
        console.error('Failed to get pressure');
    }

    return pressure;
}

function bmp280_init()
{
    var chipID = sensor.readMem(0xd0, 1);
    console.log('bmp280 chip id is ' + chipID);
    bmp280SoftReset();
    bmp280SetOdr();
    bmp280ReadCalibParams();
    bmp280SetWorkMode();
    bmp280PowerEnable();
}

// for test purpose only
function bmp280_test()
{
    var cnt = 10;
    var loop = 10;

    bmp280_init();

    var intervalHandled = setInterval(function(){
        console.log('Temperature is ' + bmp280TemperatureRead());
        console.log('Pressure is ' + bmp280PressureRead());
        cnt--;
        if(cnt <= 0){
            //clearInterval(intervalHandled)
            console.log('bmp280: sensor close ' + loop);
            sensor.close();
    
            loop--;
            cnt = 1;
            if (loop == 0) {
                clearInterval(intervalHandled);
            }
            else {
              sensor = i2c.open({
              id: 'bmp280',
              success: function() {
                console.log('bmp280 sensor open success')
              },
              fail: function() {
                console.log('bmp280 sensor open failed')
              }
            });
                
            }
    
        }
    }, 2000);
}

module.exports = {
    bmp280_init,
    bmp280TemperatureRead,
    bmp280PressureRead,
    bmp280_test,
}

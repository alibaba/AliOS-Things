// The driver for spl06 chip, it is a temperature and humidity sensor.

/*
添加如下配置到app.json中：
        "spl06": {
            "type": "I2C",
            "port": 1,
            "addrWidth": 7,
            "freq": 400000,
            "mode": "master",
            "devAddr": 119
        }
*/
// Require libjs/lib/i2c.js module.
var i2c = require('i2c');

var spl06Dev;
/*
duktape engine doesn't support async/await to sleep.
function sleep(ms) {
    return new Promise(function (resolve, reject) {
        setTimeout(function () {
            resolve();
        }, ms);
    });
}
*/

function fakeSleep(ms) {
    var date = new Date();
    var curDate;
    do {
        curDate = new Date();
    } while((curDate - date) < ms);
}

/* Init spl06
 *
 * I2C's options are configured in app.json. Specify the ID, e.g. I2C0 below to init spl06.
 * "I2C0": {
 *     "type": "I2C",
 *     "port": 1,
 *     "addrWidth": 7,
 *     "freq": 400000,
 *     "mode": "master",
 *     "devAddr": 119
 *   }
 */

var EEPROM_CHIP_ADDRESS = 0x77;


function i2c_eeprom_read_var(chipAddress, addr){
    return devRegReadWrite1Byte(0, addr, 0);
}

function devRegRead1Byte(addr) {
    return devRegReadWrite1Byte(0, addr, 0);
}

function devRegReadWrite1Byte(mode, addr, value)
{
    //0 read mode
    //1 write mode
    if (mode == 0) {
        var Reg = [];
        Reg[0] = addr
        spl06Dev.write(Reg);
        value = spl06Dev.read(1);
        // console.log("<-- read addr " + addr + ", value = " + value);
        return value;
    } else {
        var Reg = [addr, value];
        spl06Dev.write(Reg);
        // console.log("--> write addr " + addr + ", value = " + value);
        return 0;
    }
}

function init(i2cid) {
    spl06Dev = i2c.open({id: i2cid});
    fakeSleep(30);

    var tmp = 0;
    var rRegID = [0x0D, 0x0];
    var wRegPressure8xOversampling = [0x06, 0x03];
    var wRegTemperature8xOversampling = [0x07, 0x83];
    var wRegContinuousTempAndPressureMeasurement = [0x08, 0B0111];
    var wRegFIFOPressureMeasurement = [0x09, 0x00];

    tmp = rRegID;
    devRegReadWrite1Byte(0, tmp[0], tmp[1]);

    tmp = wRegPressure8xOversampling;
    devRegReadWrite1Byte(1, tmp[0], tmp[1]);
    tmp = wRegTemperature8xOversampling;
    devRegReadWrite1Byte(1, tmp[0], tmp[1]);
    tmp = wRegContinuousTempAndPressureMeasurement;
    devRegReadWrite1Byte(1, tmp[0], tmp[1]);
    tmp = wRegFIFOPressureMeasurement;
    devRegReadWrite1Byte(1, tmp[0], tmp[1]);
}

// Get the firmware version of the chip.
function getID() {
    var reg = [0x0D];
    var version = 0;

    spl06Dev.write(reg);
    version = spl06Dev.read(1);

    console.log("spl06 ID is " + version);

    return version;
}


function get_altitude(pressure, seaLevelhPa)
{
    if (seaLevelhPa == 0) {
        return -1;
    }

    var altitude;

    pressure /= 100;
    altitude = 44330 * (1.0 - Math.pow(pressure / seaLevelhPa, 0.1903));

    return altitude;
}

function get_temperature_scale_factor()
{
    var k;

    var tmp_Byte;
    tmp_Byte = i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X07); // MSB

    tmp_Byte = tmp_Byte & 0B00000111;
    console.log("tmp_Byte: %d\n", tmp_Byte);

    switch (tmp_Byte) {
        case 0B000:
            k = 524288.0;
            break;

        case 0B001:
            k = 1572864.0;
            break;

        case 0B010:
            k = 3670016.0;
            break;

        case 0B011:
            k = 7864320.0;
            break;

        case 0B100:
            k = 253952.0;
            break;

        case 0B101:
            k = 516096.0;
            break;

        case 0B110:
            k = 1040384.0;
            break;

        case 0B111:
            k = 2088960.0;
            break;
    }
    console.log("k=%d\n", k);
    return k;
}

function get_pressure_scale_factor()
{
    var k;

    var tmp_Byte;
    tmp_Byte = i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X06); // MSB
    // tmp_Byte = tmp_Byte >> 4; //Focus on bits 6-4 - measurement rate
    tmp_Byte = tmp_Byte & 0B00000111; // Focus on 2-0 oversampling rate
    // tmp_Byte = 0B011;

    // oversampling rate
    switch (tmp_Byte) {
        case 0B000:
            k = 524288.0;
            break;

        case 0B001:
            k = 1572864.0;
            break;

        case 0B010:
            k = 3670016.0;
            break;

        case 0B011:
            k = 7864320.0;
            break;

        case 0B100:
            k = 253952.0;
            break;

        case 0B101:
            k = 516096.0;
            break;

        case 0B110:
            k = 1040384.0;
            break;

        case 0B111:
            k = 2088960.0;
            break;
    }
    console.log("k=%d\n", k);
    return k;
}

function get_traw()
{
    var tmp;
    var tmp_MSB, tmp_LSB, tmp_XLSB;
    tmp_MSB  = i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X03); // MSB
    tmp_LSB  = i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X04); // LSB
    tmp_XLSB = i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X05); // XLSB

    tmp = (tmp_MSB << 8) | tmp_LSB;
    tmp = (tmp << 8) | tmp_XLSB;

    if (tmp & (1 << 23))
        tmp = tmp | 0XFF000000; // Set left bits to one for 2's complement
                                // conversion of negitive number
    console.log("get_traw: tmp_MSB=%d, tmp_LSB=%d, tmp_XLSB=%d\n", tmp_MSB, tmp_LSB, tmp_XLSB);
    return tmp;
}

function get_praw(){
    var tmp;
    var tmp_MSB, tmp_LSB, tmp_XLSB;
    tmp_MSB  = i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X00); // MSB
    tmp_LSB  = i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X01); // LSB
    tmp_XLSB = i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X02); // XLSB

    tmp = (tmp_MSB << 8) | tmp_LSB;
    tmp = (tmp << 8) | tmp_XLSB;

    if (tmp & (1 << 23))
        tmp = tmp | 0XFF000000; // Set left bits to one for 2's complement
                                // conversion of negitive number
    return tmp;
}

function get_c0()
{
    var tmp;
    var tmp_MSB, tmp_LSB;

    tmp_MSB = i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X10);
    tmp_LSB = i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X11);

    tmp_LSB = tmp_LSB >> 4;

    tmp = (tmp_MSB << 4) | tmp_LSB;

    if (tmp & (1 << 11))
        // Check for 2's complement negative number
        tmp = tmp | 0XF000; // Set left bits to one for 2's complement
                            // conversion of negitive number
    if (tmp > (1 << 15)) {
        tmp = tmp - (1<<16)
    }
    console.log("get_c0: tmp_MSB=%d, tmp_LSB=%d, tmp=%d\n", tmp_MSB, tmp_LSB, tmp);

    return tmp;
}

function get_c1()
{
    var tmp;
    var tmp_MSB, tmp_LSB;

    tmp_MSB = i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X11);
    tmp_LSB = i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X12);

    tmp_MSB = tmp_MSB & 0XF;
    tmp     = (tmp_MSB << 8) | tmp_LSB;

    if (tmp & (1 << 11))
        // Check for 2's complement negative number
        tmp = tmp | 0XF000; // Set left bits to one for 2's complement
                            // conversion of negitive number
    
    if (tmp > (1 << 15)) {
        tmp = tmp - (1<<16)
    }
    console.log("get_c1: tmp_MSB=%d, tmp_LSB=%d, tmp=%d\n", tmp_MSB, tmp_LSB, tmp);
    return tmp;
}

function get_c00()
{
    var tmp;
    var tmp_MSB, tmp_LSB, tmp_XLSB;

    tmp_MSB  = i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X13);
    tmp_LSB  = i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X14);
    tmp_XLSB = i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X15);

    tmp_XLSB = tmp_XLSB >> 4;

    tmp = (tmp_MSB << 8) | tmp_LSB;
    tmp = (tmp << 4) | tmp_XLSB;

    tmp = tmp_MSB << 12 | tmp_LSB << 4 |
          tmp_XLSB >> 4;

    if (tmp & (1 << 19))
        tmp = tmp | 0XFFF00000; // Set left bits to one for 2's complement
                                // conversion of negitive number

    return tmp;
}

function get_c10()
{
    var tmp;
    var tmp_MSB, tmp_LSB, tmp_XLSB;

    tmp_MSB  = i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X15); // 4 bits
    tmp_LSB  = i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X16); // 8 bits
    tmp_XLSB = i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X17); // 8 bits

    tmp_MSB = tmp_MSB & 0b00001111;

    tmp = (tmp_MSB << 4) | tmp_LSB;
    tmp = (tmp << 8) | tmp_XLSB;

    tmp = tmp_MSB << 16 | tmp_LSB << 8 | tmp_XLSB;

    if (tmp & (1 << 19))
        tmp = tmp | 0XFFF00000; // Set left bits to one for 2's complement
                                // conversion of negitive number

    return tmp;
}

function get_c01()
{
    var tmp;
    var tmp_MSB, tmp_LSB;

    tmp_MSB = i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X18);
    tmp_LSB = i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X19);

    tmp = (tmp_MSB << 8) | tmp_LSB;
    if (tmp > (1 << 15)) {
        tmp = tmp - (1<<16)
    }
    return tmp;
}

function get_c11()
{
    var tmp;
    var tmp_MSB, tmp_LSB;

    tmp_MSB = i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X1A);
    tmp_LSB = i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X1B);

    tmp = (tmp_MSB << 8) | tmp_LSB;
    if (tmp > (1 << 15)) {
        tmp = tmp - (1<<16)
    }
    return tmp;
}

function get_c20()
{
    var tmp;
    var tmp_MSB, tmp_LSB;

    tmp_MSB = i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X1C);
    tmp_LSB = i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X1D);

    tmp = (tmp_MSB << 8) | tmp_LSB;
    if (tmp > (1 << 15)) {
        tmp = tmp - (1<<16)
    }
    return tmp;
}

function get_c21()
{
    var tmp;
    var tmp_MSB, tmp_LSB;

    tmp_MSB = i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X1E);
    tmp_LSB = i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X1F);

    tmp = (tmp_MSB << 8) | tmp_LSB;
    if (tmp > (1 << 15)) {
        tmp = tmp - (1<<16)
    }
    return tmp;
}

function get_c30()
{
    var tmp;
    var tmp_MSB, tmp_LSB;

    tmp_MSB = i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X20);
    tmp_LSB = i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0X21);

    tmp = (tmp_MSB << 8) | tmp_LSB;
    if (tmp > (1 << 15)) {
        tmp = tmp - (1<<16)
    }
    return tmp;
    //  console.log("tmp: %d\n", tmp);
}

function spl06_getdata()
{
    var tmp;
    var c00, c10;
    var c0, c1, c01, c11, c20, c21, c30;

    // Serial.println("\nDevice Reset\n");
    // i2c_eeprom_write_var(EEPROM_CHIP_ADDRESS, 0x0C, 0b1001);
    // delay(1000);

    tmp    = i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0x0D);
    tmp    = i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0x06);
    tmp    = i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0x07);
    tmp    = i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0x08);
    tmp    = i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0x09);
    tmp    = i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0x0A);
    tmp    = i2c_eeprom_read_var(EEPROM_CHIP_ADDRESS, 0x0B);

    c0              = get_c0();
    c1              = get_c1();
    c00             = get_c00();
    c10             = get_c10();
    c01             = get_c01();
    c11             = get_c11();
    c20             = get_c20();
    c21             = get_c21();
    c30             = get_c30();
    var traw    = get_traw();
    var  traw_sc = traw / get_temperature_scale_factor();
    traw_sc = traw_sc.toFixed(2)
    console.log("traw_sc: %0.2f\n", traw_sc);

    var Ctemp = c0 * 0.5 + c1 * traw_sc;
    Ctemp = Ctemp.toFixed(2)
    console.log("Ctemp:" + Ctemp + ".  " + "c0:" + c0 + " c1" + c1 + " traw_sc:" + traw_sc);

    var Ftemp = (Ctemp * 9 / 5) + 32;
    Ftemp = Ftemp.toFixed(2)
    console.log("Ftemp:" + Ftemp)

    var praw = get_praw();

    var praw_sc = (praw) / get_pressure_scale_factor();
    

    console.log("praw:" + praw)
    console.log("praw_sc:" + praw_sc)

    console.log("c00:" + c00)
    console.log("c10:" + c10)
    console.log("c20:" + c20)
    console.log("c30:" + c30)
    console.log("c01:" + c01)
    console.log("c11:" + c11)
    console.log("c21:" + c21)

    var pcomp =
        (c00) +
        praw_sc * ((c10) +
                   praw_sc * ((c20) + praw_sc * (c30))) +
        traw_sc * (c01) +
        traw_sc * praw_sc * ((c11) + praw_sc * (c21));

    var pressure = pcomp / 100; // convert to mb
    console.log("pressure:" + pressure)

    // var local_pressure = 1010.5; // Look up local sea level pressure on
    // google
    var local_pressure =
        1011.1; // Look up local sea level pressure on google // Local pressure
                // from airport website 8/22
    console.log("Local Airport Sea Level Pressure: %0.2f mb\n", local_pressure);
    var altitude = get_altitude(pcomp, local_pressure);
    console.log("altitude:" + altitude)
}




// De-init spl06
function deinit() {
    spl06Dev.close();
}

module.exports = {
    init,
    getID,
    spl06_getdata,
    deinit
}

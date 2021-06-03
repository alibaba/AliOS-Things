// The driver for qlc5883 chip, it is a temperature and humidity sensor.


var x_max, x_min, z_min, y_max, y_min, z_max;
var addr;
var mode;
var rate;
var range;
var oversampling;

var INT16_MIN = (-32767-1)
var INT16_MAX = 32767

/* Register numbers */
var QMC5883L_X_LSB    = 0
var QMC5883L_X_MSB    = 1
var QMC5883L_Y_LSB    = 2
var QMC5883L_Y_MSB    = 3
var QMC5883L_Z_LSB    = 4
var QMC5883L_Z_MSB    = 5
var QMC5883L_STATUS   = 6
var QMC5883L_TEMP_LSB = 7
var QMC5883L_TEMP_MSB = 8
var QMC5883L_CONFIG   = 9
var QMC5883L_CONFIG2  = 10
var QMC5883L_RESET    = 11
var QMC5883L_RESERVED = 12
var QMC5883L_CHIP_ID  = 13

/* Bit values for the STATUS register */
var QMC5883L_STATUS_DRDY = 1
var QMC5883L_STATUS_OVL  = 2
var QMC5883L_STATUS_DOR  = 4

/* Oversampling values for the CONFIG register */
var QMC5883L_CONFIG_OS512 = 0b00000000
var QMC5883L_CONFIG_OS256 = 0b01000000
var QMC5883L_CONFIG_OS128 = 0b10000000
var QMC5883L_CONFIG_OS64  = 0b11000000

/* Range values for the CONFIG register */
var QMC5883L_CONFIG_2GAUSS = 0b00000000
var QMC5883L_CONFIG_8GAUSS = 0b00010000

/* Rate values for the CONFIG register */
var QMC5883L_CONFIG_10HZ  = 0b00000000
var QMC5883L_CONFIG_50HZ  = 0b00000100
var QMC5883L_CONFIG_100HZ = 0b00001000
var QMC5883L_CONFIG_200HZ = 0b00001100

/* Mode values for the CONFIG register */
var QMC5883L_CONFIG_STANDBY = 0b00000000
var QMC5883L_CONFIG_CONT    = 0b00000001

/* Apparently M_PI isn't available in all environments. */
var M_PI = 3.14159265358979323846264338327950288

/*
添加如下配置到app.json中：
        "qlc5883": {
            "type": "I2C",
            "port": 1,
            "addrWidth": 7,
            "freq": 400000,
            "mode": "master",
            "devAddr": 13
        }
*/
// Require libjs/lib/i2c.js module.
var i2c = require('i2c');

var qlc5883Dev;
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

/* Init qlc5883
 *
 * I2C's options are configured in app.json. Specify the ID, e.g. I2C0 below to init qlc5883.
 * "I2C0": {
 *     "type": "I2C",
 *     "port": 1,
 *     "addrWidth": 7,
 *     "freq": 400000,
 *     "mode": "master",
 *     "devAddr": 13
 *   }
 */

var QMC5883L_ADDR = 0x0D;


function i2c_eeprom_read_var(chipAddress, addr){
    return devRegReadWrite1Byte(0, addr, 0);
}

function devRegRead1Byte(addr) {
    var Reg = [];
    Reg[0] = addr
    qlc5883Dev.write(Reg);
    value = qlc5883Dev.read(1);
    console.log("<-- read addr " + addr + ", value = " + value);
    return value;
}

function devRegWrite1Byte(data) {
    var Reg = [];
    Reg[0] = data;
    qlc5883Dev.write(Reg);
    console.log("--> write addr " + addr + ", value = " + Reg);
}

function devRegReadNByte(addr, len)
{
    var data = [];
    var reg = [addr]
    qlc5883Dev.write(reg);
    for (i = 0; i < len; i++) {
        data[i] = qlc5883Dev.read(1);
    }
    console.log("--> read addr " + addr + ", " + len + " bytes value = " + data);
    return data;
}

function init(i2cid) {
    qlc5883Dev = i2c.open({id: i2cid});
    fakeSleep(30);

    _qmc5883l_init();
}

// De-init qlc5883
function deinit() {
    qlc5883Dev.close();
}

function qmc5883l_write_register(addr, reg,  data)
{
    devRegWrite1Byte(reg);
    fakeSleep(200);
    devRegWrite1Byte(data);
}

function qmc5883l_read_register(addr, reg)
{
    return devRegRead1Byte(reg);
}

function qmc5883l_read_len(addr, reg, buf, len)
{
    return devRegReadNByte(reg, len);
}

function qmc5883l_reconfig()
{
    qmc5883l_write_register(addr, QMC5883L_CONFIG,
                            oversampling | range | rate | mode);
    qmc5883l_write_register(addr, QMC5883L_CONFIG2, 0x1);
}

function qmc5883l_reset()
{
    qmc5883l_write_register(addr, QMC5883L_RESET, 0x01);
    fakeSleep(500);
    qmc5883l_reconfig();
    qmc5883l_resetCalibration();
}

function qmc5883l_setOversampling(x)
{
    switch (x) {
        case 512:
            oversampling = QMC5883L_CONFIG_OS512;
            break;
        case 256:
            oversampling = QMC5883L_CONFIG_OS256;
            break;
        case 128:
            oversampling = QMC5883L_CONFIG_OS128;
            break;
        case 64:
            oversampling = QMC5883L_CONFIG_OS64;
            break;
    }
    qmc5883l_reconfig();
}

function qmc5883l_setRange(x)
{
    switch (x) {
        case 2:
            range = QMC5883L_CONFIG_2GAUSS;
            break;
        case 8:
            range = QMC5883L_CONFIG_8GAUSS;
            break;
    }
    qmc5883l_reconfig();
}

function qmc5883l_setSamplingRate(x)
{
    switch (x) {
        case 10:
            rate = QMC5883L_CONFIG_10HZ;
            break;
        case 50:
            rate = QMC5883L_CONFIG_50HZ;
            break;
        case 100:
            rate = QMC5883L_CONFIG_100HZ;
            break;
        case 200:
            rate = QMC5883L_CONFIG_200HZ;
            break;
    }
    qmc5883l_reconfig();
}

function _qmc5883l_init()
{
    /* This assumes the wire library has been initialized. */
    addr         = QMC5883L_ADDR;
    oversampling = QMC5883L_CONFIG_OS512;
    range        = QMC5883L_CONFIG_8GAUSS;
    rate         = QMC5883L_CONFIG_200HZ;
    mode         = QMC5883L_CONFIG_CONT;

    qmc5883l_reset();
}

function qmc5883l_ready()
{
    fakeSleep(200);
    return qmc5883l_read_register(addr, QMC5883L_STATUS) & QMC5883L_STATUS_DRDY;
}

function qmc5883l_readRaw(x, y, z)
{
    var  data;
    var timeout = 10000;
    var arr = [];

    while (!qmc5883l_ready() && (timeout--))
        ;

    data = qmc5883l_read_len(addr, QMC5883L_X_LSB, data, 6);

    x = data[0] | (data[1] << 8);
    y = data[2] | (data[3] << 8);
    z = data[4] | (data[5] << 8);

    if (x > (1 << 15)) {
        x = x - (1<<16)
    }
    if (y > (1 << 15)) {
        y = y - (1<<16)
    }
    if (z > (1 << 15)) {
        z = z - (1<<16)
    }

    arr[0] = x;
    arr[1] = y;
    arr[2] = z;
    return arr;
}

function qmc5883l_resetCalibration()
{
    x_max = y_max = z_max = INT16_MIN;
    x_min = y_min = z_min = INT16_MAX;
}

function qmc5883l_readHeading()
{
    var x_org, y_org, z_org;
    var xyz_org = [];
    var x_offset, y_offset, z_offset;
    var x_fit, y_fit, z_fit;

    qmc5883l_read_register(addr, QMC5883L_STATUS) & QMC5883L_STATUS_DRDY;    

    xyz_org = qmc5883l_readRaw(x_org, y_org, z_org);
    x_org = xyz_org[0];
    y_org = xyz_org[1];
    z_org = xyz_org[2];

    console.log("org[%d,%d,%d],\n"+ " x:" + x_org+ " y:" + y_org+ " z:" + z_org);

    /* Update the observed boundaries of the measurements */
    x_min = x_org < x_min ? x_org : x_min;
    x_max = x_org > x_max ? x_org : x_max;
    y_min = y_org < y_min ? y_org : y_min;
    y_max = y_org > y_max ? y_org : y_max;
    z_min = z_org < z_min ? z_org : z_min;
    z_max = z_org > z_max ? z_org : z_max;

    /* Bail out if not enough data is available. */
    if (x_min == x_max || y_min == y_max || z_max == z_min)
        return 0;

    /* Recenter the measurement by subtracting the average */
    x_offset = (x_max + x_min) / 2.0;
    y_offset = (y_max + y_min) / 2.0;
    z_offset = (z_max + z_min) / 2.0;

    x_fit = (x_org - x_offset) * 1000.0 / (x_max - x_min);
    y_fit = (y_org - y_offset) * 1000.0 / (y_max - y_min);
    z_fit = (z_org - z_offset) * 1000.0 / (z_max - z_min);

    console.log("fix[%f,%f,%f],\n", "x " + x_fit + ",y " +  y_fit + ",z " + z_fit);

    var heading = 180.0 * Math.atan2(x_fit, y_fit) / M_PI;
    heading     = (heading <= 0) ? (heading + 360) : heading;

    return heading;
}


module.exports = {
    init,
    qmc5883l_readHeading,
    deinit
}

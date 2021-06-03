// The driver for Si7006 chip, it is a temperature and humidity sensor.

// The register address in Si7006 controller.
var Si7006_MEAS_REL_HUMIDITY_MASTER_MODE    = 0xE5;
var Si7006_MEAS_REL_HUMIDITY_NO_MASTER_MODE = 0xF5;
var Si7006_MEAS_TEMP_MASTER_MODE            = 0xE3;
var Si7006_MEAS_TEMP_NO_MASTER_MODE         = 0xF3;
var Si7006_READ_OLD_TEMP                    = 0xE0;
var Si7006_RESET                            = 0xFE;
var Si7006_READ_ID_LOW_0                    = 0xFA;
var Si7006_READ_ID_LOW_1                    = 0x0F;
var Si7006_READ_ID_HIGH_0                   = 0xFC;
var Si7006_READ_ID_HIGH_1                   = 0xC9;
var Si7006_READ_Firmware_Revision_0         = 0x84;
var Si7006_READ_Firmware_Revision_1         = 0xB8;

// Require libjs/lib/i2c.js module.
var i2c = require('i2c');

var si7006Dev;

/* Init Si7006
 *
 * I2C's options are configured in app.json. Specify the ID, e.g. I2C0 below to init Si7006.
 * "I2C0": {
 *     "type": "I2C",
 *     "port": 1,
 *     "addrWidth": 7,
 *     "freq": 400000,
 *     "mode": "master",
 *     "devAddr": 72
 *   }
 */
function init(i2cid) {
    si7006Dev = i2c.open({id: i2cid});
}

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

// Get the firmware version of the chip.
function getVer() {
    var reg = [Si7006_READ_Firmware_Revision_0, Si7006_READ_Firmware_Revision_1];
    var version = 0;

    si7006Dev.write(reg);
    version = si7006Dev.read(1);

    //console.log("si7006 firmware version is " + version);

    return version;
}

// Get the chip ID.
function getID() {
    var chipID, lowID, highID;
    var lowIDReg = [Si7006_READ_ID_LOW_0, Si7006_READ_ID_LOW_1];
    var highIDReg = [Si7006_READ_ID_HIGH_0, Si7006_READ_ID_HIGH_1];

    si7006Dev.write(lowIDReg);
    lowID = si7006Dev.read(4);


    si7006Dev.write(highIDReg);
    highID = si7006Dev.read(4);

    chipID = lowID.concat(highID);

    return chipID;
}

// Get temperature
function getTemperature() {
    var reg = [Si7006_MEAS_TEMP_NO_MASTER_MODE];
    var readData;
    var value;
    var temperature;

    si7006Dev.write(reg);
    fakeSleep(30);
    readData = si7006Dev.read(2);

    value = (readData[0] << 8) | readData[1];
    // A temperature measurement will always return XXXXXX00 in the LSB field.
    if (value & 0xFFFC) {
        temperature = ((175.72 * value) / 65536.0) - 46.85;
        return temperature;
    } else {
        throw new Error("Error on getTemperature");
    }
}

// Get humidity
function getHumidity() {
    var reg = [Si7006_MEAS_REL_HUMIDITY_NO_MASTER_MODE];
    var readData;
    var value;
    var humidity;

    si7006Dev.write(reg);
    fakeSleep(30);
    readData = si7006Dev.read(2);

    value = (readData[0] << 8) | readData[1];
    if (value & 0xFFFE) {
        humidity = ((125.0 * value) / 65535.0) - 6.0;
        return humidity;
    } else {
        throw new Error ("Error on getHumidity");
    }
}

// Get temperature and humidity
function getTempHumidity() {
    var tempHumidity = [0, 0];

    tempHumidity[0] = getTemperature();
    tempHumidity[1] = getHumidity();

    return tempHumidity;
}

// De-init Si7006
function deinit() {
    si7006Dev.close();
}

module.exports = {
    init,
    getVer,
    getID,
    getTemperature,
    getHumidity,
    getTempHumidity,
    deinit
}

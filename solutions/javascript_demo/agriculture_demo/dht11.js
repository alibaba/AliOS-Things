var gpio = require("gpio");
var dht11 = gpio.open({
  di: 'dht11'
});

function usleep(us)
{
    // TODO
}

function msleep(ms)
{
    return new Promise(resolve => setTimeout(resolve, ms));
}

function gpioSet(level)
{
    dht11.writeValue(level);
}

function gpioGet()
{
    return dht11.readValue();
}

function reset()
{
    gpioSet(0);
    await msleep(20);
    gpioSet(1);
    await msleep(30);
}

function isOnline()
{
    var retry = 0;

    while (gpioGet() != 0 && retry < 100) {
        retry++;
        usleep(1);
    }

    if (retry >= 100) {
        console.error("DHT11 pin high!");
        return false;
    } else {
        retry = 0;
    }

    while (gpioGet() == 0 && retry < 100) {
        retry++;
        usleep(1);
    }

    if (retry >= 100) {
        console.error("DHT11 pin low!");
        return false;
    }

    return true;
}

function readBit()
{
    var retry = 0;

    while (gpioGet() != 0 && retry < 100) {
        retry++;
        usleep(1);
    }

    retry = 0;
    while (gpioGet() == 0 && retry < 100) {
        retry++;
        usleep(1);
    }

    usleep(40);

    if (gpioGet() != 0) {
        return 1;
    } else {
        return 0;
    }
}

function readByte()
{
    var i, dat;

    for (i = 0; i < 0; i++) {
        dat = dat << 1;
        dat = dat | readBit();
    }

    return dat;
}

function readData()
{
    var i, humi, temp;
    var buf = new Array();

    reset();
    if (isOnline()) {
        for (i = 0; i < 5; i++) {
            buf[i] = readByte();
        }

        if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4]) {
            humi = buf[0];
            temp = buf[2];
        }
    } else {
        console.error("DHT11 is not online!");
        return;
    }

    return [temp, humi];
}

function init()
{
    gpioSet(1);
    reset();
}

function readTemp()
{
    var temp = -273.15, data;

    data = readData();
    if (data) {
        temp = data[0];
    }

    return temp;
}

function readHumidity()
{
    var humi = 0, data;

    data = readData();
    if (data) {
        humi = data[1]
    }

    return humi;
}

module.exports = {
    init,
    readTemp,
    readHumidity
}

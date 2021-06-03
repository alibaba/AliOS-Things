// The driver for DS18B20 chip, it is a temperature sensor.

// Require libjs/lib/onewire.js module.
var onewire = require('onewire');

var DS18B20Dev;
var start_flag = 0;

/* Init DS18B20
 *
 * GPIO's options are configured in app.json. Specify the ID, e.g. GPIO below to init DS18B20.
 {
    "version": "1.0.0",
    "io": {
        "DS18B20": {
            "type": "GPIO",
            "port": 4,
            "dir": "output",
            "pull": "pullup"
        }
    },
    "debugLevel": "DEBUG"
 }

 */
function init(gpioid) {
    DS18B20Dev = onewire.open({id: gpioid});
}

// Get temperature
function getTemperature()
{
    var TL, TH;
    var tem;

   /*
    {
        DS18B20Dev.setspeed(1);
        start_flag = 1;
    } 

    DS18B20Dev.reset();

    DS18B20Dev.writeByte(0x55);*/

    if(1)
    {
        if(!start_flag)
        {
            DS18B20Dev.setspeed(1);
            DS18B20Dev.reset();
        
            DS18B20Dev.writeByte(0xcc);
            DS18B20Dev.writeByte(0x44);
    
            start_flag = 1;
        }
    
    
        DS18B20Dev.reset();
    
        DS18B20Dev.writeByte(0xcc);
        DS18B20Dev.writeByte(0xbe);
    
        TL = DS18B20Dev.readByte();    /* LSB first */
        TH = DS18B20Dev.readByte();
        
        if (TH > 7)
        {
            TH =~ TH;
            TL =~ TL;
            tem = TH;
            tem <<= 8;
            tem += TL;
            tem = (tem * 0.0625 * 10 + 0.5);
            return -tem;
        }
        else
        {
            tem = TH;
            tem <<= 8;
            tem += TL;
            tem = (tem * 0.0625 * 10 + 0.5);
            return tem;
        }
    }
 
}

// De-init Si7006
function deinit() {
    DS18B20Dev.close();
}

module.exports = {
    init,
    getTemperature,
    deinit
}

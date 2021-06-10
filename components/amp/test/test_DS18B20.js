
/*
Please add this section into app.json when run this script as app.js.
This configuration is designed for haas100 edu k1.
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

console.log('testing DS18B20...');

var DS18B20 = require('./DS18B20.js');

DS18B20.init("DS18B20");

var temperature;
var count = 10;

while(1)
{
    temperature = DS18B20.getTemperature();

    {
        console.log("Temperature is: " , temperature);
    }
}

DS18B20.deinit();
console.log("test DS18B20 success!");


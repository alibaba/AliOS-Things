
import * as DS18B20 from 'ds18b20'


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

DS18B20.init("DS18B20");

var temperature;
var count = 10;

while(count-- > 0)
{
    temperature = DS18B20.getTemperature();

    {
        console.log("Temperature is: " , temperature);
    }
}

DS18B20.deinit();
console.log("test DS18B20 success!");


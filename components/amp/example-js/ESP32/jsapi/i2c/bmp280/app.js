import * as bmp280 from './bmp280.js';
var temperature = 25.0;
var pressure = 0.0;
bmp280.bmp280_init();

setInterval(function(){
    temperature = bmp280.bmp280TemperatureRead();
    pressure = bmp280.bmp280PressureRead();
    pressure = pressure / 100; // iot needs hPa
    console.log('Temperature is ' + temperature + 'degrees, Pressure is ' + pressure + "hPa");
}, 5000);

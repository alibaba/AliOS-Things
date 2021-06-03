
/*
Please add this section into app.json when run this script as app.js.
This configuration is designed for haas100 edu k1.
"si7006": {
    "type": "I2C",
    "port": 1,
    "addrWidth": 7,
    "freq": 400000,
    "mode": "master",
    "devAddr": 64
}
*/

console.log('testing si7006...');

var si7006 = require('./si7006.js');

si7006.init("si7006");

var version = si7006.getVer();
console.log("Si7006 version is: " , version);

var chipID = si7006.getID();
console.log("Si7006 chipid is: " , chipID);

var temperature = si7006.getTemperature();
console.log("Temperature is: " , temperature);

var humidity = si7006.getHumidity();
console.log("Humidity is: " , humidity);

si7006.deinit();
console.log("test si7006 success!");

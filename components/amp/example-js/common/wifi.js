console.log('wifi info get test')

WIFI.connect("aiot", "iot123456", function(){
    console.log('wifi connect');
});

var ssid = WIFI.getInfo().ssid;
var ip = WIFI.getInfo().ip;
var mac = WIFI.getInfo().mac;
var rssi = WIFI.getInfo().rssi;

console.log('ssid is ' + ssid);
console.log('ip is ' + ip);
console.log('mac is ' + mac);
console.log('rssi is ' + rssi);

WIFI.disconnect();
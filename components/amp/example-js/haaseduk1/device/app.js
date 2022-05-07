var device = require('device');
var token = device.getToken();
var value = '1234567890abc';
if (token === undefined) {
    console.log('found no token in kv, write new token.');
    device.setToken(value);
    console.log('write token: ' + value);
    if (value === device.getToken()) {
        console.log('read token and test pass: ' + device.getToken());
    }
} else {
    console.log('read token: ' + device.getToken());
}
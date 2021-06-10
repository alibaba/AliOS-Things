import * as netmgr from 'netmgr';
console.log('hello amp!');
var network = netmgr.openNetMgrClient({
  name: '/dev/wifi0'
});
var status;
status = network.getState();
console.log('status is ' + status);
network.connect({
  ssid: 'own_ssid',             //请替换为自己的热点ssid
  password: 'own_password'      //请替换为自己热点的密码
});
network.on('error', function () {
  console.log('error ...');
});
network.on('connect', function () {
  console.log('net connect success');
});

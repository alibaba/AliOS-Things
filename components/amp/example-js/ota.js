var appota = require('appota');
var iot = require('iot');

/* device info */
var productKey = '';      /* your productKey */
var deviceName = '';      /* your deviceName */
var deviceSecret = '';  /* your deviceSecret */
var module_name = 'default';
var default_ver = '1.0.0';

var ota;
var status;
/* download info */
var info = {
	url: '',
	store_path: '/data/jsamp/pack.bin',
	install_path: '/data/jsamp/',
	length: 0,
	hashType: '',
	hash: ''
}

var device = iot.device({
	productKey: productKey,
	deviceName: deviceName,
	deviceSecret: deviceSecret
});

device.on('connect', function(iot_res) {
	console.log('device connected');
	var iotDeviceHandle = device.getDeviceHandle();
	ota = appota.open(iotDeviceHandle);
	console.log('report default module ver');
	ota.report({
		device_handle: iotDeviceHandle,
		product_key: productKey,
		device_name: deviceName,
		module_name: module_name,
		version: default_ver
	});
	ota.on('new', function(res) {
		console.log('length is ' + res.length);
		console.log('module_name is ' + res.module_name);
		console.log('version is ' + res.version);
		console.log('url is ' + res.url);
		console.log('hash is ' + res.hash);
		console.log('hash_type is ' + res.hash_type);

		info.url = res.url;
		info.length = res.length;
		info.module_name = res.module_name;
		info.version = res.version;
		info.hash = res.hash;
		info.hashType = res.hash_type;

		ota.download({
			url: info.url,
			store_path: info.store_path
		}, function(res) {
			if (res >= 0) {
				console.log('download success');
				console.log('verify start');
				console.log(info.hashType);
				ota.verify({
					length: info.length,
					hash_type: info.hashType,
					hash: info.hash,
					store_path: info.store_path
				}, function(res) {
					if (res >= 0) {
						console.log('verify success');
						console.log('upgrade start');
						ota.upgrade({
							length: info.length,
							store_path: info.store_path,
							install_path: info.install_path
						}, function(res) {
							if (res >= 0) {
								console.log('upgrade success')
							} else {
								console.log('upgrade failed')
							}
						}) 
					} else {
						console.log('verify failed');
					}
				})
			} else {
				console.log('download failed');
			}
		});
	});
})


import * as netmgr from 'netmgr';
import * as oss from 'oss';

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


var oss_key = ''   // 请替换为自己的OSS KEY
var oss_secret = '' //  请替换为自己的OSS secret
var endpoint = '' //请替换为自己 oss endpoint
var bucket_name = '' //请替换为自己 oss bucketname
// 'oss_filename_path' 替换为oss上存放的文件名及路径，比如 test/test.jpg
// 'your_local_file_name' 替换为设备本地路径及文件名，比如 /data/test.jpg
network.on('connect', function () {
  console.log('net connect success');
  var url = oss.uploadFile(oss_key, oss_secret, endpoint, bucket_name, 'oss_filename_path', 'your_local_file_name'); 
  console.log('upload url: ' + url)

  console.log('download begin');
  oss.downloadFile(oss_key, oss_secret, endpoint, bucket_name, 'oss_filename_path', 'your_local_file_name');
  console.log('oss download success ')
});


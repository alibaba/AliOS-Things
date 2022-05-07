var network = require('network');
var net = network.openNetWorkClient({devType: 'ethnet'});

var netType = net.getType();
console.log('net type is: ' + netType);

var netStatus = net.getStatus();
console.log('net status is: ' + netStatus);

if (netStatus == 'connect') {
    /* 网络状态已连接，获取网络状态 */
    console.log('net status already connected');
    getNetInfo();
 } else {
  /* wifi或者以太网，设置ifconfig */
  if (netType === "wifi" || netType === "ethnet") {
    console.log('net setIfConfig');
    net.setIfConfig({
      dhcp_en: true,                   // true: 动态IP模式(以下参数可忽略）; false: 静态IP模式（以下参数必填）
      ip_addr: '172.20.10.66',
      mask: '255.255.255.0',
      gw: '172.20.10.1',
      dns_server: '8.8.8.8'
    });
  }

  /**
    * 监听网络连接成功事件，成功执行回调函数
    */
  net.on('connect', function () {
    console.log('network connected');
    getNetInfo();
  });

  net.on('disconnect', function () {
  console.log('network disconnected');
  });
}

function getNetInfo() {
  console.log('net status is connected, begin getting net information...');
  var info = net.getInfo();
  if (netType === "wifi" || netType === "ethnet") {
    /* 是否开启dhcp */
    console.log('net dhcp_en is: ' + info.netInfo.dhcp_en);

    /* ip地址*/
    console.log('net ip_addr is: ' + info.netInfo.ip_addr);

    /* dns */
    console.log('net dns_server is: ' + info.netInfo.dns_server);

    /* 网关 */
    console.log('net gateway is: ' + info.netInfo.gw);

    /* 子网掩码 */
    console.log('net mask is: ' + info.netInfo.mask);

    /* mac地址 */
    console.log('net mac is: ' + info.netInfo.mac);

    /* wifi信号强度 */
    console.log('net wifi rssi is: ' + info.netInfo.rssi);

    return;
  }
  console.log('unkown net type');
}

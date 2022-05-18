var http = require('http');
var network = require('network');
var net = network.openNetWorkClient({devType: 'wifi'});

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
      dhcp_en: true,             // true: 动态IP模式(以下参数可忽略）; false: 静态IP模式（以下参数必填）
      ip_addr: '192.168.124.66',
      mask: '255.255.255.0',
      gw: '192.168.124.1',
      dns_server: '8.8.8.8'
    });
  }

  /* 网络状态未连接，如果是wifi设备则主动联网(4G模组中系统会自动注网) */
  if (netType === "wifi") {
    net.connect({
      ssid: 'xxx',       //请替换为自己的热点ssid
      password: 'xxx'       //请替换为自己热点的密码
    });
  }

  /**
    * 监听网络连接成功事件，成功执行回调函数
    */
  net.on('connect', function () {
    console.log('network connected');
    getNetInfo();

    // start http test
    http_test();
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

function http_test() {
    // request POST example
  http.request({
    url: 'https://www.ixigua.com/tlb/comment/article/v5/tab_comments/',
    method: 'POST',
    headers: {
      'content-type':'application/x-www-form-urlencoded'
    },
    params: 'tab_index=0&count=3&group_id=6914830518563373582&item_id=6914830518563373581&aid=1768',

    success: function (data) {
      console.log('http POST: [debug] receive data is ' + data);
    }
  });
}

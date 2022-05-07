var network = require('network');
var net = network.openNetWorkClient({devType: 'cellular'});

var netType = net.getType();
console.log('net type is: ' + netType);

var netStatus = net.getStatus();
console.log('net status is: ' + netStatus);

if (netStatus == 'connect') {
    /* 网络状态已连接，获取网络状态 */
    console.log('net status already connected');
    getNetInfo();
 } else {
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
  if (netType === "cellular") {
    /* imsi 国际移动用户识别码 */
    console.log('net simInfo imsi is: ' + info.simInfo.imsi);

    /* imei 国际移动设备识别码 */
    console.log('net simInfo imei is: ' + info.simInfo.imei);

    /* iccid 集成电路卡识别码 */
    console.log('net simInfo iccid is: ' + info.simInfo.iccid);

    /* cid 基站编号 */
    console.log('net  locatorInfo cellid is: ' + info.locatorInfo.cellid);

    /* lac 位置区域码 */
    console.log('net  locatorInfo lac is: ' + info.locatorInfo.lac);

    /* mcc 移动国家代码（中国的为460 */
    console.log('net  locatorInfo mcc is: ' + info.locatorInfo.mcc);

    /* mnc 移动网络号码（中国移动为00，中国联通为01） */
    console.log('net  locatorInfo mnc is: ' + info.locatorInfo.mnc);

    /* signal 接收的信号强度值 */
    console.log('net  locatorInfo signal is: ' + info.locatorInfo.signal);
    return;
  }
  console.log('unkown net type');
}

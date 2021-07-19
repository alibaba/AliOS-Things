import * as netmgr from 'netmgr';
import * as iot from 'iot';
import * as ulog from 'ulog';

var productKey = '*****';      /* 输入你的product key*/
var deviceName = '*****';      /* 输入你的device name */
var deviceSecret = '*****';    /* 输入你的deviceSecret */
var device;

/* set different log level */
/*设置本地日志输出等级为debug*/
ulog.stdloglevel('debug');
/*设置上报到云端的等级为debug*/
ulog.cloudloglevel('debug');

ulog.info('hello HaaS amp ulog!');

var network = netmgr.openNetMgrClient({
  name: '/dev/wifi0'
});

network.on('error', function () {
    ulog.error('netowrk error ...');
});

network.on('connect', function () {
    ulog.info('net connect success');
    createDevice();
});

var status;
status = network.getState();

if (status == 'connect') {
    /*network is connect  */
    log.info('network is connected try to connect to aliyun iot platform');
    createDevice();
} else {
    network.connect({
        ssid: '*****',             //请替换为自己的热点ssid
        password: '****'      //请替换为自己热点的密码
    });
}

function createDevice() {
    device = iot.device({
        productKey: productKey,
        deviceName: deviceName,
        deviceSecret: deviceSecret,
        region: 'cn-shanghai',
    });

    device.on('connect', function () {
        ulog.info('iot connect successed start to pop different log level output');
        var log_count = 0;
        setInterval(function(){
          log_count++;
          /*输出调试日志*/
          ulog.debug('ulog test demo debug log output count ' + log_count);
          if (log_count % 2 == 0) {
            /*输出信息级别日志*/
            ulog.info('ulog test demo info log output count ' + log_count/2);
          }
          if (log_count % 3 == 0) {
            /*输出告警级别日志*/
            ulog.warn('ulog test demo warn log output count ' + log_count/3);
          }
          if (log_count % 4 == 0) {
            /*输出错误级别日志*/
            ulog.error('ulog test demo error log output count ' + log_count/4);
          }
          if (log_count % 5 == 0) {
            /*输出致命级别日志*/
            ulog.fatal('ulog test demo fatal log output count ' + log_count/5);
          }
          }, 2000);
    });

}

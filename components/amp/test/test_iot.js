console.log('iot: testing iot...');
var iot = require('iot');

if (!(iot && iot.device)) {
    throw new Error("iot: [failed] require(\'iot\')");
}

console.log('iot: [success] require(\'iot\')');

var productKey = 'a10h6IdruJf';
var deviceName = 'nodemcu_02';
var deviceSecret = 'StFYN4UigwDiRbhRwVFkFIKsEqpuaHKu';

var device = iot.device({
    productKey: productKey,
    deviceName: deviceName,
    deviceSecret: deviceSecret,
    region: 'cn-shanghai',
    success: function () {
        console.log('iot: [success] connect');
        onConnect();
    },
    fail: function () {
        console.log('iot: [failed] connect');
    }
});

var postCnt = 10;

var lightSwitch = 0;
function onConnect() {
    var intervalHandle = setInterval(function () {
        postCnt--;
        if(postCnt <= 0) {
            clearInterval(intervalHandle);
        }
        /** post properties */
        lightSwitch = 1 - lightSwitch;
        device.postProps({
            payload: {
                LightSwitch: lightSwitch
            },
            success: function () {
                console.log('iot: [success] iot.postProps cnt: ' + postCnt);
            },
            fail: function () {
                console.log('iot: [failed] iot.postProps cnt: ' + postCnt);
            }
        });
        /** post events */
        device.postEvent({
            id: 'Error',
            params: {
                ErrorCode: 0
            },
            success: function () {
                console.log('iot: [success] iot.postEvent cnt: ' + postCnt);

            },
            fail: function () {
                console.log('iot: [failed] iot.postEvent cnt: ' + postCnt);
            }
        });
    }, 1000);
}

device.on('connect', function () {
    console.log('iot: [success] iot.on(\'connect\')');
});

/* 网络断开事件 */
device.on('disconnect', function () {
    console.log('iot: [success] iot.on(\'disconnect\')');
});

/* 关闭连接事件 */
device.on('close', function () {
    console.log('iot: [success] iot.on(\'close\')');
});

/* 发生错误事件 */
device.on('error', function (err) {
    throw new Error('iot: [failed] iot.on(\'error\') ' + err);
});

/* 云端设置属性事件 */
device.on('props', function (payload) {
    console.log('iot: [success] iot.on(\'props\'), payload: ' + JSON.stringify(payload));
});

/* 云端下发服务事件 */
device.on('service', function (id, payload) {
    console.log('iot: [success] iot.on(\'service\'), id: ' + id + ', payload: ' + JSON.stringify(payload));
});
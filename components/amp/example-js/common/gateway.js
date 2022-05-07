// import * as iot from 'iot'
import * as iot from 'gateway'

var productKey = 'a17rAb1ltpS';
var deviceName = '123454';
var deviceSecret = 'abf0028f3d7ccf2b9c555826f32381d8';

var lightSwitch = 0;
var gateway;

var subdev = [
    {
        productKey: 'a1zK6eTWoZO',
        deviceName: 'test101',
        deviceSecret: 'caf25b3839a24a772be3d386f6c5bd74'
    },
    {
        productKey: 'a1zK6eTWoZO',
        deviceName: 'test100',
        deviceSecret: '253ab8d388094c0087c4c9a2c58b7373'
    }
];

/* post property */
function postProperty(id, devInfo, content) {
    var topic = '/sys/' + devInfo.productKey + '/' + devInfo.deviceName + '/thing/event/property/post';
    var payload = JSON.stringify({
        id: id,
        version: '1.0',
        params: content,
        method: 'thing.event.property.post'
    });

    gateway.publish({
        topic: topic,
        payload: payload,
        qos: 1
    });
}

/* post event */
function postEvent(id, devInfo, eventId, content) {
    var topic = '/sys/' + devInfo.productKey + '/' + devInfo.deviceName + '/thing/event/' + eventId + '/post';
    var payload = JSON.stringify({
        id: id,
        version: '1.0',
        params: content,
        method: 'thing.event.' + eventId + '.post'
    });

    gateway.publish({
        topic: topic,
        payload: payload,
        qos: 1
    });
}

function createGateway() {
    gateway = iot.gateway({
        productKey: productKey,
        deviceName: deviceName,
        deviceSecret: deviceSecret
    });

    gateway.on('connect', function () {
        console.log('(re)connected');

        // gateway.addTopo(subdev);

        gateway.login(subdev);

        setTimeout(function () {
            postProperty(1, subdev[0], { 'PowerSwitch_1': lightSwitch });
            postEvent(1, subdev[0], 'Error', { 'ErrorCode': 0 });
        }, 2000);
    });

    /* 网络断开事件 */
    gateway.on('disconnect', function () {
        console.log('disconnect ');
    });

    /* mqtt消息 */
    gateway.on('message', function (res) {
        /* 通过 topic中的pk和dn信息，判断是对哪个子设备的调用 */
        var pk = res.topic.split('/')[2];
        var dn = res.topic.split('/')[3];
        console.log('mqtt message')
        console.log('mqtt topic is ' + res.topic);
        console.log('PK: ' + pk)
        console.log('DN: ' + dn)
        console.log('mqtt payload is ' + res.payload);
    })

    /* 关闭连接事件 */
    gateway.on('end', function () {
        console.log('iot client just closed');
    });

    /* 发生错误事件 */
    gateway.on('error', function (err) {
        console.log('error ' + err);
    });
}


console.log('====== create aiot gateway ======');
createGateway();



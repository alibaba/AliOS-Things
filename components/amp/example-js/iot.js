import * as iot from 'iot'

var productKey = 'a1MOapE3PH8';      /* your productKey */
var deviceName = 'FjWNmTsPWfPSykfkBv1l';      /* your deviceName */
var deviceSecret = '2a6afba08a135efda3c49e4cc7684254';  /* your deviceSecret */
var device ;

var topic = '/sys/' + productKey + '/' + deviceName + '/user/haas/info';

function createDevice() {
  device = iot.device({
    productKey: productKey,
    deviceName: deviceName,
    deviceSecret: deviceSecret,
    region: 'cn-shanghai',
  });

  device.on('connect', function () {
    console.log('(re)connected');

    device.subscribe({
        topic: topic,
        qos: 0
    });

    device.unsubscribe({
      topic: topic
    });

    device.publish({
        topic: topic,
        payload: 'haas haas haas',
        qos: 1
    });

    var lightswitch = 0;
    var eventid = 'alarmEvent';

    setInterval(function () {
        if (lightswitch) {
            lightswitch = 0;
        } else {
            lightswitch = 1;
        }

        /* post props */
        var propertyPayload = JSON.stringify({
            LightSwitch: lightswitch
        });
        device.postProps({
          payload: propertyPayload
        });

        /* post event */
        var eventPayload = JSON.stringify({
            alarmType: 0
        });
        device.postEvent({
            id: eventid,
            params: eventPayload
        });
    }, 3000);

    /* 云端设置属性事件 */
    device.onProps(function (res) {
        console.log('cloud req msg_id is ' + res.msg_id);
        console.log('cloud req params_len is ' + res.params_len);
        console.log('cloud req params is ' + res.params);
    });

    /* 云端下发服务事件 */
    device.onService(function (res) {
        console.log('received cloud msg_id is ' + res.msg_id);
        console.log('received cloud service_id is ' + res.service_id);
        console.log('received cloud params_len is ' + res.params_len);
        console.log('received cloud params is ' + res.params);
    });
  });
}
console.log('====== create aiot device ======');
createDevice();



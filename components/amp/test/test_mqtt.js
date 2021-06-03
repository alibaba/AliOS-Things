function ArrayToString(fileData) {
    var dataString = "";
    for (var i = 0; i < fileData.length; i++) {
        dataString += String.fromCharCode(fileData[i]);
    }

    return dataString;
}

console.log('mqtt: testing mqtt...');

var mqtt = require('mqtt');
if (!(mqtt && mqtt.createClient)) {
    throw new Error("mqtt: [failed] require(\'mqtt\')");
}

console.log('mqtt: [success] require(\'mqtt\')');

function testUnsubscribe() {
    return new Promise(function (resolve, reject) {
        mqttClient.subscribe({
            topic: '/amp-test',
            success: function () {
                console.log('mqtt: [success] mqtt subscribe');
                mqttClient.unsubscribe({
                    topic: '/amp-test',
                    success: function () {
                        console.log('mqtt: [success] mqtt unsubscribe');
                        resolve();
                    },
                    fail: function () {
                        console.log('mqtt: [failed] mqtt unsubscribe');
                        reject();
                    }
                });
            },
            fail: function () {
                console.log('mqtt: [failed] mqtt subscribe');
                reject();
            }
        });
    });
}

function testOnMessage() {
    return new Promise(function (resolve, reject) {
        var testTopic = '/amp-hello';
        var recvCnt = 0, sendCnt = 0;
        var defaultMessage = 'AMP mqtt reply';
        mqttClient.subscribe({
            topic: testTopic
        });
        mqttClient.on('message', function (topic, payload) {
            recvCnt++;
            console.log('mqtt: [debug] mqtt on message, topic[' + topic + ']: ' + ArrayToString(payload) + ', cnt: ' + recvCnt);

            if (!topic || typeof topic !== 'string' || !payload || typeof payload !== 'object') {
                console.log("mqtt: [failed] mqtt on(\'message\')");
                reject();
            }

            if (topic !== testTopic || ArrayToString(payload) !== defaultMessage) {
                console.log("mqtt: [failed] mqtt on(\'message\')");
                reject();
            }
            if (recvCnt >= 10) {
                mqttClient.close();
                console.log('mqtt: [success] mqtt.on(\'message\')');
                console.log('mqtt: testing mqtt success');
                resolve();
            }
        });

        setTimeout(function () {
            if (recvCnt != 10) {
                console.log('mqtt: [failed] mqtt on message');
                reject();
            }
        }, 12000);

        var intervalHandle = setInterval(function () {
            mqttClient.publish({
                topic: testTopic,
                message: defaultMessage,
                success: function () {
                    sendCnt++;
                    console.log("mqtt: [debug] publish success, cnt: ", sendCnt);
                    if (sendCnt >= 10) {
                        clearInterval(intervalHandle);
                        console.log('mqtt: [success] mqtt.publish()');
                        resolve();
                    }
                },
                fail: function () {
                    console.log("mqtt: [failed] mqtt.publish()");
                    reject();
                }
            });
        }, 1000);
    });
}

function onConnect() {
    testUnsubscribe().then(function () {
        return testOnMessage().then(function () {
            console.log('mqtt: testing mqtt success');
        });
    }).catch(function () {
        console.log('mqtt: testing mqtt failed');
    });
}

var mqttClient = mqtt.createClient({
    host: 'mqtt.eclipse.org',
    port: 1883,
    username: 'aiot',
    password: '123',
    success: function () {
        console.log('mqtt: [debug] mqtt connected');
    },
    fail: function () {
        console.log('mqtt: [debug] mqtt connect failed');
    }
});

mqttClient.on('connect', function () {
    console.log('mqtt: [success] mqtt.on(\'connect\')');
    onConnect();
});

mqttClient.on('disconnect', function () {
    console.log('mqtt: [success] mqtt.on(\'disconnect\')');
});

mqttClient.on('close', function () {
    console.log('mqtt: [success] mqtt.on(\'close\')');
});
var mqtt = require('mqtt');

function ArrayToString(fileData) {
    var dataString = "";
    for (var i = 0; i < fileData.length; i++) {
        dataString += String.fromCharCode(fileData[i]);
    }

    return dataString;
}

function onConnect() {
    // subscribe
    mqttClient.subscribe({
        topic: '/hello',
        success: function() {
            console.log('subscribe [/hello] success');
        }
    });
    
    // publish
    setInterval(function () {
        mqttClient.publish({
        topic: '/hello',
        message: 'this is AMP mqtt test',
        success: function() {
            console.log('publish [/hello] success');
        }
        });
    }, 2 * 1000);
}

// create mqtt client
var mqttClient = mqtt.createClient({
  host: 'mqtt.eclipse.org',
  port: 1883,
  username: 'aiot',
  password: '123',
  success: function() {
    console.log('mqtt connected');
  },
  fail: function() {
    console.log('mqtt connect failed');
  }
});

mqttClient.on('connect', function() {
    console.log('mqtt connected');
    onConnect();
});

mqttClient.on('message', function(topic, payload) {
  console.log('[' + topic + ']  message: ' + ArrayToString(payload));
});
var udp = require('udp');

var udpSocket = udp.createSocket();

function arrayToString(fileData) {
    var dataString = "";
    for (var i = 0; i < fileData.length; i++) {
        dataString += String.fromCharCode(fileData[i]);
    }

    return dataString;
}

udpSocket.on('message', function(data, rinfo) {
  console.log('receive data from ' + rinfo.host + ' port: ' + rinfo.port);
  console.log('udp socket just receive data is: ' + arrayToString(data));
  udpClient.close();
});

udpSocket.on('close', function() {
  console.log('udp close');
});

udpSocket.on('error', function(err) {
  console.log('udp error ' + err);
});

setInterval(function() {
    udpSocket.send({
        address: '127.0.0.1',
        port: 50000,
        message: 'hello, this is amp',
        success: function() {
          console.log('send success');
        }
    });
}, 2000);


// bind port & start to receive message
udpSocket.bind();
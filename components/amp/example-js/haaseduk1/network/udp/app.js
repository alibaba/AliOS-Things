var udp = require('udp');

var udpSocket = udp.createSocket();

// bind port
udpSocket.bind(8061);

udpSocket.on('send', function() {
  console.log('udp data send');
});

udpSocket.on('message', function(data, rinfo) {
  console.log('receive data from ' + rinfo.host + ' port: ' + rinfo.port);
  console.log('udp socket just receive data is: ' + data);
  udpSocket.close();
});

udpSocket.on('close', function() {
  console.log('udp close');
});

udpSocket.on('error', function(err) {
  console.log('udp error ' + err);
});

udpSocket.send({
  address: '192.168.193.2',
  port: 8848,
  message: 'hello, this is amp'
});
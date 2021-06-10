var net = require('net');

var HOST = '100.81.240.32';
var PORT = 50000;

var server = net.createServer();
server.listen(PORT, HOST);
// console.log(server);
console.log('Server listening on ' + HOST + ':' + PORT);

server.on('connection', function(sock) {
  console.log('CONNECTED: ' + sock.remoteAddress +':'+ sock.remotePort);
  // 为这个socket实例添加一个"data"事件处理函数
  sock.on('data', function(data) {
    console.log('DATA ' + sock.remoteAddress + ': ' + data);
    // 回发该数据，客户端将收到来自服务端的数据
    sock.write('AMP tcp server reply');
  });

  // 为这个socket实例添加一个"close"事件处理函数
  sock.on('close', function(data) {
    console.log('CLOSED: ' +
        sock.remoteAddress + ' ' + sock.remotePort);
  });

});
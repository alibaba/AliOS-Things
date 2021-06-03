/* 本测试case，测试tcp的接口，接口返回值等，tcp server是公网tcp服务器：47.101.151.113 */
function ArrayToString(fileData){
  var dataString = "";
  for (var i = 0; i < fileData.length; i++) {
    dataString += String.fromCharCode(fileData[i]);
  }
 
  return dataString;
}

console.log('tcp: testing tcp...');
// 测试 tcp 模块
var tcp = require('tcp');
if(!(tcp && tcp.createClient)){
  throw new Error("tcp: [failed] require(\'tcp\')");
}

console.log('tcp: [success] require(\'tcp\')');

var tcpSocket = tcp.createClient({
  host: '47.101.151.113',
  port: 50020,
  success: function() {
    console.log('tcp: tcp client connect success');
  },
  fail: function() {
    console.log('tcp: tcp client connect failed');
  }
});
// 测试 tcpSocket 接口
if(!tcpSocket || !tcpSocket.send || !tcpSocket.close || !tcpSocket.on || !tcpSocket.reconnect) {
  throw new Error("tcp: [failed] tcp instance");
}

// 测试接收数据
var sendCnt = 0, recvCnt = 0;
var defaultMessage = 'AMP tcp server reply';
tcpSocket.on('message', function(message) {
  if(!message || typeof message !== 'object' || ArrayToString(message) !== defaultMessage) {
      throw new Error("tcp: [failed] tcp receive message");
  }
  recvCnt ++;
  console.log('tcp: [debug] on message: ' + ArrayToString(message) + ', cnt: ' + recvCnt);
  if(recvCnt >= 10){
      tcpSocket.close();
      console.log('tcp: [success] tcp.on(\'message\')');
      console.log('tcp: testing tcp success');
  }
});

// 接收数据超时
setTimeout(function() {
  if(recvCnt != 10) {
      console.log('tcp: [failed] tcp on message');
  }
}, 12000);

// 测试socket异常
tcpSocket.on('error', function(err) {
  console.log('tcp: tcp error ', err);
  tcpSocket.close();
  throw new Error("tcp: [failed] tcp on error");
});

tcpSocket.on('disconnect', function() {
  console.log('tcp: tcp disconnected');
});

// 测试发送数据
var intervalHandle = setInterval(function() {
  tcpSocket.send({
      message: "this is xxw, cnt: " + sendCnt,
      success: function() {
          sendCnt ++;
          console.log("tcp: [debug] send success, cnt: ", sendCnt);
          // 退出发送
          if(sendCnt >= 10){
              clearInterval(intervalHandle);
              console.log('tcp: [success] tcp.send()');
          }
      },
      fail: function() {
          console.log("tcp: send fail");
          throw new Error("tcp: [failed] tcp send fail");
      }
  });
}, 1000);
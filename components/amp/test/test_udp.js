/* 本测试case，测试udp的接口，接口返回值等，udp server是公网udp服务器：47.101.151.113 */
function ArrayToString(fileData){
    var dataString = "";
    for (var i = 0; i < fileData.length; i++) {
      dataString += String.fromCharCode(fileData[i]);
    }
   
    return dataString;
  }

console.log('udp: testing udp...');
// 测试 udp 模块
var udp = require('udp');
if(!(udp && udp.createSocket)){
    throw new Error("udp: [failed] require(\'udp\')");
}

console.log('udp: [success] require(\'udp\')');

var udpSocket = udp.createSocket();
// 测试 UDPSocket 接口
if(!udpSocket || !udpSocket.bind || !udpSocket.send || !udpSocket.close || !udpSocket.on) {
    throw new Error("udp: [failed] udp instance");
}

// 测试接收数据
var sendCnt = 0, recvCnt = 0;
var defaultMessage = 'AMP udp server reply';
udpSocket.on('message', function(message, rinfo) {
    if(!message || typeof message !== 'object' || ArrayToString(message) !== defaultMessage) {
        throw new Error("udp: [failed] udp receive message");
    }
    if(!rinfo || !rinfo.host || !rinfo.port) {
        throw new Error("udp: [failed] udp receive rinfo");
    }
    recvCnt ++;
    console.log('udp: [debug] remote[' + rinfo.host + ': ' + rinfo.port + '] on message: ' + ArrayToString(message) + ', cnt: ' + recvCnt);
    if(recvCnt >= 10){
        udpSocket.close();
        console.log('udp: [success] udp.on(\'message\')');
        console.log('udp: testing udp success');
    }
});

// 接收数据超时
setTimeout(function() {
    if(recvCnt != 10) {
        console.log('udp: [failed] udp on message');
    }
}, 12000);

// 测试socket异常
udpSocket.on('error', function(err) {
    console.log('udp error ', err);
    throw new Error("udp: [failed] udp on error");
});

// 测试发送数据
var intervalHandle = setInterval(function() {
    udpSocket.send({
        address: "47.101.151.113",
        port: 50000,
        message: "this is xxw, cnt: " + sendCnt,
        success: function() {
            sendCnt ++;
            console.log("udp: [debug] send success, cnt: ", sendCnt);
            // 退出发送
            if(sendCnt >= 10){
                clearInterval(intervalHandle);
                console.log('udp: [success] udp.send()');
            }
        },
        fail: function() {
            console.log("udp: send fail");
            throw new Error("udp: [failed] udp send fail");
        }
    });
}, 1000);

udpSocket.bind(10240);
if(!udpSocket.localPort || typeof udpSocket.localPort !== 'number') {
    throw new Error("udp: [failed] udp localPort");
}

console.log('udp: [success] udp.localPort');
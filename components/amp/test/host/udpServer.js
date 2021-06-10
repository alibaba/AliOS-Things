const dgram = require('dgram'); 
const server = dgram.createSocket('udp4'); //创建udp服务器

//以下server.on 都是在监听不同信号
server.on('close',()=>{
    console.log('socket已关闭');
});

server.on('error',(err)=>{
    console.log(err);
});

server.on('listening',()=>{
    console.log('socket正在监听中...');
});

server.on('message',(msg,rinfo)=>{
    console.log(`receive message: ${msg} from ${rinfo.address}:${rinfo.port}`);
    var message = 'AMP udp server reply';
    server.send(message,rinfo.port,rinfo.address);
});

server.bind(50000); //绑定端口，不绑定的话也可以send数据但是无法接受


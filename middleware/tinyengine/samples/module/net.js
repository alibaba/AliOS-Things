var ssid = "Xiaomi_296E_rock";
var passwd = "rockzhou";
WIFI.connect(ssid,passwd,function(state){console.log('wifi state:'+state);
var ip = WIFI.getip();
console.log('WIFI state getip ='+ip);
var mac = WIFI.getmac();
console.log('WIFI.getmac:'+mac);
var channel = WIFI.getchannel();
console.log('WIFI.getchannel:'+channel);
if (state == 'CONNECTED'){
	HTTP.request("http://www.baidu.com",function(result){
		console.log('http requst reulst=:'+result);
	});
    /*you can replace to udp with udp://ip:port*/
	NET.connect({addr: 'tcp://192.168.8.144:80',  
    onconnect: function(conn) {console.log('onconnect :'+conn);
    	NET.send(conn,"i am wuzhun");
	},  
    ondata: function(conn, data){
    	console.log('ondata :'+conn +'data= '+data);
    }, 
    onclose: function(conn) {
    	console.log('onclose :'+conn);
    }, 
    onerror: function(conn) {
    	console.log('onerror :'+conn);
    }, 
	});
	}
});
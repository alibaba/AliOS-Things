WIFI.scan(function(result){ 
	for (var i in result){
	console.log('WIFI.scan callback enter,ssid='+result[i].ssid+' chanenl='+result[i].channel); }
});

var ssid = "Xiaomi_296E_rock";
var passwd = "rockzhou";
WIFI.connect(ssid,passwd,function(state){console.log('wifi state:'+state);
var ip = WIFI.getip();
console.log('WIFI state getip ='+ip);
var mac = WIFI.getmac();
console.log('WIFI.getmac:'+mac);
var channel = WIFI.getchannel();
console.log('WIFI.getchannel:'+channel);
var ret = WIFI.setchannel(11);
console.log('WIFI.setchanenl return:'+ret);
});
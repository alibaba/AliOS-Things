
var times = 1;

setInterval(function() {
  	console.log('http timers ='+times);
    HTTP.request("http://www.baidu.com",function(result){
		console.log('http requst reulst=:'+result);
	});
    times=times+1;
}, 30000);

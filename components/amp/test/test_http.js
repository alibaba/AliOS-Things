console.log('http: testing http...');

var http = require('http');
if (!(http && http.request)) {
    throw new Error("http: [failed] require(\'http\')");
}

console.log('http: [success] require(\'http\')');


var request_url = 'http://appengine.oss-cn-hangzhou.aliyuncs.com/httpTest.txt';
var defaultMessage = 'this is AMP HTTP test file';

http.request({
    url: request_url,
    method: 'GET',
    headers: {
      'content-type':'application/json'
    },
    success: function (data) {
      console.log('http: [debug] receive data is ' + data);
      if(data === defaultMessage) {
          console.log('http: [success] http.request');
      }
    }
});
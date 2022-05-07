console.log('http: testing http...');

var http = require('http');
if (!(http && http.request)) {
    throw new Error("http: [failed] require(\'http\')");
}

console.log('http: [success] require(\'http\')');


// request GET example
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


// request POST example
// http.request({
//   url: 'https://www.ixigua.com/tlb/comment/article/v5/tab_comments/',
//   method: 'POST',
//   headers: {
//     'content-type':'application/x-www-form-urlencoded'
//   },
//   params: 'tab_index=0&count=3&group_id=6914830518563373582&item_id=6914830518563373581&aid=1768',
 
//   success: function (data) {
//     console.log('http: [debug] receive data is ' + data);
//   }
// });


// download example
// http.download({
//     url:'http://wangguan-498.oss-cn-beijing.aliyuncs.com/SHOPAD/public/mould5.png',
//     filepath:'/data/http_download_test.png',
//     success: function (data) {
//       console.log('http: [debug] downlad is ' + data);
//     }
// });
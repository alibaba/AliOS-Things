console.log(Promise);

function test_setTimeout() {
    return new Promise(function (resolve, reject) {
        console.log('=================setTimeout');
        setTimeout(function () {
            console.log('=================setTimeout---------------done');
            resolve("");
        }, 1000);
    });
}

function test_http() {
    return new Promise(function (resolve, reject) {
        console.log('=================http');
        HTTP.request("http://appengine.oss-cn-hangzhou.aliyuncs.com/httpTest.txt", function(data){
            resolve(data);
        });
    });
}

test_setTimeout()
.then(function(){
    // setInterval异步测试返回
    return test_http();
})
.then(function(data){
    // http异步测试返回
    console.log("接收到 HTTP 数据：", data);
});
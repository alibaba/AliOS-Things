
/*
  模块测试,  Modules.addCached 通过JS程序注册模块,代替从文件中读取
*/
var count=0;

Modules.addCached("a","module.exports.foo=1;");
var ta = require("a").foo;
count += ta;
console.log('ta=' + ta);

Modules.addCached("b","module.exports = {foo:1};");
var tb = require("b").foo;
count += tb;
console.log('tb=' + tb);

Modules.addCached("c","module.exports = 1;");
var tc = require("c");
count += tc;
console.log('tc=' + tc);

Modules.addCached("d","exports.foo = 1;");
var td = require("d").foo;
count += td;
console.log('td=' + td);

Modules.addCached("e","exports.foo = function(){ return 1;};");
var te = require("e").foo();
count += te;
console.log('te=' + td);


console.log(count);
console.log('Modules.addCache 测试'  +  ( (count == 5) ? '正确' : '失败'));



var myModule = "function myModule() {" +
"  this.hello = function() {" +
"    return 'hello!'; " +
"  }; " +
"  this.goodbye = function() {" +
"    return 'goodbye!';" +
"  }; " +
"}; " +
"module.exports = myModule; " ;

//console.log('myModule=' + myModule);

Modules.addCached("myModule", myModule);

myModule = require('myModule');
var myModuleInstance = new myModule();
console.log(myModuleInstance.hello());
console.log(myModuleInstance.goodbye());


var linkModule = " " +
"var mac = 'abc'; " +
"function Link() { " +
"	this.uuid = '1234';" +
"};" +
"var mLink = new Link();" +
"mLink.start = function(){" +
"    return this.uuid + ' start';" +
"};" +
"module.exports.Link = mLink;" +
"module.exports.mac = mac;";

Modules.addCached("linkModule", linkModule);
linkModule = undefined;

link = require('linkModule').Link;
console.log(link.start());
console.log(require('linkModule').mac);

// 验证module所分符号表使用无异常
/*
for(i=0; i<100; i++){
	console.log(link.start());
	console.log(require('linkModule').mac);
	console.log('te=' + require("e").foo());
	console.log('td=' + require("d").foo);
}
*/




// Number 测试，支持整数、浮点数、指数、八进制数、十六进制数、二进制数
var a = 345;
var b = 34.5;
var c = 3.45e2;
var d = 0377;
var e = 0xFF;
var f = 0b111;
var result = a==345 && b*10==345 && c==345 && d==255 && e==255 && f==7;

console.log('Number 测试 ' + ((result == 1 ) ? '成功' : '失败'));


// undefined/null 测试
var testUndefined;        // variable declared but not defined, set to value of undefined
var testObj = {};  

var a = 0;
if ((""+testUndefined) != "undefined") a = 1; // test variable exists but value not defined, displays undefined
if ((""+testObj.myProp) != "undefined") a = 2; // testObj exists, property does not, displays undefined
if (!(undefined == null)) a = 3;  // unenforced type during check, displays true
if (undefined === null) a = 4;// enforce type during check, displays false
if (null != undefined) a = 5;  // unenforced type during check, displays true
if (null === undefined) a = 6; // enforce type during check, displays false
if (undefined != undefined) a = 7;
if (!(undefined == undefined)) a = 8;
var b = undefined;
if (b !=undefined ) a = 9;	//  check for undefined
result = a==0;
console.log(testUndefined);
console.log('undefined/null 测试 ' + ((result == 1 ) ? '成功' : '失败'));

// Variable creation and scope from http://en.wikipedia.org/wiki/JavaScript_syntax
x = 0; // A global variable
var y = 'Hello!'; // Another global variable
z = 0; // yet another global variable

function f(){
  var z = 'foxes'; // A local variable
  twenty = 20; // Global because keyword var is not used
  return x; // We can use x here because it is global
}
// The value of z is no longer available

// testing
blah = f();
result = blah==0 && z!='foxes' && twenty==20 && (z==undefined);

console.log('变量及作用域 测试 ' + ((result == 1 ) ? '成功' : '失败'));


// if .. else
var a = 42;
if (a != 42)
  result = 0;
else
  result = 1;
console.log('if else 测试 ' + ((result == 1 ) ? '成功' : '失败'));


function Person(name) {
  this.name = name;
  this.kill = function() { this.name += " is dead"; };
}

var a = new Person("Kenny");
a.kill();
result = a.name == "Kenny is dead";

console.log('简单对象 测试 ' + ((result == 1 ) ? '成功' : '失败'));

obj1 = new Object();
obj1.food = "cake";
obj1.desert = "pie";

// 仅支持支持string 及 无传参的object对象的clone
obj2 = obj1.clone();
obj2.food = "kittens";

result = obj1.food=="cake" && obj2.desert=="pie";
console.log('简单对象 测试 ' + ((result == 1 ) ? '成功' : '失败'));


var Foo = {
 value : function() { return this.x + this.y; }
};

var a = { prototype: Foo, x: 1, y: 2 };
var b = new Foo(); 
b.x = 2;
b.y = 3;

var result1 = a.value();
var result2 = b.value();
result = result1==3 && result2==5;

console.log('嵌套对象 测试 ' + ((result == 1 ) ? '成功' : '失败'));


r = Math.random();
console.log('Math.random 测试 ' + ((result = r != undefined ) ? '成功' : '失败'));


parsed = Integer.parseInt("42");
aChar = 'A';

result = parsed==42 && Integer.valueOf(aChar)==65 ;

console.log('Integer.parseInt/valueOf 测试 ' + ((result == 1 ) ? '成功' : '失败'));


// test for shift
var a = (2<<2);
var b = (16>>3);

/*
// >>> (无符号右移) 暂不支持
var c = (-1 >>> 16);
*/

result = a==8 && b==2;
console.log('移位操作 测试 ' + ((result == 1 ) ? '成功' : '失败'));
	

var a=5.0/10.0*100.0;
var b=5.0*110.0;
var c=50.0/10.0;

result = a==50 && b==550 && c==5;
console.log('浮点数除法 测试 ' + ((result == 1 ) ? '成功' : '失败'));
	
	
var a =2;
var b=5;
var c=6;
result =  ((!a*b+a*10+a*b*c) == 80);

console.log('乘法/非操作 测试 ' + ((result == 1 ) ? '成功' : '失败'));

// 只支持后缀自增运算符
var a = 5;
var b = 5;
result = ((a++)==5) && (a==6) && ((b--) == 5) && (b==4);

console.log('后缀自增运算符 ++/-- 测试 ' + ((result == 1 ) ? '成功' : '失败'));


/*
当前语法不支持
var a = 5;
var b = 5;
result = ((++a)==6) && (a==6) && ((--b) == 4) && (b==4);

console.log('前置型 ++/-- 测试 ' + ((result == 1 ) ? '成功' : '失败'));
*/



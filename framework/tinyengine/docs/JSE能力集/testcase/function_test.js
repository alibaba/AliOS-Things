
// simple function
var b = function(x)
{
	return x;
};

var a = b(10)+b(20);

function add(x,y)
{
	return x+y;
}
var result;
result = (a == 30) && (add(3,6)==9);
console.log('简单函数测试 ' + ((result == 1 ) ? '成功' : '失败'));



// simple function scoping test
var a = 7;
function add(x,y) { var a=x+y; return a; }
result = add(3,6)==9 && a==7;
console.log('简单函数+作用域 测试 ' + ((result == 1 ) ? '成功' : '失败'));


// functions in variables
var bob = {};
bob.add = function(x,y) { return x+y; };

result = bob.add(3,6)==9;

console.log('对象函数 测试 ' + ((result == 1 ) ? '成功' : '失败'));


// functions in variables using JSON-style initialisation
var bob = { add : function(x,y) { return x+y; } };

result = bob.add(3,6)==9;

console.log('JSON描述对象函数 测试 ' + ((result == 1 ) ? '成功' : '失败'));


// double function calls
function a(x) { return x+2; }
function b(x) { return a(x)+1; }
result = a(3)==5 && b(3)==6;

console.log('嵌套调用函数 测试 ' + ((result == 1 ) ? '成功' : '失败'));

// recursion
function a(x) {
  if (x>1)
    return x*a(x-1);
  return 1;
}
result = a(5)==1*2*3*4*5;
console.log('嵌套调用函数 测试 ' + ((result == 1 ) ? '成功' : '失败'));



// references with functions
// 函数参数变量可以存在两种类型的值，即原始值和引用值。
// ECMAScript 有 5 种原始类型（primitive type），即 Undefined、Null、Boolean、Number 和 String

var a = 42;
var b = [];
b[0] = 43;

function foo(myarray) {
  myarray[0]++;
}

function bar(myvalue) {
  myvalue++;
}

foo(b);
bar(a);

result = a==42 && b[0]==44;
console.log('函数参数 原始值和引用值 测试 ' + ((result == 1 ) ? '成功' : '失败'));



// 全局变量及作用域

z = 0;
function addstuff() {
 var count=0;
 z = function() {
  count++;
  return count;
 };
}

addstuff();

result = z();

//console.log(count);  // BoneEngine > undefined

console.log(result);

console.log('全局变量及作用域 测试 ' + ((result == 1 ) ? '成功' : '失败'));


var func = addstuff;

console.log(func);   // BoneEngine > function

if( func )
  console.log('if判断函数 sucess');
else
  console.log('if判断函数 fail');



// references for arrays

//变量有两种类型的值，即原始值和引用值。
// ECMAScript 有 5 种原始类型（primitive type），即 Undefined、Null、Boolean、Number 和 String
// array 是引用值变量

var a = [];
a[0] = 10;
a[1] = 22;

b = a;
b[0] = 5;

result = a[0]==5 && a[1]==22 && b[1]==22;

console.log('array引用传递 测试 ' + (result ? '成功' : '失败'));


// Array length test

myArray = [ 1, 2, 3, 4, 5 ];
myArray2 = [ 1, 2, 3, 4, 5 ];
myArray2[8] = 42;

result = myArray.length == 5 && myArray2.length == 9;

console.log('array length 测试 ' + (result ? '成功' : '失败'));



// test for array contains
var a = [1,2,4,5,7];
var b = ["bread","cheese","sandwich"];

result = a.contains(1) && !a.contains(42) && b.contains("cheese") && !b.contains("eggs");

console.log('array contains 测试 ' + (result ? '成功' : '失败'));

// Array.indexOf

var a = [8,9,10];
a["foo"]="lala";
a[3.333] = 3;

var r = [
  a.indexOf(8),
  a.indexOf(10),
  a.indexOf(42)
];

result = r[0]==0 && r[1]==2 && r[2]==undefined;

console.log('array indexOf 测试 ' + (result ? '成功' : '失败'));


var a = [1,2];

a.push(3);
a.push(4);
a.push('ok5');
a.push(6);

a.pop();

result = (a.length == 5) && (a.indexOf(2) == 1) && a.contains('ok5') && (!a.contains(6)) ;

console.log('array push/pop 测试 ' + (result ? '成功' : '失败'));


// 支持数组 in 操作
var x;
for (x in a)
{
    console.log('a[' + x + ']=' + a[x]);
}

if( 4 in a )
    console.log('4 in a , sucess');


/*
// test for array remove
var a = [1,2,4,5,7];

// 不支持object.remove
a.remove(2);
a.remove(5);

result = a.length==3 && a[0]==1 && a[1]==4 && a[2]==7;


// test for array join
var a = [1,2,4,5,7];

// join 不支持
result = a.join(",")=="1,2,4,5,7";
*/




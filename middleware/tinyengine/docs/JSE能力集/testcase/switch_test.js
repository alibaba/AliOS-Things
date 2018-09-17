// switch-case-tests

////////////////////////////////////////////////////
// switch-test 1: case with break;
////////////////////////////////////////////////////
var a1=5;
var b1=6;
var r1=0;

switch(a1+5){
  case 6:
    r1 = 2;
    break;
  case b1+4:
    r1 = 42;
    break;
  case 7:
    r1 = 2;
    break;
}

console.log('switch case break测试 ' + ((result = r1 == 42 ) ? '成功' : '失败'));


////////////////////////////////////////////////////
// switch-test 2: case with out break;
////////////////////////////////////////////////////
var a2=5;
var b2=6;
var r2=0;

switch(a2+4){
  case 6:
    r2 = 2;
    break;
  case b2+3:
    r2 = 40;
    //break;
  case 7:
    r2 += 2;
    break;
}

console.log('switch 缺少break 测试 ' + ((result = r2 == 42 ) ? '成功' : '失败'));



////////////////////////////////////////////////////
// switch-test 3: case with default;
////////////////////////////////////////////////////
var a3=5;
var b3=6;
var r3=0;

switch(a3+44){
  case 6:
    r3 = 2;
    break;
  case b3+3:
    r3 = 1;
    break;
  default:
    r3 = 42;
}

console.log('switch default 测试 ' + ((result = r3 == 42 ) ? '成功' : '失败'));


/*
// switch-test 4: case default before case;  语法分析出错
default必须在最后一个case之后且default不能使用break结束字
var a4=5;
var b4=46;
var r4=40;

switch(a4+44){
  default:
    r4 = 42;
  case 6:
    r4 = 2;
    break;
  case b4+3:
    r4 = 1;
    break;
}
console.log('switch default before case 测试 ' + ((result = r4 == 42 ) ? '成功' : '失败'));
*/


result = r1 == 42 && r2 == 42 && r3 == 42;

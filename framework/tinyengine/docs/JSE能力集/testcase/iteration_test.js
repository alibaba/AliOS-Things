
// simple for loop
var a = 0;
var i;
for (i=1;i<10;i++) 
	a = a + i;
result = a==45;

console.log('for循环测试 ' + ((result == 1 ) ? '成功' : '失败'));


// simple for loop containing initialisation, using +=
var a = 0;
for (var i=1;i<10;i++) 
	a += i;
result = a==45;

console.log('for循环 += 测试 ' + ((result == 1 ) ? '成功' : '失败'));


// test break

var c1=0;
var c2=0;
var c3=0;

for (i=0;i<10;i++) {
  if (i>4) break;
  c1++;
}

for (i=0;i<10;i++) {
  c2++;
  if (i>4) break;
}

for (j=0;j<10;j++) { 
  for (i=0;i<10;i++) {
    if (i>4) break;
    c3++;
  }
  c3++;
}

result = (c1==5) && (c2==6) && (c3==10+5*10);

console.log('break测试 ' + ((result == 1 ) ? '成功' : '失败'));


// test continue

var c1=0;
var c3=0;

for (i=0;i<10;i++) {
  if (i>4 && i<8) continue;
  c1++;
}

for (j=0;j<10;j++) { 
  for (i=0;i<10;i++) {
    if (i>4 && i<8) continue;
    c3++;
  }
  c3++;
}

result = (c1==7) && (c3==10+7*10);

console.log('continue测试 ' + ((result == 1 ) ? '成功' : '失败'));



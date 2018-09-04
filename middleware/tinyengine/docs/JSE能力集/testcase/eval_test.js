

/* Javascript eval */

// 对象转换需要加()
myfoo = eval("({ foo: 42 })");

result = eval("4*10+2")==42 && myfoo.foo==42;
console.log('eval 测试 ' + (result ? '成功' : '失败'));


/* Javascript eval */

mystructure = { a:39, b:3, addStuff : function(c,d) { return c+d; } };

mystring = JSON.stringify(mystructure); 

mynewstructure = eval("("+mystring+")");

result = mynewstructure.addStuff(mynewstructure.a, mynewstructure.b) == 42;

console.log('eval 测试 ' + (result ? '成功' : '失败'));


var array = JSON.parse('[1,2,3,true]');
console.log('JSON.parse 测试 ' + (array.length==4 ? '成功' : '失败'));
console.log(array);

var obj = JSON.parse('{"name":"alibaba","age":18}'); 
console.log('JSON.parse 测试 ' + (obj.age==18 ? '成功' : '失败'));
console.log(obj.name);

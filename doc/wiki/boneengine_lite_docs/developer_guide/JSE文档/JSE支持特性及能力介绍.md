
# JSE介绍

JSE是一个专门为IoT系统（模组）设计及优化的JavaScript引擎，ROM/RAM占用小。

JSE特点： 不生成语法树，边解析边执行，遇到函数时把函数内容保存在符号表中，待函数调用时再来解析该函数内容并执行


## JSE 定制

* **符号表容量定制**
当前支持两种方法：
    1. 固定大小
```js
#undef RESIZABLE_JSE_SYMBOL_TABLE
#define BE_JSE_SYMBOL_TABLE_SIZE 1000
```

    2. 可动态分配 (**推荐**)
初始大小`BE_JSE_SYMBOL_TABLE_BLOCK_SIZE`个符号表，不够时再分配`BE_JSE_SYMBOL_TABLE_BLOCK_SIZE`个
``` js
#define RESIZABLE_JSE_SYMBOL_TABLE		    1
#define BE_JSE_SYMBOL_TABLE_BLOCK_SIZE		512
#define BE_JSE_SYMBOL_TABLE_BLOCK_SHIFT		9
```
<div id="6edags" data-type="math" data-display="inline" data-align="left" data-src="https://cdn.yuque.com/__latex/93b253fdbe8ab30d3ac97438bddefaa6.svg" data-text="2%5E9%20%3D%20512" data-width="62" data-height="24"><img src="https://cdn.yuque.com/__latex/93b253fdbe8ab30d3ac97438bddefaa6.svg" width="62"/></div>


## 特殊限制

* **声明变量/常量时，表达式命名最长为63个字符**
限制地方: `#define MAX_TOKEN_LENGTH  64`
若token值超过63，词法分析会出错，会打印"Token name is too long"
> 但是String类型的值的长度并没有限制
`0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef567899` 该常量长度大于64, **不正确**
`name0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef567899` 该变量长度大于64, **不正确**
``` js
var count = 1;
var name = '0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef567899'
var name0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef567899 = 1
```

* **定义函数原型时，函数参数名称不要超过8byte**
限制地方： `#define BE_JSE_SYMBOL_DATA_STRING_LEN  8`
函数参数名称 `parama` , 长度为6Byte, 正常 
`parambbbb` 长度大于8, **不正确**
``` js
function add(parama, parambbbb)
{
    ...
}
```

## JS语法详细介绍

* [**请见《javascript支持范围.xlsx》**](JSE能力集/javascript支持范围.xlsx)
<span data-type="color" style="color:#F5222D"><strong>支持语法实例请参考testcase中实例文件</strong></span>
```bash
docs/JSE能力集/testcase
├── array_test.js
├── base_test.js
├── eval_test.js
├── function_test.js
├── iteration_test.js
├── module_test.js
├── string_test.js
└── switch_test.js
```

* **内置对象有限支持new操作，不支持Object的constructor属性**

```bash
下例不支持
var arr = new Array(2);
var str = new String(2);
仅仅支持
var arr = new Array();
或者 var arr = [];
var obj = new Object();
或者 var obj = {};

var Person = function(name) {
  this.name = name;
  this.canTalk = true;
};
不支持，Person不是BE_SYM_OBJECT对象，类型是BE_SYM_FUNCTION
Person.prototype.greet = function() {
  if (this.canTalk) {
    print('Hi, I am ' + this.name);
  }
};
var rock = new Person('rock');
rock.greet();

// Foo的类型是BE_SYM_OBJECT
var Foo = {
 value : function() { return this.x + this.y; }
};
// a的类型是BE_SYM_OBJECT
var a = { prototype: Foo, x: 1, y: 2 };
var b = new Foo(); 
b.x = 2;
b.y = 3;
var result1 = a.value();    // == 3
var result2 = b.value();    // == 5;
```

* **String不支持编码类型**
只支持普通字符串`BYTE`类型,为0为结束符

* **不支持类型转换函数**
```bash
9.1 ToPrimitive
9.2 ToBoolean
9.3 ToNumber
9.4 ToInteger
9.5 ToInt32
9.6 ToUint32
9.7 ToUint16
9.8 ToString
9.9 ToObject
```

* **部分运算符不支持**
``` bash
下例不支持
11.4.1 delete 运算符
11.4.2 void 运算符
11.4.3 typeof 运算符
11.8.6 instanceof 运算符
```

* **部份语法不支持**
``` bash
下例不支持
12.6.1 do-while 语句
12.12 标签语句
12.13 throw 语句
12.14 try 语句
```

* **支持require模块**
```bash
require(module_name)
特殊扩展，可以通过JavaScript语法注册模块内容
Modules.addCached(module_name, code)
```
* **不支持NaN
不支持Infinity
不支持Date 对象
不支持RegExp 对象
不支持Error 对象**


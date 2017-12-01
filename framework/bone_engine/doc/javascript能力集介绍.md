 
# 介绍 

**`BoneEngine@lite` 是一个为模组设计的JavaScript引擎，由于模组对于`ROM、RAM`的使用限制要求比较严格，所以`BoneEngine@lite`对于JS的标准有裁剪,对JavaScript语法支持范围有限。**

---

## 支持能力集

### 1. 请见《javascript支持范围.xlsx》

### 2. javascript测试集目录testcases

```
编译master分支，运行自测程序
./BoneEngine  testcases
```

### 3. 标准JS对象说明

* Object对象
    不支持constructor、prototype属性

* Integer对象
    - Integer.parseInt(value)

    测试代码：
    ```
    var a = Integer.parseInt('0b011');
    var b = Integer.parseInt('0B1000');
    var c = Integer.parseInt('10.33');
    var d = Integer.parseInt('34 45 66');
    var e = Integer.parseInt(' 60 ');
    var f = Integer.parseInt('40 years');
    var h = Integer.parseInt('10');
    var i = Integer.parseInt('010');
    var j = Integer.parseInt('10');
    var k = Integer.parseInt('0x10');
    print('a=' + a);
    print('b=' + b);
    print('c=' + c);
    print('d=' + d);
    print('e=' + e);
    print('f=' + f);
    print('h=' + h);
    print('i=' + i);
    print('j=' + j);
    print('k=' + k);
    ```
    运行结果:
    ```
    BoneEngine > a=3 
    BoneEngine > b=8 
    BoneEngine > c=10 
    BoneEngine > d=34 
    BoneEngine > e=60 
    BoneEngine > f=40 
    BoneEngine > h=10 
    BoneEngine > i=8 
    BoneEngine > j=10 
    BoneEngine > k=16
    ```
* Math对象
    - Math.random()

* Array对象
    - 不支持 new 操作
    - 支持 []初始化
    - length
    - push(newelement)
    - pop()
    - indexOf(element)
    - contains(element)
    
    测试代码：
    ```
    var arr = [1,2];
    arr.push(3);
    arr.push(4);
    arr.push('ok5');
    arr.push(6);
    print('arr.length = ' + arr.length);
    arr.pop();
    print('arr.length = ' + arr.length);
    print('arr.indeOf(2) = ' + arr.indexOf(2));
    print('arr.contains(2) = ' + arr.contains(2));
    print('arr.contains(9) = ' + arr.contains(9));
    ```
    运行结果：
    ```
    BoneEngine > arr.length = 6 
    BoneEngine > arr.length = 5 
    BoneEngine > arr.indeOf(2) = 1 
    BoneEngine > arr.contains(2) = 1 
    BoneEngine > arr.contains(9) = 0 
    ```

* String对象
    - 不支持 new 操作
    - length
    - charAt(index)
    - substring(start,[stop])
    - substr(start,[length])
    - clone()

    测试代码：
    ```
    var str="Hello world!"
    print('str.charAt(2)=' + str.charAt(2));
    print('str.substr(3,7)=' + str.substr(3,7));
    print('str.substring(3)=' + str.substring(3));
    print('str.length=' + str.length);
    str = str + 'ok';
    print('str.length=' + str.length);
    print('str=' + str);
    var str2 = str.clone();
    print('str2=' + str2);
    ```
    运行结果
    ```
    BoneEngine > str.charAt(2)=l 
    BoneEngine > str.substr(3,7)=lo worl 
    BoneEngine > str.substring(3)=lo world! 
    BoneEngine > str.length=12 
    BoneEngine > str.length=14 
    BoneEngine > str=Hello world!ok
    BoneEngine > str2=Hello world!ok 
    ```
* JSON对象
    - stringify(value)
    - parse(text)

    测试代码:
    ```
    var cmd = {};
    cmd.attrSet = ['BodyWeight', "DateTime"];
    cmd.BodyWeight = {value:57.9};
    cmd.DateTime = {value: '2017/10/20 14:00' };
    print('JSON.stringify(cmd)=' + JSON.stringify(cmd));
    print('JSON.stringify(cmd.attrSet)=' + JSON.stringify(cmd.attrSet));
    
    var jsontext = '{"firstname":"Jesper",' + '"surname":"Aaberg","phone":["555-0100","555-0120"]}';
    var contact = JSON.parse(jsontext);
    print('contact.firstname=' + contact.firstname);
    print('contact.surname=' + contact.surname);
    
    var arr = ["a", "b", "c"];
    var str = JSON.stringify(arr);
    print('str=' + str);
    var newArr = JSON.parse(str);
    print('newArr.pop()='+newArr.pop());
    ```
    运行结果:
    ```
    BoneEngine > JSON.stringify(cmd)={"attrSet":["BodyWeight","DateTime"],"BodyWeight":{"value":"57.900000"},"DateTime":{"value":"2017/10/20 14:00"}} 
    BoneEngine > JSON.stringify(cmd.attrSet)=["BodyWeight","DateTime"] 
    BoneEngine > contact.firstname=Jesper 
    BoneEngine > contact.surname=Aaberg 
    BoneEngine > str=["a","b","c"] 
    BoneEngine > newArr.pop()=c 

    ```

* 全局函数
    - eval(string)
    
    测试代码：
    ```
    var x = 10;
    var y = 20;
    var a = eval("x * y");
    var b = eval("2 + 2");
    var c = eval("x + 17");
    
    print('a='+a);
    print('b='+b);
    print('c='+c);
    ```
    运行结果:
    ```
    BoneEngine > a=200 
    BoneEngine > b=4 
    BoneEngine > c=27 
    ```

* 支持的语法

    - 常用语法
    ```
    12.5 if语句
    12.6.2 while 语句
    12.6.3 for语句
    12.6.4 for-in 语句
    12.7 continue 语句
    12.8 break 语句
    12.11 switch 语句
        default语句在switch块的最后，且不需要break结束
    ```

## 不支持范围及限制条件


*  声明变量时，表达式命名最长为63个字符
> 但是`String`类型的值的长度并没有限制

```
var name="123456789012345678901234567890123456789012345678901234567890123";
在js语法中, name最大可以等于63个字符的常量
若name值超过63，运行出会出现"Token name is too long"

var str = "1234567890";
int i;
for(i=0; i<31; i++)
	str = str + "1234567890"

print(str.length);
print(str);

//str 计算正常
//320
//12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
```

* 内置对象不支持`new`操作
不支持Object的`prototype`属性
不支持Object的`constructor`属性
```
下例不支持
var arr = new Array(2);
var str = new String(2);

```

* String不支持`UTF-8`
只支持普通字符串`char*`,为0为结束符

* 不支持类型转换函数

```
下例不支持
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

* 部分运算符不支持

```
下例不支持
11.4.1 delete 运算符
11.4.2 void 运算符
11.4.3 typeof 运算符
11.8.6 instanceof 运算符
11.8.7 in 运算符
11.9.4 严格等于运算符 ( === )
11.9.5 严格不等于运算符 ( !== )
```

* 部份语法不支持

```
下例不支持
12.6.1 do-while 语句
12.10 with 语句
12.12 标签语句
12.13 throw 语句
12.14 try 语句
```

* 不支持`NaN`

* 不支持`Infinity`

* 不支持`Date 对象`

* 不支持`RegExp 对象`

* 不支持`Error 对象`



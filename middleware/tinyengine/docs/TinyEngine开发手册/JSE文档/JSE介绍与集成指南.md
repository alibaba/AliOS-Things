# <a name="5at2er"></a>JSE介绍

__名词解释: __

* <strong> </strong><strong><code>JSE: JavaScript Engine</code></strong><strong>的缩写简称 </strong>

### <a name="17ncsk"></a>JSE 工作原理
     JSE运行在主任务(`main task`)中，以单任务方式__顺序解析执行__，事件通知或回调必须在主任务中调用，即`C call JavaScript`不能多任务中并行调用，只能在主任务中串行调用。

* __顺序解析执行__
    以下面sample为例，JSE在执行到`var c = add(1,2)`时会出错，因为`add`这个函数在JSE的符号表中找不到(在addon中也找不到该函数，非本地扩展函数）。
```js
var c = add(1,2);
function add(a,b)
{
    return a+b;
}
```

正确写法如下
JSE在执行到`function add(a,b)`时把该函数的内容记录在符号表中，执行至`var c = add(1,2)`时会从符号表中找`add`函数，找到之后再解析执行`add`函数，并把返回值赋给`c`。

```js
function add(a,b)
{
    return a+b;
}
var c = add(1,2);
```

* <strong>符号表(</strong><strong><code>be_jse_symbol_t</code></strong><strong>)</strong>
<span data-type="color" style="color:rgb(38, 38, 38)"><span data-type="background" style="background-color:rgb(255, 255, 255)">JSE解析JavaScript时把所有JS对象都会记录保持在符号表中，每个符号表的两中重要标识，一个是</span></span>`refs`<span data-type="color" style="color:rgb(38, 38, 38)"><span data-type="background" style="background-color:rgb(255, 255, 255)">引用计数，与作用域有关，还有一个是</span></span>`locks`<span data-type="color" style="color:rgb(38, 38, 38)"><span data-type="background" style="background-color:rgb(255, 255, 255)">锁定计数，一般与函数使用有关，在函数中使用时+1，不再使用时-1，只有当引用计数和锁定计数都为</span></span>`0`<span data-type="color" style="color:rgb(38, 38, 38)"><span data-type="background" style="background-color:rgb(255, 255, 255)">时，回收该对象，释放该符号表。符号表的基本类型主要有： </span></span>`NULL`<span data-type="color" style="color:rgb(38, 38, 38)"><span data-type="background" style="background-color:rgb(255, 255, 255)">，</span></span>`字符串`<span data-type="color" style="color:rgb(38, 38, 38)"><span data-type="background" style="background-color:rgb(255, 255, 255)">，</span></span>`数组`<span data-type="color" style="color:rgb(38, 38, 38)"><span data-type="background" style="background-color:rgb(255, 255, 255)">，</span></span>`对象`<span data-type="color" style="color:rgb(38, 38, 38)"><span data-type="background" style="background-color:rgb(255, 255, 255)">，</span></span>`函数`<span data-type="color" style="color:rgb(38, 38, 38)"><span data-type="background" style="background-color:rgb(255, 255, 255)">,</span></span>`整数`<span data-type="color" style="color:rgb(38, 38, 38)"><span data-type="background" style="background-color:rgb(255, 255, 255)">，</span></span>`浮点数`<span data-type="color" style="color:rgb(38, 38, 38)"><span data-type="background" style="background-color:rgb(255, 255, 255)">，</span></span>`对象名称`<span data-type="color" style="color:rgb(38, 38, 38)"><span data-type="background" style="background-color:rgb(255, 255, 255)">，</span></span>`本地扩展对象`<span data-type="color" style="color:rgb(38, 38, 38)"><span data-type="background" style="background-color:rgb(255, 255, 255)">举例</span></span>`var a=5`<span data-type="color" style="color:rgb(38, 38, 38)"><span data-type="background" style="background-color:rgb(255, 255, 255)">说明， 在符号表数组中，</span></span>`a`<span data-type="color" style="color:rgb(38, 38, 38)"><span data-type="background" style="background-color:rgb(255, 255, 255)">是对象名称，</span></span>`5`<span data-type="color" style="color:rgb(38, 38, 38)"><span data-type="background" style="background-color:rgb(255, 255, 255)">一个整数类型的值。</span></span>

## <a name="3emecx"></a>JSE 定制

* __符号表容量定制__
    当前支持两种方法：

1. __固定大小__
```js
#undef RESIZABLE_JSE_SYMBOL_TABLE
#define BE_JSE_SYMBOL_TABLE_SIZE 1000
```

2. __可动态分配 (推荐)__
初始大小`BE_JSE_SYMBOL_TABLE_BLOCK_SIZE`个符号表，不够时再分配`BE_JSE_SYMBOL_TABLE_BLOCK_SIZE`个
```
#define RESIZABLE_JSE_SYMBOL_TABLE		    1
#define BE_JSE_SYMBOL_TABLE_BLOCK_SIZE		512
#define BE_JSE_SYMBOL_TABLE_BLOCK_SHIFT		9
```

## <a name="c5c7ip"></a>特殊限制

* __声明变量/常量时，表达式命名最长为63个字符__
    限制地方: `#define MAX_TOKEN_LENGTH 64`
    若token值超过63，词法分析会出错，会打印"Token name is too long"
> 但是String类型的值的长度并没有限制
> `0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef567899` 该常量长度大于64, __不正确__
> `name0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef567899` 该变量长度大于64, __不正确__
```
var count = 1;
var name = '0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef567899'
var name0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef567899 = 1
```

* __定义函数原型时，函数参数名称不要超过8byte__
    限制地方： `#define BE_JSE_SYMBOL_DATA_STRING_LEN 8`
    函数参数名称 `parama` , 长度为6Byte, 正常
    `parambbbb` 长度大于8, __不正确__

```
function add(parama, parambbbb)
{
    ...
}
```

## <a name="ng1qgl"></a>API 介绍

在`gravity_main.c`中使用， 头文件： be\_jse\_api.h

### <a name="33clzg"></a>void bone\_engine\_init();
BoneEngine初始化

### <a name="0n0ozg"></a>void bone\_engine\_native\_function\_register(BE\_JSE\_FUNCTION\_EXECUTE\_CB extension);
注册自定义JS对象Native扩展方法及属性的实现函数
__必须在bone\_engine\_init之后调用__
建议：
使用该函数扩展全局函数

### <a name="uryewq"></a>void be\_jse\_module\_load(const char\* moduleName, BE\_JSE\_FUNCTION\_EXECUTE\_CB extension);
用户注册自定义JS对象__moduleName__, 注册该对象的扩展方法及属性的实现函数__extension__
__必须在bone\_engine\_init之后调用__
建议：
使用该函数扩展自定义对象

### <a name="kiiywy"></a>void bone\_engine\_start(const char\* js);
运行JS程序(完整语法块)，可以多次调用

### <a name="y7q3kh"></a>void bone\_engine\_restart(const char\* js);
重新运行JS程序, JSE的符号表会重新初始化

### <a name="xk4uli"></a>void bone\_engine\_exit();
退出BoneEngine

### <a name="fn4dzf"></a>be\_jse\_executor\_ctx\_t\*  bone\_engine\_get\_executor();
获取当前BoneEngine引擎的executor对象

### <a name="4cdihg"></a>void bone\_engine\_loadmodule\_register(BE\_JSE\_LOAD\_MODULE\_CB func\_cb);
注册load module函数，用于js `requre`操作，根据模块名返回模块内容



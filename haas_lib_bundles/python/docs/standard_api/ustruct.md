# ustruct - 打包和解压原始数据类型

* 模块功能:  
ustruct提供打包和解压原始数据类型的功能。

默认情况下，C类型以机器的本机格式和字节顺序表示，并在必要时通过跳过填充字节来正确对齐（根据C编译器使用的规则）。参考下表，格式字符串的第一个字符用于指示打包数据的字节顺序，大小和对齐方式：

| 字符 | 自己额顺序             | 大小     | 对其方式 |
| --------- | ---------------------- | -------- | --------- |
| `@`       | native                 | native   | native    |
| `=`       | native                 | standard | none      |
| `<`       | little-endian          | standard | none      |
| `>`       | big-endian             | standard | none      |
| `!`       | network (= big-endian) | standard | none      |

**格式化字符表**

| 格式  | C语言的类型            | Python对应的类型    | 类型所占空间大小 |
| ---- | -------------------- | ----------------- | ------------- |
| `x`    | pad byte             | no value          |               |
| `c`    | `char`               | bytes of length 1 | 1             |
| `b`    | `signed char`        | integer           | 1             |
| `B`    | `unsigned char`      | integer           | 1             |
| `?`    | `_Bool`              | bool              | 1             |
| `h`    | `short`              | integer           | 2             |
| `H`    | `unsigned short`     | integer           | 2             |
| `i`    | `int`                | integer           | 4             |
| `I`    | `unsigned int`       | integer           | 4             |
| `l`    | `long`               | integer           | 4             |
| `L`    | `unsigned long`      | integer           | 4             |
| `q`    | `long long`          | integer           | 8             |
| `Q`    | `unsigned long long` | integer           | 8             |
| `n`    | `ssize_t`            | integer           |               |
| `N`    | `size_t`             | integer           |               |
| `f`    | `float`              | float             | 4             |
| `d`    | `double`             | float             | 8             |
| `s`    | `char[]`             | bytes             |               |
| `p`    | `char[]`             | bytes             |               |
| `P`    | `void *`             | integer           |               |


## calcsize - 返回存放 fmt 需要的字节数

* 函数原型：
> ustruct.calcsize(fmt)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|fmt|格式字符|是| 详情参考上文格化式字符表 |

* 返回值  
返回存放fmt格式的对象需要的字节数。

* 示例：

```python
>>> ustruct.calcsize('i')
4
>>> ustruct.calcsize('d')
8
```

## pack - 按照格式字符串fmt压缩参数

* 函数原型：
> ustruct.pack(fmt, v1, v2, ...)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|fmt|string|是| 压缩参数时所采用的格式字串，详情见上文格化式字符表 |
|v1|-|是| 要压缩的对象v1 |
|v2|-|是| 要压缩的对象v2 |
|...|-|是| 要压缩的其它对象 |

* 返回值  
参数对象经过编码后的字节对象。

* 示例：
```python
>>> ustruct.pack('iiiii', 1, 2, 3, 4, 5) # 打包5个整数
b'\x01\x00\x00\x00\x02\x00\x00\x00\x03\x00\x00\x00\x04\x00\x00\x00\x05\x00\x
00\x00'
```
## unpack - 根据格式化字符串fmt对数据进行解压

* 函数原型：
> unstrcut.unpack(fmt, data)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|fmt|string|是| 解压缩数据时所采用的格式字串，详情见上文格化式字符表 |
|data|bytearray|是| 要解压缩的对象v1 |

* 返回值  
数据解压后的存放的元组。

* 示例：

```python
>>> ustruct.unpack('iiiii', b'\x01\x00\x00\x00\x02\x00\x00\x00\x03\x00\x00\x
00\x04\x00\x00\x00\x05\x00\x00\x00')# 对压缩过的整数按照整数的格式进行解压缩
(1, 2, 3, 4, 5)
```

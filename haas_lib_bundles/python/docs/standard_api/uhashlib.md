
# uhashlib - 哈希算法

* 模块功能:  
实现二进制数据散列算法，支持sha256，sha1，MD5。

## sha256 - 创建一个SHA256哈希对象

* 参数原型：
> ​	hash_obj = uhashlib.sha256([bytes])

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| bytes | bytes | 否 | 可在创建时传入bytes数据，也可通过update方法更新 |

* 返回值  
SHA256哈希对象。

## sha1 - 创建一个SHA1哈希对象

* 参数原型：
> ​	hash_obj = uhashlib.sha1(bytes)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| bytes | bytes | 否 | 可在创建时传入bytes数据，也可通过update方法更新 |

* 返回值  
SHA1哈希对象

## md5 - 创建一个MD5哈希对象

* 参数原型：
> ​	hash_obj = uhashlib.md5(bytes)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| bytes | bytes | 否 | 可在创建时传入bytes数据，也可通过update方法更新 |

* 返回值  
MD5哈希对象

## update - 将更多的数据加到散列队列中

* 参数原型：
> ​	hash_obj.update(bytes)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| bytes | bytes | 否 | 需要被加密的数据 |

## digest - 返回通过哈希传递的所有数据的散列
* 参数原型：
> ​	hash_obj.digest()

注意：调用此方法后，无法再将更多的数据送入散列。

* 返回值：  
返回加密后字节类型的数据

* 示例：

```python
>>> import uhashlib
import ubinascii

hash_obj  = uhashlib.sha256()  # 创建hash对象
hash_obj.update(b"HaaS Python hashlib test")
data = hash_obj.digest()
>>> print(data)
b'\xe3\x88\xdb\xee\x99\xb8\x02\xce\x1d!d\xb4\xcd\x92l\xd8\x8f\xf7\xbde\xd0=\xd4h\xc7;(\x98\xd7\x99\xc7\x8b'
>>> hex_msg = ubinascii.hexlify(data)
>>> print(hex_msg)
b'e388dbee99b802ce1d2164b4cd926cd88ff7bd65d03dd468c73b2898d799c78b'
>>> 
```


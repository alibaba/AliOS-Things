# uos - 基本系统服务

* 模块功能:  
uos模块包含文件系统相关的操作。

## remove - 删除文件
* 函数原型：
> uos.remove(path)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|path|string|是|要删除的文件名|

## chdir - 改变当前目录
* 函数原型：
> uos.chdir(path)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|path|string|是|要删除的文件名|

## getcwd - 获取当前路径

* 函数原型：
> uos.getcwd()

* 参数说明：  
无

## listdir - 列出指定目录文件

* 函数原型：
> uos.listdir(dir)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|dir|string|是|目标目录名|

* 示例：

```python
>>> uos.listdir('/')
['sht3x.py', 'data', 'main.py', 'mq2.py', 'aht21b', 'aht21b.py', 'dht11.py']
```

## mkdir - 创建新目录

* 函数原型：
> uos.mkdir(path)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|path|string|是|准备创建的目录名|

* 示例：

```
>>> uos.mkdir('test')
>>> uos.listdir()
['sht3x.py', 'data', 'main.py', 'mq2.py', 'aht21b', 'aht21b.py', 'dht11.py', 'test']
```

## rename - 重命名文件

* 函数原型：
> uos.rename(old_path, new_path)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|old_path|string|是|要修改文件/目录的当前名称|
|new_path|string|是|要修改文件/目录的目标名称|

* 示例：

```python
>>> uos.rename('test', 'test1')
>>> uos.listdir('/')
['sht3x.py', 'data', 'main.py', 'mq2.py', 'aht21b', 'aht21b.py', 'dht11.py', 'test1']
```

## rmdir - 删除指定目录

* 函数原型：
> uos.rmdir(path)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|path|string|是|要删除的目录名|

* 示例：

```python
>>> uos.rmdir('test1')
>>> uos.listdir('/')
['sht3x.py', 'data', 'main.py', 'mq2.py', 'aht21b', 'aht21b.py', 'dht11.py']
```

## ilistdir - 列出当前目录参数

* 函数原型：
> uos.ilistdir(dir)

* 返回值：  
该函数返回一个迭代器(iterator)，该迭代器会生成所列出条目对应的3元组，此3元组的形式为 `(name, type, inode[, size])`，请参考下属说明：

* name 是条目的名称，字符串类型
* type 是条目的类型，整型数，0x4000表示目录，0x8000表示常规文件，可能为0
* inode 是一个与文件的索引节点相对应的整数，对于没有这种概念的文件系统来说，可能为0；

```python
>>> x = uos.ilistdir('/')
>>> next(x)
('sht3x.py', 0, 0)
>>> next(x)
('data', 0, 0)
>>> next(x)
('main.py', 0, 0)
>>> next(x)
('mq2.py', 0, 0)
>>> next(x)
('aht21b', 0, 0)
>>> next(x)
('aht21b.py', 0, 0)
>>> next(x)
('dht11.py', 0, 0)
```

## stat - 获取文件或目录的状态

* 函数原型：
> uos.stat(path)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|path|string|是|目标文件或目录名|

* 返回值：  
返回值是一个元组，返回值形式为：

`(mode, ino, dev, nlink, uid, gid, size, atime, mtime, ctime)`

* `mode` – inode保护模式
* `ino` – inode节点号
* `dev`  – inode驻留的设备
* `nlink`  – inode的链接数
* `uid ` – 所有者的用户ID
* `gid`  – 所有者的组ID
* `size`  – 文件大小，单位字节
* `atime`  – 上次访问的时间
* `mtime`  – 最后一次修改的时间
* `ctime`  – 操作系统的“ctime”

```python
>>> uos.stat('/')
(16384, 0, 0, 0, 0, 0, 0, 0, 0, 0)
```

## uname - 获取关于底层信息或其操作系统的信息

* 函数原型：
> uos.uname()

* 返回值：   
返回结果结果格式如下：
`(sysname, nodename, release, version, machine)`

* `sysname` – 底层系统的名称，string类型
* `nodename` – 网络名称(可以与 sysname 相同) ，string类型
* `release` – 底层系统的版本，string类型
* `version` – MicroPython版本和构建日期，string类型
* `machine` – 底层硬件(如主板、CPU)的标识符，string类型

* 示例：

```python
>>> uos.uname()
(sysname='esp32', nodename='V3.3', release='1.17.0', version='v1.17-dirty on 2021-11-04', machine='ESP32 module with ESP32')

```

## urandom - 返回具有*n个*随机字节的bytes对象

* 函数原型：
> uos.urandom(n)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|n|int|是|目标随机数的字节数|

* 返回值：  
返回具有*n个*随机字节的bytes对象，只要有可能，它就应该由硬件随机数生成器生成。

* 示例：

```python
>>> uos.urandom(5)
b'-\xf4QX\xcf'
```



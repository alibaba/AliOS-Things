# uzlib - zlib解压缩

* 模块功能:   
uzlib 模块解压缩用[DEFLATE算法](https://en.wikipedia.org/wiki/DEFLATE)压缩的二进制数据 （通常在zlib库和gzip存档器中使用），压缩功能尚未实现。

注意：解压缩前，应检查模块内可使用的空间，确保有足够空间解压文件。

## decompress - 返回解压后的bytes对象

* 函数原型：
> uzlib.decompress(data, wbits=0, bufsize=0)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| data | float | 是 | 要解压的内容 |
| wbits | int | 否 | 解压时使用的DEFLATE字典窗口大小（8-15，字典大小是`wbits`值的2的幂） |
| bufsize | int | 否 | 忽略即可 |

* 返回值：  
返回解压后的bytes对象。

## DecompIO - 创建一个`stream`装饰器

* 函数原型：
> class uzlib.DecompIO(stream, wbits=0)

* 函数功能：  
创建一个`stream`装饰器，该装饰器允许在另一个stream中透明地压缩数据。这允许处理数据大于可用堆大小的压缩流。wbits的值除了decompress中定义的值以外，还可以取值24..31（16 + 8..15），这表示输入流带有有gzip标头。


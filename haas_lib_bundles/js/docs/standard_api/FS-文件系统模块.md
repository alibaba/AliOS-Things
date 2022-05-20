# FS-文件系统模块

* 类功能：  
操作系统中负责管理和存储文件信息的软件机构称为文件管理系统，简称文件系统（FS）。JS轻应用的FS模块提供了对文件系统的基础操作接口，例如文件读，写，删除以及文件系统大小信息获取等能力。

## writeSync - 写文件
* 函数功能：  
写文件操作。

* 函数原型：
> fs.writeSync(path, content, options)

* 参数说明：  
String类型，具体描述如下，

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| path | String | 是 | 写入文件的路径和名称 |
| data | String | Array | 是 | 要写入的文本或者二进制数据 |
| options | Object | 否 | 参考下面的options说明 |

**options描述**
入参为 Object 类型，属性如下：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| flag | String | 否 | 具体如下，默认是'w' |

**文件系统flag说明**
当 **flag** 选项采用字符串时，则以下标志均可用：

> **'a'**: 打开文件用于追加。 如果文件不存在，则创建该文件。<br>
> **'a+'**: 打开文件用于读取和追加。 如果文件不存在，则创建该文件。<br>
> **'r'**: 打开文件用于读取。 如果文件不存在，则会发生异常。<br>
> **'r+'**: 打开文件用于读取和写入。 如果文件不存在，则会发生异常。<br>
> **'w'**: 打开文件用于写入。 如果文件不存在则创建文件，如果文件存在则截断文件。<br>
> **'w+'**: 打开文件用于读取和写入。 如果文件不存在则创建文件，如果文件存在则截断文件。<br>

* 返回值：整型，0表示成功，其他值表示失败

## readSync - 读取文件内容

* 函数功能：  
读取path路径下文件内容。

* 函数原型：
> var data = fs.readSync(path)

* 参数说明：
String类型，文件路径和名字

* 返回值：  
String ｜ Array类型，读取到的文件内容

## unlinkSync - 删除文件

* 函数功能：  
删除path路径下的文件。

* 函数原型：
> fs.unlinkSync(path)

* 参数说明：
String类型，文件路径和名字

* 返回值：  
无

## totalSize - 查看文件系统大小

* 函数功能：  
查看文件系统总大小，单位字节。

* 函数原型：
>var size = fs.totalSize()

* 参数说明：
无

* 返回值：  
Number类型，文件系统总大小数值

## usedSize - 查看文件系统已使用大小

* 函数功能：  
查看文件系统已使用大小，单位字节。

* 函数原型：
>var size = fs.usedSize()

* 参数说明：
无

* 返回值：  
Number类型，文件系统已使用大小数值

## freeSize - 查看文件系统剩余大小

* 函数功能：  
查看文件系统剩余大小，单位字节。

* 函数原型：
>var size = fs.freeSize()

* 参数说明：
无

* 返回值：  
Number类型，查看文件系统剩余大小，单位字节。

## 使用实例
* fs模块使用示例<br>
-- [haaseduk1 fs模块使用示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/haaseduk1/fs)<br>
-- [haas600/510/531 fs模块使用示例](https://gitee.com/alios-things/amp-examples/tree/master/common/fs)<br>
-- [ESP32 fs模块使用示例](https://gitee.com/alios-things/amp/tree/rel_3.3.0/example-js/ESP32/jsapi/fs)<br>

## 硬件平台

|硬件平台|是否支持？|说明|
|-----|:---:|----|
|HaaS100|是||
|HaaS200|是||
|HaaS EDU K1|是||
|HaaS600-EC100Y|是||
|HaaS600-EC600S|是||
|HaaS510|是||
|HaaS531|是||
|ESP32|是||

<br>


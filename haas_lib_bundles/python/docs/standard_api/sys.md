# sys - 系统相关功能

* 模块功能:  
sys 模块中提供了与HaaS Python运行环境有关的函数和变量。

* 常量说明

|常量定义|常量说明|
|-----|----|
| sys.argv|当前程序启动的可变参数列表 |
| sys.byteorder | 字节顺序 (‘little’  - 小端， ‘big’ - 大端) |
| sys.implementation | 当前microPython版本信息 <br>- name - 字符串“ micropython” <br> - version - 元组（主要，次要，微型），例如（1、7、0） |
| sys.maxsize | 本机整数类型可以在当前平台上保留的最大值|
| sys.modules | 已载入模块的字典 |
| sys.platform | 当前MicroPython运行的平台 |
| sys.stdin | 标准输入 |
| sys.stdout | 标准输出 |
| sys.version | MicroPython 语言版本，字符串格式。 |
| sys.version_info | MicroPython 语言版本，整数元组格式 |

## exit - 退出当前程序

* 函数原型：
> sys.exit(retval=0)

* 函数功能：  
使用给定的参数退出当前程序。与此同时，该函数会引发`SystemExit`退出。如果给定了参数，则将其值作为参数赋值给`SystemExit`。

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|retval|int|否|程序退出的返回值|


## print_exception - 打印异常信息

* 函数原型：
> sys.print_exception(exc, file=sys.stdout)

* 函数功能：
打印异常到文件对象，默认是打印到sys.stdout（输出异常信息的标准输出）。

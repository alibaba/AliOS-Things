# ULOG - 日志模块
* 模块功能：  
ulog日志系统是HaaS轻应用为开发者提供的日志记录系统，为基于HaaS轻应用的开发者提供日志分级控制，日志本地存储等服务。

* 注意事项： 
ulog同时管理native层和python层的log。这意味着一些底层log有时也会被记录。

## 常量说明

| 常量      | 常量数值 | 常量说明                                     |
| --------- | -------- | -------------------------------------------- |
| LOG_DEBUG | 7        | 调试级别 debug + info + warn + error + fatal |
| LOG_INFO  | 6        | 信息级别 info + warn + error + fatal         |
| LOG_WARN  | 4        | 警告级别 warn + error + fatal                |
| LOG_ERROR | 2        | 错误级别 error + fatal                       |
| LOG_FATAL | 3        | 灾难级别 fatal                               |

## setStdLogLevel - 设置终端日志输出等级
* 函数功能： 
设置终端日志输出等级

* 函数原型：
> ulog.setStdLogLevel(level)

* 参数说明：

| 参数  | 类型 | 必选参数？ | 说明     |
| ----- | ---- | :--------: | -------- |
| level | int  |     是     | 日志标签 |

* 返回值：  
成功：0，失败：-1。

* 示例： 

```python
import ulog
ulog.setStdLogLevel(ulog.LOG_DEBUG)
```

* 输出：

```
0
```


## setFsLogLevel - 设置记录到文件系统的日志等级
* 函数功能：  
设置记录到文件系统的日志等级

* 函数原型：
> ulog.setFsLogLevel(level)

* 参数说明：

| 参数  | 类型 | 必选参数？ | 说明     |
| ----- | ---- | :--------: | -------- |
| level | int  |     是     | 日志标签 |

* 返回值：  
成功：0，失败：-1。

* 示例： 

```python
import ulog
ulog.setFsLogLevel(ulog.LOG_DEBUG)
```

* 输出：

```
0
```

## setLogFilePath - 设置日志文件的存储路径
* 函数功能：  
设置记录到文件系统的日志等级

* 函数原型：
> ulog.setFsLogLevel(path)

* 参数说明：

| 参数 | 类型   | 必选参数？ | 说明         |
| ---- | ------ | :--------: | ------------ |
| path | string |     是     | 日志文件路径 |

* 返回值：  
成功：0，失败：-1。

* 示例： 

```python
import ulog
ulog.setLogFilePath('/example.log')
```

* 输出：

```
0
```

## setLogFileSize - 设置日志文件的最大存储大小

* 函数功能：  
设置日志文件的最大存储大小

* 函数原型：
> ulog.setLogFileSize(size)

* 参数说明：

| 参数 | 类型 | 必选参数？ | 说明                            |
| ---- | ---- | :--------: | ------------------------------- |
| size | int  |     是     | 日志文件的最大存储大小,单位Byte |

* 返回值：  
成功：0，失败：-1。

* 示例： 

```python
import ulog
ulog.setLogFileSize(1024)
```

* 输出：

```
0
```

## debug - debug日志

* 函数功能：  
记录debug级别日志

* 函数原型：
> ulog.debug(tag, msg)

* 参数说明：

| 参数 | 类型   | 必选参数？ | 说明     |
| ---- | ------ | :--------: | -------- |
| tag  | string |     是     | 日志标签 |
| msg  | string |     是     | 日志内容 |

* 示例： 

```python
import ulog
ulog.debug('TEST','test log here')
```

* 输出：
```
[  47.433]<D>TEST test log here
```

## info - info日志

* 函数功能：  
记录info级别日志

* 函数原型：
> ulog.info(tag, msg)

* 参数说明：

| 参数 | 类型   | 必选参数？ | 说明     |
| ---- | ------ | :--------: | -------- |
| tag  | string |     是     | 日志标签 |
| msg  | string |     是     | 日志内容 |

* 示例： 

```python
import ulog
ulog.info('TEST','test log here')
```

* 输出：
```
[  47.433]<I>TEST test log here
```

## warn - warn日志

* 函数功能：  
记录warn级别日志

* 函数原型：
> ulog.warn(tag, msg)

* 参数说明：

| 参数 | 类型   | 必选参数？ | 说明     |
| ---- | ------ | :--------: | -------- |
| tag  | string |     是     | 日志标签 |
| msg  | string |     是     | 日志内容 |

* 示例： 

```python
import ulog
ulog.warn('TEST','test log here')
```

* 输出：

```
[  47.433]<W>TEST test log here
```

## error - debug日志

* 函数功能：  
记录debug级别日志

* 函数原型：
> ulog.error(tag, msg)

* 参数说明：

| 参数 | 类型   | 必选参数？ | 说明     |
| ---- | ------ | :--------: | -------- |
| tag  | string |     是     | 日志标签 |
| msg  | string |     是     | 日志内容 |

* 示例： 

```python
import ulog
ulog.error('TEST','test log here')
```

* 输出：
```
[  47.433]<E>TEST test log here
```

## fatal - fatal日志

* 函数功能：  
记录debug级别日志

* 函数原型：
> ulog.fatal(tag, msg)

* 参数说明：

| 参数 | 类型   | 必选参数？ | 说明     |
| ---- | ------ | :--------: | -------- |
| tag  | string |     是     | 日志标签 |
| msg  | string |     是     | 日志内容 |

* 示例： 

```python
import ulog
ulog.fatal('TEST','test log here')
```

* 输出：

```
[  47.433]<F>TEST test log here
```

## 使用实例

* 代码：

```python
import ulog
ulog.setStdLogLevel(ulog.LOG_DEBUG)
ulog.setLogFilePath('/test.log')
ulog.setLogFileSize(1024)
ulog.setFsLogLevel(ulog.LOG_DEBUG)

ulog.debug('TEST','test log here')
```

* 输出：

```python
[  52.579]<D>TEST test log here
```
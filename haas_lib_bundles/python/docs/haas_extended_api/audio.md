# audio - 音频系统模块

* 模块功能：  
audio音频系统实现HaaS平台音频相关的功能，包括2个子模块：播放和录制。

* 注意事项：  
部分开发板（m5stack-core2）音频模块外交功放，所以在使用前需要确保功放已经打开。
如果是播放网络音频资源，需要确保网络连接成功。
下面是m5stack-core2开发板使能功放的代码。

```python
from axp192 import *
axp = Axp192()
axp.powerAll()
axp.setSpkEnable(1)
```

## 常量说明
|常量|常亮数值|常量说明|
|:-|:-|:-|
|Player.STATUS_UNKNOWN|0|播放状态未定义|
|Player.STATUS_RUNNING|1|播放进行中|
|Player.STATUS_PAUSED|2|播放暂停|
|Player.STATUS_STOPPED|3|播放停止|
|Player.STATUS_FINISHED|4|播放结束|
|Player.STATUS_ERROR|5|播放错误|

## Player - 创建Player对象
* 函数原型：
> player = Player()

* 参数说明：  
无

* 返回值：  
Player对象成功，返回PlayerT对象；Player对象创建失败，返回None

* 示例代码： 

```python
from audio import Player
player = Player()
```

## open - 打开播放器

* 函数功能：  
打开播放器

* 函数原型：
> Player.open()

* 参数说明：  
 无

* 返回值：  
打开Player成功返回0；打开Player失败返回失败错误码

* 示例： 

```python
from audio import Player

player = Player()
ret = player.open()
print(ret)
player.close()
```

* 输出：

```
0
```

## close - 关闭播放器

* 函数功能：  
关闭播放器

* 注意事项：  
需确保要关闭的播放器处于open状态

* 函数原型：
> Player.close()

* 参数说明：  
无

* 返回值：  
关闭Player设备成功返回0；关闭Playe设备失败返回失败错误码

* 示例： 

```python
from audio import Player

player = Player()
player.open()
ret = player.close()
print(ret)
```

* 输出：

```
0
```

## play - 播放音频文件

* 函数功能：  
播放音频文件，可以播放本地文件或者网络文件

* 注意事项：  
需确保此Player处于open状态

* 函数原型：
> Player.play(uri, sync)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|uri|string|是|音频文件的uri信息，目前支持两种格式： "https://dl.espressif.com/dl/audio/mp3_music.mp3" 或者 "file://sdcard/test.mp3" |
|sync|bool|否|是否同步播放，默认为false|

* 返回值：  
播放成功则返回0，播放失败返回错误码。

* 示例： 

```python

from audio import Player

player = Player()
ret = player.open()
ret = player.play(uri='file://lib/audio/test.mp3', sync=True)
print(ret)
player.stop()
player.close()
```

* 输出：

```
0
```

## stop - 播放器停止

* 函数功能：
播放器停止

* 注意事项：
需确保证Player处于打开状态

* 函数原型：
> Player.stop()

* 参数说明：
无

* 返回值：
成功则返回0，失败返回错误码

* 示例： 

```python

from audio import Player
import utime

player = Player()
player.open()
player.play(uri='file://lib/audio/test.mp3', sync=False)
utime.sleep(3)
ret = player.stop()
player.close()
print(ret)

```

* 输出：

```
0
```

## pause - 播放器暂停

* 函数功能：  
播放器暂停

* 注意事项：  
需确保证Player处于正在播放状态

* 函数原型：
> Player.pause()

* 参数说明：  
无

* 返回值：  
成功则返回0，失败返回错误码

* 示例： 

```python

from audio import Player
import utime

player = Player()
player.open()
player.play(uri='file://lib/audio/test.mp3', sync=False)
utime.sleep(3)
ret = player.pause()
print(ret)
player.stop()
player.close()

```

* 输出：

```
0
```

## resume - 播放器恢复播放

* 函数功能：  
播放器恢复播放

* 注意事项：  
需确保证Player处于pause状态

* 函数原型：
> Player.resume()

* 参数说明：  
无

* 返回值：  
成功则返回0，失败返回错误码

* 示例： 

```python

from audio import Player
import utime

player = Player()
player.open()
player.play(uri='file://lib/audio/test.mp3', sync=False)
utime.sleep(1)
player.pause()
utime.sleep(1)
ret = player.resume()
print(ret)
utime.sleep(5)
player.stop()
player.close()

```

* 输出：

```
0
```

## setVolume - 播放器设置音量

* 函数功能：  
播放器设置音量

* 注意事项：  
无

* 函数原型：
> Player.setVolume(vol)

* 参数说明：  
  
|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|vol|int|是|音频音量，取值范围为[0,100], 0表示静音 |

* 返回值：  
成功则返回0，失败返回错误码

* 示例： 

```python

from audio import Player

player = Player()
player.open()
ret = player.setVolume(75)
print(ret)
player.close()

```

* 输出：

```
0
```

## getVolume - 播放器获取音量

* 函数功能：  
播放器获取音量

* 注意事项：  
无

* 函数原型：
> Player.getVolume()

* 参数说明：  
无

* 返回值：  
成功则返回当前音量，失败返回错误码

* 示例： 

```python

from audio import Player

player = Player()
player.open()
player.setVolume(75)
ret = player.getVolume()
print(ret)
player.close()

```

* 输出：

```
75
```

## getState - 播放器获取播放状态

* 函数功能：  
播放器获取播放状态

* 注意事项：  
无

* 函数原型：
> Player.getState()

* 参数说明：  
无

* 返回值：  
返回播放器当前播放状态

* 示例： 

```python

from audio import Player

player = Player()
player.open()
player.play(uri='file://lib/audio/test.mp3', sync=False)
ret = player.getState()
print(ret)
player.stop()
player.close()

```

* 输出：

```
1
```

## getPosition - 播放器获取播放位置

* 函数功能：  
播放器获取播放位置

* 注意事项：  
无

* 函数原型：
> Player.getPosition()

* 参数说明：  
无

* 返回值：  
返回播放器当前播放位置，单位是毫秒

* 示例： 

```python

from audio import Player

player = Player()
player.open()
player.play(uri='file://lib/audio/test.mp3', sync=False)
utime.sleep(3)
ret = player.getPosition()
print(ret)
player.stop()
player.close()

```

* 输出：

```
3581
```

## getDuration - 播放器获取音频文件时长

* 函数功能：  
播放器获取音频文件时长

* 注意事项：  
无

* 函数原型：
> Player.getDuration()

* 参数说明：  
无

* 返回值：  
返回当前音频文件时长，单位是毫秒

* 示例： 

```python

from audio import Player

player = Player()
player.open()
player.play(uri='file://lib/audio/test.mp3', sync=False)
ret = player.getDuration()
print(ret)
player.stop()
player.close()

```

* 输出（这里test.mp3时长为12秒）：
```
12000
```

## seek - 播放器播放进度控制

* 函数功能：  
播放器播放进度控制

* 注意事项：  
无

* 函数原型：
> Player.seek(seek_time_sec))

* 参数说明：  

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|seek_time_sec|int|是|播放控制进度，单位是秒 |

* 返回值：  
成功则返回0，失败返回错误码

* 示例： 

```python

from audio import Player

player = Player()
player.open()
player.play(uri='file://lib/audio/test.mp3', sync=False)
ret = player.seek(3)
print(ret)
player.stop()
player.close()

```

* 输出：

```
0
```

## on - 注册Player异步回调函数

* 函数功能：  
向Player注册播放状态异步回调函数

* 注意事项：  
funcCallback仅用来返回状态，请不要在回调函数做阻塞操作或者长期占用该函数。

* 函数原型：  
> Player.on(func)

* 参数说明：  

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|func|funcCallback(data)|是|数据接收回调函数指针|

funcCallback函数参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|data|dictionary|是|键为"state"，值为播放状态|

* 返回值：  
成功则返回0，失败返回其他数值

* 示例： 

```python

from audio import Player

def cb(data):
    print(data)
	
player = Player()
player.open()
ret = player.on(cb)
print(ret)
player.close()

```

* 输出：

```
0
```


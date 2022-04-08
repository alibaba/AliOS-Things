
# music - 音乐播放器
&emsp;&emsp;
基于音乐播放器模板可以很方便的对要播放的音乐进行管理。其代码实现请参考music.py的实现。该模板的默认运行效果如下：
<div align="center">
<img src=./../../../images/haas_ui_lite_music_player.png width=25%/>
</div>

&emsp;&emsp;
下面是改模板API及UI面板元素的说明。

## 音乐播放器API说明

### MusicPlayer() - 创建播放器对象
<br>

* 函数原型

> player = MusicPlayer()

* 参数说明
> 

* 返回值
> 创建成功返回播放器类型的对象

* 示例代码
```python
from music import MusicPlayer

player = MusicPlayer()
print('Music Player started')

```

* 代码输出
```log
Music Player started
```
</br>

### createPage() - 创建计时器

* 函数功能：

> 创建音乐播放器UI面板


* 函数原型

> MusicPlayer.createPage()

* 参数说明
无

* 返回值

> 无

* 示例代码
```python
from music import MusicPlayer

player = MusicPlayer()
print('Music Player started')

player.createPage()
print("create player UI")

```
* 代码输出
```log
Music Player started
create player UI
```

### addToList(musicItem) - 将musicItem添加到播放列表中

* 函数原型

> MusicPlayer.addToList(musicItem)

* 参数说明

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| musicItem | 字典类型 | 是 | 字典中各变量说明如下：<br> title：音乐名称<br> album：专辑名称<br> album_url：专辑封面<br> url：音乐文件所在位置<br> duration：音乐总时长，单位：秒<br> favorite：是否加入收藏列表|

* 返回值
> 无

> 模板文件中内置了一个播放列表，详情见music.py中musicData的内容。

* 示例代码
```python
# -*- encoding: utf-8 -*-
from music import MusicPlayer

player = MusicPlayer()
player.createPage()
print('Music Player started')

my_music = {
    "title":"spring_short",
    "album":"Darius",
    "album_url": '',
    "url":"file://data/pyamp/spring.mp3",
    "duration":30,
    "favorite": False
}

player.addToList(my_music)
print('add new music into play list')

```

* 代码输出
```log
Music Player started
add new music into play list
```
</br>

### removeFromList(musicItem) - 将musicItem从播放列表中移除
<br>

* 函数原型

> MusicPlayer.removeFromList(musicItem)

* 参数说明

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| musicItem | 字典类型 | 是 | 同**addToList**|

>在遍历musicData查找目标音乐信息的时候"favorite"字段的值会被忽略。

* 返回值
> 无

* 示例代码
```python
# -*- encoding: utf-8 -*-
from music import MusicPlayer

player = MusicPlayer()
player.createPage()
print('Music Player started')

my_music = {
    "title":"spring_short",
    "album":"Darius",
    "album_url": '',
    "url":"file://data/pyamp/spring.mp3",
    "duration":30,
    "favorite": False
}

player.addToList(my_music)
print('add new music into play list')

player.removeFromList(my_music)
print('remove music from play list')

```

* 代码输出
```log
Music Player started
add new music into play list
remove music from play list
```
</br>


## 音乐播放器模板元素
&emsp;&emsp;
如下图所示，此温湿度面板一共有8个元素，编号为1-4的元素是温度显示的组成部分，编号为5-8的元素为相对湿度显示的组成部分。
<div align="center">
<img src=./../../../images/haas_ui_lite_music_player_elements.png width=20%/>
</div>

&emsp;&emsp;
下面是音乐播放器模板代码中显示各元素的位置，读者可以根据自己的需求对其进行替换或修改。

|元素ID|功能|相关代码|说明|
|-----|----|:---|----|
|1|音乐/专辑封面|albumCover.set_src(musicData[currentMusic]["album_url"])|静态图片，尺寸：116*116|
|2|音乐名称|songTitle.set_text(musicData[currentMusic]["title"])|英文|
|3|专辑名称|albumTitle.set_text(musicData[currentMusic]["album"])|英文|
|4|当前位置|当前播放的音乐位置|时间，单位：秒|
|5|音乐时长|音乐信息的"duration"字段|需与音乐实际长度相符，单位：秒|
|6|上一首按钮| image[i].set_src(functionImage[i])|无|
|7|开始/暂停播放按钮| image[i].set_src(functionImage[i])|无|
|8|下一首按钮| image[i].set_src(functionImage[i])|无|
|9|添加/取消收藏|image[3].set_src(RESOURCES_ROOT + "images/favorite.png")<br>image[3].set_src(RESOURCES_ROOT + "images/favorited.png")|本功能仅仅是演示功能，重开机即可|

&emsp;&emsp;
如果读者想要基于此模板进行UI布局的调整，可能需要用到UI lite的更多组件，UI Lite组件的详细用法请参考[UI Lite控件合集文章](https://haas.iot.aliyun.com/haasapi/index.html#/Python/docs/zh-CN/haas_extended_api/ui_lite/UI_Lite_widgets)

&emsp;&emsp;
另外我们提供了温湿度面板、计时器等模版，只需要几行代码就能使用这些功能。基于这些模板的案例详情请参考如下趣味案例。
* [温湿度面板](https://haas.iot.aliyun.com/solution/detail/solution?versionId=800AEA703FFFF1A300000001&dataId=800AEA703FFFF1A3)
* [倒计时计时器](https://haas.iot.aliyun.com/solution/detail/solution?versionId=800A1DAF896F599700000001&dataId=800A1DAF896F5997)
* [音乐播放器](https://haas.iot.aliyun.com/solution/detail/solution?versionId=800AE9E2A8B7DCDB00000001&dataId=800AE9E2A8B7DCDB)

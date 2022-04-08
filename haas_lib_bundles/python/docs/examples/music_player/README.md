

# 音乐播放器
&emsp;&emsp;
音乐播放器是大家日常使用最多的工具之一，类似的音频播放场景还有在医院/银行经常见到的排队叫号系统、工业场景中常用的的操作提示装置等等。

## 简介
&emsp;&emsp;
本文将基于HaaS UI Lite开发框架使用Python语言进行音乐播放器的打造，实际运行效果图如下：
<div align="center">
<img src=./../../images/haas_ui_lite_music_player.png width=25%/>
</div>

## 准备
&emsp;&emsp;
本案例打造只需要使用[M5StackCore2开发板](../../startup/M5StackCore2_startup.md)一台即可。

## 设备端开发

### 开发环境
&emsp;&emsp;
在进行下一步之前请确保M5StackCore2开发环境已经搭建完毕，详情请参考[M5StackCore2快速开始](../../startup/M5StackCore2_startup.md)中的说明。

### 创建解决方案
&emsp;&emsp;
如下图所示，打开VS Code之后在新建一个基于helloworld的python工程，设定好工程名称（“music_player”）及工作区路径之后，**硬件类型**选择m5stackcore2，点击**立即创建**，创建一个Python轻应用的解决方案。

<div align="center">
<img src=./../../images/music_player_创建工程_m5stack.png width=40%/>
</div>

&emsp;&emsp;
将[本案例脚本](./code/)的代码全部复制后，覆盖“music_player”工程根目录下的所有文件，main.py文件如下图所示：
<div align="center">
<img src=./../../images/music_player_main界面.png width=80%/>
</div>

## 运行效果
&emsp;&emsp;
将music_player工程推送到M5StackCore2开发板之后，脚本会自动运行。程序运行之后，会默认加载播放器，如下图所示：


&emsp;&emsp;
播放音乐过程中点击“开始播放”，“暂停播放”，“下一首”，“上一首”等按钮后，日志如下。其中：
* python execute from /data/pyamp/main.py：代表Python脚本开始运行<br>
* start to play：开始播放音乐<br>
* music puaused：暂停播放<br>
* switch to next song: 切换到下首音乐<br>
* switch to previous song: 切换到上首音乐<br>

```log
 ==== python execute bootpy ====
 ==== python file check /data/pyamp/main.py ====
 ==== python execute from /data/pyamp/main.py ====
play Blob
start to play
...
----------------------------- ESP Audio Platform -----------------------------
|                                                                            |
|                 ESP_AUDIO-v1.7.0-31-g5b8f999-3072767-09be8fe               |
|                     Compile date: Oct 14 2021-11:00:34                     |
------------------------------------------------------------------------------
...
play Blob
music puaused
...
next Blob
switch to next song
...
prev Blob
switch to previous song
...
```

## HaaS UI Lite 音乐播放器模板说明
&emsp;&emsp;
本案例中使用到了HaaS UI Lite封装的**音乐播放器**类型的**模板**。此模板的代码实现请参考music.py的实现。基于此模板可以很方便的对要播放的音乐进行管理。下面是对模板元素及面板库API的说明。

### 音乐播放器元素
&emsp;&emsp;
如下图所示，此温湿度面板一共有8个元素，编号为1-4的元素是温度显示的组成部分，编号为5-8的元素为相对湿度显示的组成部分。
<div align="center">
<img src=./../../images/haas_ui_lite_music_player_elements.png width=40%/>
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

### 音乐播放器API说明
目前音乐播放器提供了添加/删除音乐到播放列表的功能作为示例。
> 模板文件中内置了一个播放列表，详情见music.py中musicData的内容。

#### addToList(musicItem) - 将musicItem添加到播放列表中
<br>

* 函数原型

> addToList(musicItem)

* 参数说明

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| musicItem | 字典类型 | 是 | 字典中各变量说明如下：<br> title：音乐名称<br> album：专辑名称<br> album_url：专辑封面<br> url：音乐文件所在位置<br> duration：音乐总时长，单位：秒<br> favorite：是否加入收藏列表|

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

```

* 代码输出
```log
Music Player started
add new music into play list
```
</br>

#### removeFromList(musicItem) - 将musicItem从播放列表中移除
<br>

* 函数原型

> removeFromList(musicItem)

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

&emsp;&emsp;
本案例中使用到了多种控件，关于HaaS UI Lite的控件说明请参考[链接](../../haas_extended_api/haas_ui_lite/HaaS_UI_Lite_widgets.md)。

## 思考
&emsp;&emsp;
本案例中音乐播放器模板提供了添加和删除音乐到播放列表中的功能，常见的音乐播放器还有很多其它功能，比如单曲循环、播放列表循环等等。有没有兴趣做一个客制化的播放器，添加自己想要的功能？有的话赶紧来挑战一下吧。也欢迎将您的作品投稿给我们，说不定会有小惊喜哦。

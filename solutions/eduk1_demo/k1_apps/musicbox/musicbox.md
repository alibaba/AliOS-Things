@page musicbox 复古八音盒
# 实验介绍
Chiptune是不少80，90后的童年回忆，说Chiptune的名字应该很多人比较陌生，不过它有另外一个名字：8-bit。所谓的所谓的Chiptune也就是由老式家用电脑、录像游戏机和街机的芯片（也就是所谓的CHIP）发出的声音而写作的曲子。严格说来其实Chiptune不仅仅只有8bit，不过都是追求复古颗粒感的低比特率。本实验中，我们也来实现一款复古“八音”盒。

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01B8CEvh1kaBmdtwajB_!!6000000004699-1-tps-1200-800.gif" style="zoom:50%;" />
</div>

# 涉及知识点
乐谱编码
PWM与蜂鸣器


# 开发环境准备
## 硬件
    开发用电脑一台
    HAAS EDU K1 开发板一块
    USB2TypeC 数据线一根

## 软件
### AliOS Things开发环境搭建
    开发环境的搭建请参考 @ref HaaS_EDU_K1_Quick_Start (搭建开发环境章节)，其中详细的介绍了AliOS Things 3.3的IDE集成开发环境的搭建流程。

### HaaS EDU K1 DEMO 代码下载
    HaaS EDU K1 DEMO 的代码下载请参考 @ref HaaS_EDU_K1_Quick_Start (创建工程章节)，其中，
    选择解决方案: 基于教育开发板的示例
    选择开发板: haaseduk1 board configure

### 代码编译、烧录
    参考 @ref HaaS_EDU_K1_Quick_Start (3.1 编译工程章节)，点击 ✅ 即可完成编译固件。
    参考 @ref HaaS_EDU_K1_Quick_Start (3.2 烧录镜像章节)，点击 "⚡️" 即可完成烧录固件。



# 蜂鸣器
蜂鸣器是一种非常简单的发声器件，和播放播放使用的扬声器不同，蜂鸣器只能播放较为简单的频率。
从驱动原理上区分，蜂鸣器可以分为无源蜂鸣器和有源蜂鸣器。这里的“源”，指的就是有无驱动源。无源蜂鸣器，顾名思义，就是没有自己的内置驱动源。只有为音圈接入交变电流后，其内部的电磁铁与永磁铁相吸或相斥而推动振膜发声，而接入直流电后，只能持续推动振膜而无法产生声音，只能在接通或断开时产生声音。而有源驱动器相反，只要接入直流电，其内部的驱动源会以一个固定的频率驱动振膜，直接发声。
在本实验中，推荐大家使用无源蜂鸣器，因为它只由PWM驱动，声音会更清脆纯净。使用有源蜂鸣器时，也能实现类似的效果，不过由于叠加了有源蜂鸣器自己的震动频率，声音会略显嘈杂。
## 驱动电路

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN015ylEJz1X8cwWhJHxh_!!6000000002879-2-tps-758-516.png" style="zoom:50%;" />
</div>

蜂鸣器的 1端 连接到VCC，2端 连接到三极管。这里的三极管由PWM0驱动，来决定蜂鸣器的 2端 是否和GND连通，进而引发一次振荡。通过不断翻转IO口，即可以驱动蜂鸣器发声。
## 驱动代码
为了实现IO口按特定频率翻转，我们可以使用PWM（脉冲宽度调制）功能。关于PWM的详细介绍可以参看z第三章资源PWM部分。
在本实验中，我们实现了tone和noTone两个方法。其中，tone方法用于驱动蜂鸣器发出特定频率的声音，也就是“音调”。noTone方法用于关闭蜂鸣器。
值得注意的是，在tone方法中，pwm的占空比固定设置为0.5，这代表在一个震动周期内，蜂鸣器的振膜总是一半时间在上，一半时间在下。在这里改变占空比并不会改变蜂鸣器的功率，所以音量大小不会改变。
```c
// solutions/eduk1_demo/k1_apps/musicbox/musicbox.c

void tone(uint16_t port, uint16_t frequency, uint16_t duration)
{
    pwm_dev_t pwm = {port, {0.5, frequency}, NULL};	// 设定pwm 频率为设定频率
    if (frequency > 0)								// 频率值合法才会初始化pwm
    {
        hal_pwm_init(&pwm);
        hal_pwm_start(&pwm);
    }
    if (duration != 0)					
    {
        aos_msleep(duration);
    }
    if (frequency > 0 && duration > 0)		// 如果设定了 duration，则在该延时后停止播放
    {
        hal_pwm_stop(&pwm);
        hal_pwm_finalize(&pwm);
    }
}

void noTone(uint16_t port)
{
    pwm_dev_t pwm = {port, {0.5, 1}, NULL};	// 关闭对应端口的pwm输出
    hal_pwm_stop(&pwm);
    hal_pwm_finalize(&pwm);
}
```


# 从音调到音乐
完成了蜂鸣器的驱动，可以让蜂鸣器发出我们想要频率的声音了。接下来，我们需要做的就是把这些频率组合起来，形成音乐。
## 定义音调
目前我们只能指定发声的频率，却不知道频率怎么对应音调。而遵循音调，才能拼接出音乐。如果把蜂鸣器看作我们要驱动的器件，那么频率与音调的对应关系就是通讯协议，而音乐就是理想的器件输出。
我们采用目前对常用的音乐律式——[十二平均律](https://zh.wikipedia.org/wiki/%E5%8D%81%E4%BA%8C%E5%B9%B3%E5%9D%87%E5%BE%8B)。采用维基百科的定义，可以计算如下：
将主音设为a1(440Hz)，来计算所有音的频率，结果如下（为计算过程更清晰，分数不进行约分）：

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01N7U89x22PLUMFgOIr_!!6000000007112-2-tps-1105-563.png" />
</div>


这样就得到了频率与音调的关系，我们将它记录在头文件中。
```c
// solutions/eduk1_demo/k1_apps/musicbox/pitches.h

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
... ...
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
```
这样，我们就可以采用tone方法来发出对应的音调。
```c
tone(0, NOTE_B7, 100)
// 使用pwm0对应的蜂鸣器播放 NOTE_B7 持续100ms
```
## 生成乐谱
接下来，我们就可以开始谱曲了，这里我们选用一首非常简单的儿歌——《两只老虎》，来为大家演示如何谱曲。
我们的tone方法有两个需要关注的参数：frequency决定了播放的音调，duration决定了该音调播放的时长，也就是节拍。因此我们在读简谱时，也需要关注这两个参数。
关于简谱的一些基础知识，感兴趣的同学可以参考[wikipedia-简谱](https://zh.wikipedia.org/wiki/%E7%B0%A1%E8%AD%9C)。本实验只会使用到非常简单的方法，因此也可以直接往下阅读。


<div align=center>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01oV6Kmh1LRbyKDMM1y_!!6000000001296-2-tps-605-260.png">
</div>


以《两只老虎》这张简谱为例。
### 音符
音符用数字1至7表示。这7个数字就等于大调的自然音阶。
左上角的 1 = C 表示调号，代表这张简谱使用C大调，加上音名，就会是这样：

| 1 = C |  |  |  |  |  |  |  |
| --- | --- | --- | --- | --- | --- | --- | --- |
| **音阶** | C | D | E | F | G | A | B |
| **唱名** | do | re | mi | fa | sol | la | Si |
| **数字** | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
| **代码** | NOTE_C4 | NOTE_D4 | NOTE_E4 | NOTE_F4 | NOTE_G4 | NOTE_A4 | NOTE_B4 |

如果 左上角的定义 1 = D，那么就从D开始重新标注，如下表：

| 1 = D |  |  |  |  |  |  |  |
| --- | --- | --- | --- | --- | --- | --- | --- |
| **音阶** | D | E | F | G | A | B | C |
| **唱名** | do | re | mi | fa | sol | la | Si |
| **数字** | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
| **代码** | NOTE_D4 | NOTE_E4 | NOTE_F4 | NOTE_G4 | NOTE_A4 | NOTE_B4 | NOTE_C4 |

### 八度
如果是高一个八度，就会在数字上方加上一点。如果是低一个八度，就会数字下方加上一点。在中间的那一个八度就什么也不用加。如果要再高一个八度，就在上方垂直加上两点（如：![image.png](https://img.alicdn.com/imgextra/i1/O1CN01Vfw6WJ28KCjmlPLdh_!!6000000007913-2-tps-10-29.png)）；要再低一个八度，就在下方垂直加上两点（如：![image.png](https://img.alicdn.com/imgextra/i1/O1CN011wREwN1cGwCr6prnx_!!6000000003574-2-tps-10-35.png)），如此类推。

#### 自然大调

| 1 = C |  |  |  |  |  |  | **自然大调** |
| --- | --- | --- | --- | --- | --- | --- | --- |
| **数字** | ![image.png](https://img.alicdn.com/imgextra/i1/O1CN01VoVPgb22WfpMB6Ubs_!!6000000007128-2-tps-10-33.png) | ![image.png](https://img.alicdn.com/imgextra/i1/O1CN01Vfw6WJ28KCjmlPLdh_!!6000000007913-2-tps-10-29.png) | ![image.png](https://img.alicdn.com/imgextra/i4/O1CN01IMoAHk1XFUsxjJkfP_!!6000000002894-2-tps-10-24.png) | 5 | ![image.png](https://img.alicdn.com/imgextra/i2/O1CN01T2q6co1tfWwexm0eM_!!6000000005929-2-tps-10-35.png) | ![image.png](https://img.alicdn.com/imgextra/i1/O1CN011wREwN1cGwCr6prnx_!!6000000003574-2-tps-10-35.png) | ![image.png](https://img.alicdn.com/imgextra/i3/O1CN01M18RG023JgCjbLMew_!!6000000007235-2-tps-10-47.png) |
| **代码** | NOTE_G7 | NOTE_G6 | NOTE_G5 | NOTE_G4 | NOTE_G3 | NOTE_G2 | NOTE_G1 |


#### 自然小调

| 1 = C |  |  |  |  |  |  | **自然小调** |
| --- | --- | --- | --- | --- | --- | --- | --- |
| **数字** | ![image.png](https://img.alicdn.com/imgextra/i1/O1CN01VoVPgb22WfpMB6Ubs_!!6000000007128-2-tps-10-33.png) | ![image.png](https://img.alicdn.com/imgextra/i1/O1CN01Vfw6WJ28KCjmlPLdh_!!6000000007913-2-tps-10-29.png) | ![image.png](https://img.alicdn.com/imgextra/i4/O1CN01IMoAHk1XFUsxjJkfP_!!6000000002894-2-tps-10-24.png) | 5 | ![image.png](https://img.alicdn.com/imgextra/i2/O1CN01T2q6co1tfWwexm0eM_!!6000000005929-2-tps-10-35.png) | ![image.png](https://img.alicdn.com/imgextra/i1/O1CN011wREwN1cGwCr6prnx_!!6000000003574-2-tps-10-35.png) | ![image.png](https://img.alicdn.com/imgextra/i3/O1CN01M18RG023JgCjbLMew_!!6000000007235-2-tps-10-47.png) |
| **代码** | NOTE_GS7 | NOTE_GS6 | NOTE_GS5 | NOTE_GS4 | NOTE_GS3 | NOTE_GS2 | NOTE_GS1 |



了解了音符和八度后，我们可以开始填写音调数组，这个数组里的每个元素对应 tone 方法的 frequency 参数。
```c
static int liang_zhi_lao_hu_Notes[] = {
    NOTE_C4, NOTE_D4, NOTE_E4, NOTE_C4, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_C4, 
//   两		   只		 老		  虎		 两		  只	  老		  虎
    NOTE_E4, NOTE_F4, NOTE_G4, NOTE_E4, NOTE_F4, NOTE_G4,
//	 跑		   得		 快 	  跑		 得		  快
    NOTE_G4, NOTE_A4, NOTE_G4, NOTE_F4, NOTE_E4, NOTE_C4, 
//   一		   只		 没		  有		 眼		  睛    
    NOTE_G4, NOTE_A4, NOTE_G4, NOTE_F4, NOTE_E4, NOTE_C4, 
//	 一		   只		 没		  有		 尾		  巴
    NOTE_D4, NOTE_G3, NOTE_C4, 0, 
// 	 真		   奇		 怪
    NOTE_D4, NOTE_G3, NOTE_C4, 0};
// 	 真		   奇		 怪
```
### 拍号和音长
左上角的 2/4 表示拍号。这里的4代表4分音符为一拍，2代表每一个小节里共有两拍。
通常只有数字的是[四分音符](https://zh.wikipedia.org/wiki/%E5%9B%9B%E5%88%86%E9%9F%B3%E7%AC%A6)。数字下加一条横线，就可令四分音符的长度减半，即成为[八分音符](https://zh.wikipedia.org/wiki/%E5%85%AB%E5%88%86%E9%9F%B3%E7%AC%A6)；两条横线可令八分音符的长度减半，即成为[十六分音符](https://zh.wikipedia.org/wiki/%E5%8D%81%E5%85%AD%E5%88%86%E9%9F%B3%E7%AC%A6)，以此类推；数字后方的横线延长音符，每加一条横线延长一个[四分音符](https://zh.wikipedia.org/wiki/%E5%9B%9B%E5%88%86%E9%9F%B3%E7%AC%A6)的长度。
因此我们可以得到节拍数组，这个数组里的每个元素对应 tone 方法的 duration 参数。
```c
static int liang_zhi_lao_hu_NoteDurations[] = {
    8, 8, 8, 8, 8, 8, 8, 8, 
    8, 8, 4, 8, 8, 4, 
    16, 16, 16, 16, 4, 4, 
    16, 16, 16, 16, 4, 4, 
    8, 8, 4, 4, 
    8, 8, 4, 4};
```
### 结构体定义
接下来，我们将得到的乐谱信息填入结构体当中。
```c
// solutions/eduk1_demo/k1_apps/musicbox/musicbox.c

typedef struct
{
    char *name;					// 音乐的名字	
    int *notes;					// 音符数组
    int *noteDurations;			// 节拍数组
    unsigned int noteLength;	// 音符数量
    unsigned int musicTime;		// 音乐总时长 由播放器处理 用于界面显示 用户不需要关心
} music_t;						// 音乐结构体

typedef struct
{
    music_t **music_list;			// 音乐列表
    unsigned int music_list_len;	// 音乐列表的长度
    int cur_music_index;			// 当前第几首音乐
    unsigned int cur_music_note;	// 当前音乐的第几个音符
    unsigned int cur_music_time;	// 当前的播放时长 由播放器处理 用于界面显示 用户不需要关心
    unsigned int isPlaying;			// 音乐是否播放/暂停 由播放器处理 用户不需要关心
} player_t;

static music_t liang_zhi_lao_hu = {
    "liang_zhi_lao_hu", 
    liang_zhi_lao_hu_Notes, 
    liang_zhi_lao_hu_NoteDurations, 
    34
};

music_t *music_list[] = {
    &liang_zhi_lao_hu_Notes,		// 将音乐插入到音乐列表中
};

player_t musicbox_player = {music_list, 1, 0, 0, 0, 0};	// 初始化音乐播放器
```
# 实现播放音乐
```c
while (1)
{
    // 如果当前音调下标小于这首音乐的总音调 即尚未播放完
    if (musicbox_player.cur_music_note < cur_music->noteLength)
    {
        // 通过节拍计算出当前音符需要的延时 1000ms / n分音符
        int noteDuration = 1000 / cur_music->noteDurations[musicbox_player.cur_music_note];
        // 对于附点音符 我们用读数来标记 加有一个附点后音符的音长比其原来的音长增加了一半，即原音长的1.5倍。
        noteDuration = (noteDuration < 0) ? (-noteDuration * 1.5) : noteDuration;
		// 得到当前的音调
        int note = cur_music->notes[musicbox_player.cur_music_note];
        // 使用 tone 方法播放音调
        tone(0, note, noteDuration);
        // 延时一段时间 让音调转换更清晰
        aos_msleep((int)(noteDuration * NOTE_SPACE_RATIO));
        // 计算当前的播放时间
        musicbox_player.cur_music_time += (noteDuration + (int)(noteDuration * NOTE_SPACE_RATIO));
        // 准备播放下一个音调
        musicbox_player.cur_music_note++;
    }
}
```
# 绘制播放器
作为一位有理想有追求的开发者，仅仅能播放音乐肯定没法满足我们的创造欲。所以我们再来实现一个播放器，可以做到 暂停/播放， 上一首/下一首， 还能显示歌曲名和进度条。
实现这些需要的信息，我们在结构体中都已经完成了相关的定义，只需要根据按键操作完成对应的音乐播放控制即可。
```c
void musicbox_task()
{
    while (1)
    {
        // 清除上一次绘画的残留
        OLED_Clear();
        // 获取当前音乐的指针
        music_t *cur_music = musicbox_player.music_list[musicbox_player.cur_music_index];

        // 获取当前音乐的名字并且绘制
        char show_song_name[14] = {0};
        sprintf(show_song_name, "%-13.13s", cur_music->name);
        OLED_Show_String(14, 4, show_song_name, 16, 1);

        // 如果当前播放器并未被暂停（正在播放）
        if (musicbox_player.isPlaying)
        {
       		// 如果还没播放完
            if (musicbox_player.cur_music_note < cur_music->noteLength)
            {
                int noteDuration = 1000 / cur_music->noteDurations[musicbox_player.cur_music_note];
                noteDuration = (noteDuration < 0) ? (-noteDuration * 1.5) : noteDuration;
                printf("note[%d] = %d\t delay %d ms\n", musicbox_player.cur_music_note, cur_music->noteDurations[musicbox_player.cur_music_note], noteDuration);
                int note = cur_music->notes[musicbox_player.cur_music_note];
                tone(0, note, noteDuration);
                aos_msleep((int)(noteDuration * NOTE_SPACE_RATIO));
                musicbox_player.cur_music_time += (noteDuration + (int)(noteDuration * NOTE_SPACE_RATIO));
                musicbox_player.cur_music_note++;
            }
            // 如果播放完 切换到下一首
            else
            {
                noTone(0);
                aos_msleep(1000);
                next_song();	// musicbox_player.cur_music_index++ 播放器的指向下一首音乐
            }
            OLED_Icon_Draw(54, 36, &icon_pause_24_24, 1);	// 播放器处于播放状态时 绘制暂停图标
        }
        else
        {
            OLED_Icon_Draw(54, 36, &icon_resume_24_24, 1);	// 播放器处于暂停状态时 绘制播放图标
            aos_msleep(500);
        }
        
        // 绘制一条直线代表进度条 直线的长度是 99.0(可绘画区域的最大长度) * (musicbox_player.cur_music_time(播放器记录的的当前音乐播放时长) / cur_music->musicTime(这首歌的总时长))
        OLED_DrawLine(16, 27, (int)(16 + 99.0 * (musicbox_player.cur_music_time * 1.0 / cur_music->musicTime)), 27, 1);	
 
        // 绘制上一首和下一首的图标
        OLED_Icon_Draw(94, 36, &icon_next_song_24_24, 1);
        OLED_Icon_Draw(14, 36, &icon_previous_song_24_24, 1);
        
        // 将绘制的信息显示在屏幕上
        OLED_Refresh_GRAM();
    }
}
```

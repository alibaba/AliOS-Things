@page kws 智能语音唤醒
# 实验介绍
本章主要介绍通过本地HaaS HaaS唤醒词唤醒设备实现本地语音播报响应。
开始学习之前我们先看一下显示效果，如下图所示：

<video width="100%" height="560"  controls="controls" src="https://cloud.video.taobao.com//play/u/3903519387/p/2/e/6/t/1/312891141393.mp4"></video>

# 涉及知识点

- 模拟MIC声音采样
- 喇叭播放mp3
- 本地唤醒AI推理引擎运用
- OLED绘图


# 开发环境准备
## 硬件
    开发用电脑一台
    HAAS EDU K1 开发板一块
    模拟MIC一个
    喇叭一个
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

### OLED绘图
参考上一章节。
# 软硬件环境准备

## 硬件 
    1、开发用电脑一台
    2、HaaS EDU K1开发板一个
    3、USB Type-C 数据线一根
    4、外接模拟MIC一个

[模拟MIC购买链接](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.795a2e8drZ42nl&id=583316469629&_u=531h6c618fe)
[喇叭购买链接](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.6c4e2e8de1Ubch&id=564311744050&_u=ob3iuit4288)

# 硬件接线图
在本实验中，喇叭和模拟MIC与HaaS EDUK1连接如下所示：

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01XKHM5s1djcdqTJo1S_!!6000000003772-2-tps-1394-724.png" style="zoom:50%;" />
</div>
MIC也可以接MIC1_P，喇叭的in也可以接到SPK_LN。

## 软件设计
#### 应用代码部分
文件路径如下：
solutions/eduk1_demo/k1_apps/kws/kws.c
solutions/eduk1_demo/k1_apps/kws/kws.h
```basic
# 唤醒后蜂鸣器发出声音，并播放本地响应TTS，等待播放结束重现显示语音唤醒初始界面

int32_t kws_callback(ai_result_t *result)
{
    int32_t kws_ret = (int32_t)*result;
    player_state_t player_state = -1;

    if (kws_ret) {
        beeper_start(0, 1, 25);
        OLED_Clear();
        OLED_Show_String(14, 24, "Hi, I am here!", 16, 1);
        OLED_Refresh_GRAM();
        beeper_stop(0);

        /*play local asr*/
        play_local_mp3();

        uvocplayer->wait_complete();

        OLED_Clear();
        OLED_Show_String(28, 16, "HaaS HaaS!", 16, 1);
        OLED_Show_String(28, 34, "Wakeup me!", 16, 1);

        OLED_Icon_Draw(2, 24, &icon_skip_left, 0);
        OLED_Icon_Draw(122, 24, &icon_skip_right, 0);
        OLED_Refresh_GRAM();
    }

    return 0;
}
```



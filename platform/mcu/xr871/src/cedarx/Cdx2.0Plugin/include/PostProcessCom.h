
#ifndef _POST_PROCESS_COM_H_
#define _POST_PROCESS_COM_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define MaxSpecGroup        (10)

//输入采样数为1024的整数倍，如果变速播放，需要考虑输入和输出的地址不相同。
typedef struct __PostProcessSt
{
    short       fadeinoutflag;  //0:normal 1:fade in 2:fade out
    short       VPS;            //-40 - 100, 播放速度为: (1+vps%),所以变速范围是原始速度的0.6~2倍
    short       spectrumflag;   //0 : disable 1:enable
    short       spectrumOutNum;//输出频谱数
    short       SpectrumOutval[MaxSpecGroup][32];//范围为 0-20，melis采用10段高度显示。32个频段。1024个采样产生一个频谱（32频段）
    short       UserEQ[11];     //0-10
    int         channal;        //1:mono 2:stereo
    int         samplerate;

    short       *InputPcmBuf;
    int         InputPcmLen;//采样数，非bufferlenth。

    short       *OutputPcmBuf;
    int         OutputPcmBufTotLen;//outputpcmbuf 最多sample数；bufferlenth ＝2×channels × OutputPcmBufTotLen
    int         OutputPcmLen;

}PostProcessSt;

extern int AudioEQdo_auPostProc(PostProcessSt *PostProInfo); //0:fail 1:succed

#ifdef __cplusplus
}
#endif

#endif  //_POST_PROCESS_COM_H_

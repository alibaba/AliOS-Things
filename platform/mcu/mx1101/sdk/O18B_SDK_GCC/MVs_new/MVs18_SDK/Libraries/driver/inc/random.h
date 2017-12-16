#ifndef __RANDOM_H__
#define __RANDOM_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

//  maintainer: Halley
#include "type.h"


//将本模块的随机函数用于随机播放时，可以达到下面的效果，假设总曲目数为n
//1. 任意一首歌曲，都不会连续两次播放
//2. 任意一首歌曲，两次播放之间最多穿插(2*n - 2)首其余歌曲
//3. 任意一首歌曲，在连续的n首歌曲中，最多被播放两次

//获得1--MaxNum之间的不重复伪随机序列。
//某次调用本函数时，如果MaxNum参数与前一次发生变化，
//则表示要产生一个新的伪随机序列，底层会重新初始化伪随机序列发生器。
uint16_t GetRandomNum(uint16_t seed, uint16_t MaxNum);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif

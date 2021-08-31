/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __HAL_HWFFT__
#define __HAL_HWFFT__

#ifdef __cplusplus
extern "C" {
#endif

//#include "cmsis_os.h"

#define HWFFT_USE_BUSY_WAIT 1
#define HAVE_EQUIVALENT_MODE 1 /* 128in 128out not 128in 130out mode  */

struct hwfft_t {
    int mode;
    int order;
    int sym_fft;
    //osSemaphoreId _osSemaphoreId;
   // osSemaphoreDef_t _osSemaphoreDef;
//#ifdef CMSIS_OS_RTX
//    uint32_t _semaphore_data[2];
//#endif
};

enum hwfft_mode_t {
    HWFFT_MODE_FFT = 0x1,
    HWFFT_MODE_IFFT = 0x2,
    HWFFT_MODE_EQVL_FFT = 0x4,
    HWFFT_MODE_EQVL_IFFT = 0x8,
};

enum hwfft_order_t {
    HWFFT_ORDER_64SAMP  = 64,
    HWFFT_ORDER_128SAMP = 128,
    HWFFT_ORDER_256SAMP = 256,
    HWFFT_ORDER_512SAMP = 512,
};

bool hwfft_is_enable(void);
int hwfft_destroy(struct hwfft_t *fft_inst);
/*
生成hwfft_t 结构体
mode: hwfft_mode_t类型HWFFT_MODE_FFT:  复数， 输入n个复数，输出n个复数，此时sym_fft必须为0；
                                                                  实数，输入n个实数，输出是n/2+1 个复数，此时sym_fft必须为1；
                                      HWFFT_MODE_IFFT:  复数， 输入n个复数，输出n个复数，此时sym_fft必须为0；
                                                                   实数，输入 n/2+1个复数，输出n个实数；此时sym_fft必须为1；
                                      HWFFT_MODE_EQVL_FFT: 兼容软件spx_fft计算fft模式
                                      HWFFT_MODE_EQVL_IFFT: 兼容软件spx_ifft计算ifft模式
                                                                 
order: hwfft_order_t类型，支持 512/256/128/64 点的 fft/ifft

sym_fft:  1: 将利用fft的对称性;对fft，输入n个实数，输出是n/2+1 个复数；
                对ifft， 输入 n/2+1个复数，输出n个实数；n 为fft/ifft点数
              0: 输入n个复数，输出n个复数
*/
int hwfft_create(struct hwfft_t *fft_inst, int mode, int order, int sym_fft);
/*
计算fft/ifft
fft_inst: hwfft_create生成的对应结构体
in: 输入buf
out: 输出buf
*/
int hwfft_fft_process(struct hwfft_t *fft_inst, short *in, short *out);
/*
连续计算多次 fft/ifft
fft_inst: hwfft_create生成的对应结构体
in: 输入buf，多次fft的输入buf必须是连续的
out: 输出buf
num: 计算次数
mode: 0 一直等到fft完成函数才返回
         1 dma配置完成，fft开始函数即可返回，cpu可以在硬件fft 时运行其他程序
*/
int hwfft_multifft_process(struct hwfft_t *fft_inst, short *in, short *out, unsigned short num,unsigned char mode);

#ifdef __cplusplus
}
#endif

#endif

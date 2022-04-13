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
����hwfft_t �ṹ��
mode: hwfft_mode_t����HWFFT_MODE_FFT:  ������ ����n�����������n����������ʱsym_fft����Ϊ0��
                                                                  ʵ��������n��ʵ���������n/2+1 ����������ʱsym_fft����Ϊ1��
                                      HWFFT_MODE_IFFT:  ������ ����n�����������n����������ʱsym_fft����Ϊ0��
                                                                   ʵ�������� n/2+1�����������n��ʵ������ʱsym_fft����Ϊ1��
                                      HWFFT_MODE_EQVL_FFT: ��������spx_fft����fftģʽ
                                      HWFFT_MODE_EQVL_IFFT: ��������spx_ifft����ifftģʽ
                                                                 
order: hwfft_order_t���ͣ�֧�� 512/256/128/64 ��� fft/ifft

sym_fft:  1: ������fft�ĶԳ���;��fft������n��ʵ���������n/2+1 ��������
                ��ifft�� ���� n/2+1�����������n��ʵ����n Ϊfft/ifft����
              0: ����n�����������n������
*/
int hwfft_create(struct hwfft_t *fft_inst, int mode, int order, int sym_fft);
/*
����fft/ifft
fft_inst: hwfft_create���ɵĶ�Ӧ�ṹ��
in: ����buf
out: ���buf
*/
int hwfft_fft_process(struct hwfft_t *fft_inst, short *in, short *out);
/*
���������� fft/ifft
fft_inst: hwfft_create���ɵĶ�Ӧ�ṹ��
in: ����buf�����fft������buf������������
out: ���buf
num: �������
mode: 0 һֱ�ȵ�fft��ɺ����ŷ���
         1 dma������ɣ�fft��ʼ�������ɷ��أ�cpu������Ӳ��fft ʱ������������
*/
int hwfft_multifft_process(struct hwfft_t *fft_inst, short *in, short *out, unsigned short num,unsigned char mode);

#ifdef __cplusplus
}
#endif

#endif

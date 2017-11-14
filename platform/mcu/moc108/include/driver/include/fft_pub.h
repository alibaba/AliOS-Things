#ifndef _FFT_PUB_H_
#define _FFT_PUB_H_

#define FFT_FAILURE                (1)
#define FFT_SUCCESS                (0)

#define FFT_DEV_NAME                "fft"

#define FFT_CMD_MAGIC              (0xe260000)
enum
{
    CMD_FFT_BUSY = FFT_CMD_MAGIC + 1,
    CMD_FFT_ENABLE,
    CMD_FIR_SIGNLE_ENABLE
};

enum
{
    FFT_MODE_FFT,
    FFT_MODE_IFFT
};

typedef struct
{
    int mode;
    INT16 *inbuf;
    INT16 *outbuf;
    UINT16 size;
} input_fft_t;

typedef struct
{
    UINT8 fir_len;
    UINT8 fir_cwidth;
    UINT8 fir_dwidth;
    INT16 *coef;
    INT16 *input;
    INT32 *mac;
} input_fir_t;

/*******************************************************************************
* Function Declarations
*******************************************************************************/
void fft_init(void);
void fft_exit(void);
void fft_isr(void);
#endif //_FFT_PUB_H_

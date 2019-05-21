#ifndef _FFT_H_
#define _FFT_H_

#define FFT_DEBUG

#ifdef FFT_DEBUG
#define FFT_PRT      os_printf
#define FFT_WARN     warning_prf
#define FFT_FATAL    fatal_prf
#else
#define FFT_PRT      null_prf
#define FFT_WARN     null_prf
#define FFT_FATAL    null_prf
#endif

#define FFT_BASE                                     (0x00805000)

#define FFT_CONF                                     (FFT_BASE + 0 * 4)
#define FFT_CONF_MODE                                (0x01UL << 0)
#define FFT_CONF_IFFT                                (0x01UL << 1)
#define FFT_CONF_INT_EN                              (0x01UL << 2)
#define FFT_CONF_ENABLE                              (0x01UL << 3)
#define FFT_CONF_GAT                                 (0x01UL << 4)

#define FIR_CONF                                     (FFT_BASE + 1 * 4)
#define FIR_CONF_LEN_POSI                            (0)
#define FIR_CONF_LEN_MASK                            (0xFF)
#define FIR_CONF_CWIDTH_POSI                         (8)
#define FIR_CONF_CWIDTH_MASK                         (0x07)
#define FIR_CONF_DWIDTH_POSI                         (11)
#define FIR_CONF_DWIDTH_MASK                         (0x07)
#define FIR_CONF_MODE                                (0x01UL << 14)
#define FIR_CONF_INT_EN                              (0x01UL << 15)
#define FIR_CONF_ENABLE                              (0x01UL << 16)

#define FFT_DATA_PORT                                (FFT_BASE + 2 * 4)

#define FFT_COEF_PORT                                (FFT_BASE + 3 * 4)

#define FFT_MAC_LOW                                  (FFT_BASE + 4 * 4)

#define FFT_MAC_HIGH                                 (FFT_BASE + 5 * 4)

#define FFT_STATUS                                   (FFT_BASE + 6 * 4)
#define FFT_STATUS_DONE                              (0x01UL << 0)
#define FIR_STATUS_DONE                              (0x01UL << 1)

#define FFT_START                                    (FFT_BASE + 7 * 4)

#define FFT_MEMORY_BASE_ADDR                         (FFT_BASE+0x200)

#define Fft_Set_Start()                              REG_WRITE(FFT_START, 0x1)
#define Fft_Write_Data(v)                            REG_WRITE(FFT_DATA_PORT, v)
#define Fft_Read_Data()                              REG_READ(FFT_DATA_PORT)
#define Fft_Write_Coef(v)                            REG_WRITE(FFT_COEF_PORT, v)

typedef struct
{
    INT16 *in;
    INT16 *out;
    INT32 *mac_out;
    volatile UINT8 busy_flag;
    UINT16 size;
} driver_fft_t;

/*******************************************************************************
* Function Declarations
*******************************************************************************/
UINT32 fft_ctrl(UINT32 cmd, void *param);
#endif //_FFT_H_

#include "include.h"
#include "arm_arch.h"

#include "fft.h"
#include "fft_pub.h"

#include "drv_model_pub.h"
#include "intc_pub.h"

static driver_fft_t driver_fft;
static SDD_OPERATIONS fft_op =
{
    fft_ctrl
};

static int fft_busy(void)
{
    return driver_fft.busy_flag;
}

static void fft_enable(input_fft_t *fft_conf)
{
    int i;
    UINT32 value = 0;

    REG_WRITE(FIR_CONF, 0);
    REG_WRITE(FFT_CONF, 0);

    if(fft_conf->mode == FFT_MODE_FFT)
    {
        value = FFT_CONF_INT_EN | FFT_CONF_ENABLE;
    }
    else if(fft_conf->mode == FFT_MODE_IFFT)
    {
        value = FFT_CONF_IFFT | FFT_CONF_INT_EN | FFT_CONF_ENABLE;
    }
    REG_WRITE(FFT_CONF, value);

    driver_fft.in  = fft_conf->inbuf;
    driver_fft.out = fft_conf->outbuf;
    driver_fft.size = fft_conf->size;
    driver_fft.busy_flag = 1;

    for(i = 0; i < fft_conf->size; i++)
    {
        value = (UINT16)(fft_conf->inbuf[i]) << 16 | (UINT16)(fft_conf->outbuf[i]);
        Fft_Write_Data(value);
    }

    Fft_Set_Start();
}

static void fir_single_enable(input_fir_t *fir_conf)
{
    int i;
    UINT32 value = 0;

    if(fir_conf->fir_len > 64)
        return;

    REG_WRITE(FIR_CONF, 0);
    REG_WRITE(FFT_CONF, 0);

    value = (fir_conf->fir_len << FIR_CONF_LEN_POSI)
            | (fir_conf->fir_cwidth << FIR_CONF_CWIDTH_POSI)
            | (fir_conf->fir_dwidth << FIR_CONF_DWIDTH_POSI)
            | FIR_CONF_INT_EN
            | FIR_CONF_ENABLE;
    REG_WRITE(FIR_CONF, value);

    driver_fft.mac_out = fir_conf->mac;
    driver_fft.busy_flag = 1;

    for(i = 0; i < fir_conf->fir_len + 1; i++)
    {
        Fft_Write_Coef(fir_conf->coef[i]);
        Fft_Write_Data(fir_conf->input[i]);
    }

    Fft_Set_Start();

    return;
}

static __inline INT16 f_sat(int din)
{
    if (din > 32767)
        return 32767;
    if (din < -32767)
        return -32767;
    else
        return(din);
}

void fft_init(void)
{
    intc_service_register(IRQ_FFT, PRI_IRQ_FFT, fft_isr);

    sddev_register_dev(FFT_DEV_NAME, &fft_op);
}

void fft_exit(void)
{
    sddev_unregister_dev(FFT_DEV_NAME);
}

UINT32 fft_ctrl(UINT32 cmd, void *param)
{
    int ret = FFT_SUCCESS;

    switch(cmd)
    {
    case CMD_FFT_BUSY:
        *((int *)param) = fft_busy();
        break;
    case CMD_FFT_ENABLE:
        fft_enable((input_fft_t *)param);
        break;
    case CMD_FIR_SIGNLE_ENABLE:
        fir_single_enable((input_fir_t *)param);
        break;
    default:
        ret = FFT_FAILURE;
        break;
    }

    return ret;
}

void fft_isr(void)
{
    UINT32 int_status;
    UINT8 FFT_int, FIR_int;

    int_status = REG_READ(FFT_STATUS);
    FFT_int = int_status & FFT_STATUS_DONE;
    FIR_int = int_status & FIR_STATUS_DONE;

    if(FFT_int)
    {
        int i;
        int bit_ext;
        INT32 temp, temp_out;

        bit_ext = (int_status & 0x00001fff) >> 7;

        if( bit_ext & 0x20 )
        {
            bit_ext = 64 - bit_ext;
        }
        else
        {
            bit_ext = 0;
        }

        for(i = 0; i < driver_fft.size; i++)
        {
            temp = Fft_Read_Data();
            temp_out = ((temp << 16) >> 16) <<  bit_ext ;
            driver_fft.out[i] = f_sat(temp_out);
            temp_out = (temp  >> 16) << bit_ext;
            driver_fft.in[i]  = f_sat(temp_out);
        }

        driver_fft.busy_flag = 0;
        REG_WRITE(FFT_CONF, 0);
    }

    if(FIR_int)
    {
        *driver_fft.mac_out = REG_READ(FFT_MAC_LOW);

        driver_fft.busy_flag = 0;
        REG_WRITE(FIR_CONF, 0);
    }
}

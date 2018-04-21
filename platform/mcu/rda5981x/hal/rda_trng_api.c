#include "rda_dma.h"
#include "hal/soc/soc.h"
#include "rda5981.h"

#ifdef RDA_HW_ACCELERATE_ENABLE

#include <string.h>
//#include "RDA5991H.h"
//#include "wait_api.h"

/* TRNG CTRL DESC. */
// ctrl, default value 0xC
#define    TRNG_EN              (1<<0)
#define    TRNG_CONT_MODE       (1<<1)
#define    TRNG_ONCE_MODE       (0<<1)
#define    TRNG_START_EN        (1<<2)
#define    TRNG_ON_FLY_EN       (1<<3)
#define    TRNG_ON_FLY_DISABLE  (0<<3)
#define    TRNG_SRC_OPEN        (1<<4)
#define    TRNG_TEST_EN         (1<<5)
#define    TRNG_DATA_MASK_EN    (1<<6)
// mask, default value 0x0
#define    TRNG_SRC_MASK        (0xff<<16)

#define RDA_TRNG_CGEN_ENABLE()         do { \
                                            RDA_DMACFG->dma_func_ctrl |= (1 << 30); \
                                         } while(0)

#define RDA_TRNG_CGEN_DISABLE()        do { \
                                            RDA_DMACFG->dma_func_ctrl &= ~(1 << 30); \
                                         } while(0)
/* a very simple delay */
static void rda_trng_simple_delay(int n)
{
    int i, j;
    for (i=0;i<n;i++)
        for (j=0;j<1000;j++)
            asm volatile("");
}
void rda_trng_simple_delay(int n) __attribute__ ((noinline));

int rda_trng_generator( unsigned char *output, uint32_t output_len ) 
{
    uint32_t idx = 0;
    uint32_t d0;

    uint8_t  *temp_buf = (uint8_t  *)malloc(output_len + 8);
    if(NULL ==temp_buf) return (-1);

    RDA_SCU_CLK_GATE0_ENABLE();
    RDA_TRNG_CGEN_ENABLE();

#ifdef GEN_TRNG_CONTINUOUSLY
    uint32_t d1, ret;

flag_0:
    RDA_DMACFG->trng_ctrl = TRNG_SRC_MASK | TRNG_EN | TRNG_CONT_MODE | TRNG_START_EN \
                            | TRNG_ON_FLY_EN | TRNG_SRC_OPEN;//0xff001f;

    while (1) {
        ret = RDA_DMACFG->dma_int_out;

        if(ret&TRNG_ON_FLY_TEST_FAIL) {
            RDA_DMACFG->dma_int_out |= TRNG_ON_FLY_TEST_FAIL ;
            goto flag_0;
        }
        if(ret&TRNG_START_TEST_FAIL) {
            RDA_DMACFG->dma_int_out |= TRNG_START_TEST_FAIL ;
            goto flag_0;
        }

        if(ret&TRNG_DATA_READY) {
            d0 = RDA_DMACFG->trng_data0;
            d1 = RDA_DMACFG->trng_data1;

            RDA_DMACFG->dma_int_out |= TRNG_DATA_READY ;

            PUT_UINT32_BE(d0, temp_buf, 4*idx);
            idx++;
            PUT_UINT32_BE(d1, temp_buf, 4*idx);
            idx++;
            if(output_len <= 4*idx) break;
            goto flag_0;
        }
    }
#else
    RDA_DMACFG->trng_ctrl = TRNG_SRC_MASK | TRNG_EN | TRNG_ONCE_MODE | TRNG_SRC_OPEN\
                            | TRNG_ON_FLY_DISABLE; //0x00ff0011;

    while(1) {
        rda_trng_simple_delay(5);
        d0 = RDA_DMACFG->prng_data;
        PUT_UINT32_BE(d0, temp_buf, 4*idx);
        idx++;
        if(output_len <= 4*idx) break;
    }

#endif

    RDA_TRNG_CGEN_DISABLE();
    RDA_SCU_CLK_GATE0_DISABLE();

    memcpy(output, temp_buf, output_len);
    if(temp_buf) {
        free(temp_buf);
    }
    return (0);
}
#endif /* RDA_HW_ACCELERATE_ENABLE */

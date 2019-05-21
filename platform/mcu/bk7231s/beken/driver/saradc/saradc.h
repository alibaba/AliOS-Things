#ifndef _SARADC_H_
#define _SARADC_H_

#if (CFG_SOC_NAME == SOC_BK7231)
#define SARADC_BASE                     (0x00802C00)
#define SARADC_ADC_CONFIG               (SARADC_BASE + 0 * 4)
#define SARADC_ADC_MODE_POSI            (0)
#define SARADC_ADC_MODE_MASK            (0x03)
#define SARADC_ADC_CHNL_EN              (0x01UL << 2)
#define SARADC_ADC_CHNL_POSI            (3)
#define SARADC_ADC_CHNL_MAX             (7)
#define SARADC_ADC_CHNL_MASK            (0x07)
#define SARADC_ADC_FIFO_FULL            (0x01UL << 29)
#define SARADC_ADC_FIFO_EMPTY           (0x01UL << 28)
#define SARADC_ADC_BUSY                 (0x01UL << 27)
#define SARADC_ADC_SAMPLE_RATE_POSI     (16)
#define SARADC_ADC_SAMPLE_RATE_MASK     (0x00FF)
#define SARADC_ADC_DELAY_CLK_POSI       (6)
#define SARADC_ADC_SETTING              (0x01UL << 6)
#define SARADC_ADC_INT_CLR              (0x01UL << 7)
#define SARADC_ADC_PRE_DIV_POSI         (8)
#define SARADC_ADC_PRE_DIV_MASK         (0x00FF)
#define SARADC_ADC_FILTER_POSI          (24)
#define SARADC_ADC_FILTER_MASK          (0x07)

#define SARADC_ADC_DATA                 (SARADC_BASE + 1 * 4)

#else
#define SARADC_BASE                     (0x00802C00)
#define SARADC_ADC_CONFIG               (SARADC_BASE + 0 * 4)
#define SARADC_ADC_MODE_POSI            (0)
#define SARADC_ADC_MODE_MASK            (0x03)
#define SARADC_ADC_CHNL_EN              (0x01UL << 2)
#define SARADC_ADC_CHNL_POSI            (3)
#define SARADC_ADC_CHNL_MAX             (12)
#define SARADC_ADC_CHNL_MASK            (0x0F)
#define SARADC_ADC_DELAY_CLK_POSI       (7)
#define SARADC_ADC_SETTING              (0x01UL << 7)
#define SARADC_ADC_INT_CLR              (0x01UL << 8)
#define SARADC_ADC_PRE_DIV_POSI         (9)
#define SARADC_ADC_PRE_DIV_MASK         (0x3F)
#define SARADC_ADC_32M_MODE             (0x01UL << 15)
#define SARADC_ADC_SAMPLE_RATE_POSI     (16)
#define SARADC_ADC_SAMPLE_RATE_MASK     (0x3F)
#define SARADC_ADC_FILTER_POSI          (22)
#define SARADC_ADC_FILTER_MASK          (0x7F)
#define SARADC_ADC_BUSY                 (0x01UL << 29)
#define SARADC_ADC_FIFO_EMPTY           (0x01UL << 30)
#define SARADC_ADC_FIFO_FULL            (0x01UL << 31)

#define SARADC_ADC_DATA                 (SARADC_BASE + 1 * 4)

#define SARADC_ADC_CTRL_CFG             (SARADC_BASE + 2 * 4)
#define SARADC_ADC_STEADY_CTRL_POSI     (5)
#define SARADC_ADC_STEADY_CTRL_MASK     (0x7)
#define SARADC_ADC_ALMOST_CFG_POSI      (0)
#define SARADC_ADC_ALMOST_CFG_MASK      (0x1F)

#define SARADC_ADC_SATURATION_CFG       (SARADC_BASE + 3 * 4)
#define SARADC_ADC_SAT_CTRL_POSI        (0)
#define SARADC_ADC_SAT_CTRL_MASK        (0x3)
#define SARADC_ADC_SAT_ENABLE           (0x01UL << 2)
#define SARADC_ADC_SAT_OVERFLOW         (0x01UL << 3)

#define SARADC_ADC_DAT_AFTER_STA        (SARADC_BASE + 4 * 4)
#endif

/*******************************************************************************
* Function Declarations
*******************************************************************************/
static UINT32 saradc_open(UINT32 op_flag);
static UINT32 saradc_close(void);
static UINT32 saradc_ctrl(UINT32 cmd, void *param);
#endif //_SARADC_H_

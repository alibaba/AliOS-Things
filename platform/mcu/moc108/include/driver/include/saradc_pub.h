#ifndef _SARADC_PUB_H_
#define _SARADC_PUB_H_

#define SARADC_FAILURE                (1)
#define SARADC_SUCCESS                (0)

#define SARADC_DEV_NAME                "saradc"

#define SARADC_CMD_MAGIC              (0xe290000)
enum
{
    SARADC_CMD_SET_MODE = SARADC_CMD_MAGIC + 1,
    SARADC_CMD_SET_CHANNEL,
    SARADC_CMD_SET_SAMPLE_RATE,
    SARADC_CMD_SET_WAITING_TIME,
    SARADC_CMD_SET_VALID_MODE,
    SARADC_CMD_CLEAR_INT,
    SARADC_CMD_SET_CLK_RATE,
    SARADC_CMD_GET_VAULE_WITHOUT_ISR
};


#define ADC_CONFIG_MODE_SLEEP           (0x00UL)
#define ADC_CONFIG_MODE_STEP            (0x01UL)
#define ADC_CONFIG_MODE_SOFT_CTRL       (0x02UL)
#define ADC_CONFIG_MODE_CONTINUE        (0x03UL)

#define ADC_CONFIG_MODE_36DIV           (0x00UL)
#define ADC_CONFIG_MODE_18DIV           (0x01UL)

typedef struct
{
    UINT16 *pData;
    UINT8 current_sample_data_cnt;
    UINT8 current_read_data_cnt;
    UINT8 data_buff_size;
    UINT8 has_data; /* 0: has data      1: no data*/
    UINT8 channel;
    /* mode:     ADC mode
     * bit[0:1]: ADC operation mode
     *          00:  ADC power down mode
     *          01:  ADC one-step mode
     *          10:  ADC software control mode
     *          11:  ADC continuous mode
     * bit[3:2]: ADC sample rate selection
     *          00: adc_clk/36
     *          01: adc_clk/18
     *          10: Reserved
     *          11: Reserved
     * bit[7:4]: reserved
     */
    UINT8 mode;
    void (*p_Int_Handler)(void);
} saradc_desc_t;

typedef struct
{
    UINT8 enable;
    UINT8 channel;
} saradc_chan_t;


/*******************************************************************************
* Function Declarations
*******************************************************************************/
void saradc_init(void);
void saradc_exit(void);
void saradc_isr(void);
#endif //_SARADC_PUB_H_

/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <drv/adc.h>
#include <aos/adc_csi.h>
#include "objects.h"
#include "PinNames.h"
#include "pinmap.h"

static const PinMap CSI_PinMap_ADC[] = {
    { PB_4, ADC_CH0, 0 },
    { PB_5, ADC_CH1, 0 },
    { PB_6, ADC_CH2, 0 },
    { PB_7, ADC_CH3, 0 },
    { PB_1, ADC_CH4, 0 },
    { PB_2, ADC_CH5, 0 },
    { PB_3, ADC_CH6, 0 },
    { VBAT_MEAS, ADC_CH7, 0 },
    { NC, NC, 0 }
};

static uint16_t rtl872xd_adc_offset;
static uint16_t rtl872xd_adc_gain;

csi_error_t csi_adc_init(csi_adc_t *adc, uint32_t idx)
{
    if (!adc)
        return CSI_ERROR;

    adc->priv = (ADC_InitTypeDef *)malloc(sizeof(ADC_InitTypeDef));
    ADC_InitTypeDef *ADC_InitStruct = (ADC_InitTypeDef *)adc->priv;

    adc->dev.idx = idx;

    /* Initialize ADC */
    ADC_StructInit(ADC_InitStruct);
    ADC_Init(ADC_InitStruct);
    return CSI_OK;
}

void csi_adc_uninit(csi_adc_t *adc)
{
    /* Clear ADC Status */
    ADC_INTClear();
    /* Disable ADC  */
    ADC_Cmd(DISABLE);

    if (adc->priv) {
        free(adc->priv);
        adc->priv = NULL;
    }
}

csi_error_t csi_adc_start(csi_adc_t *adc)
{
    ADC_Cmd(ENABLE);
    return CSI_OK;
}

csi_error_t csi_adc_stop(csi_adc_t *adc)
{
    ADC_Cmd(DISABLE);
    return CSI_OK;
}

csi_error_t csi_adc_channel_enable(csi_adc_t *adc, uint8_t ch_id, bool is_enable)
{
    ADC_InitTypeDef *ADC_InitStruct = (ADC_InitTypeDef *)adc->priv;
    uint32_t adc_idx;

    if (ch_id != adc->dev.idx)
        adc->dev.idx = ch_id;

    adc_idx = CSI_PinMap_ADC[ch_id].peripheral;

    ADC_InitStruct->ADC_CvlistLen = 0;
    ADC_InitStruct->ADC_Cvlist[0] = adc_idx;
    ADC_InitStruct->ADC_ChIDEn = is_enable; /* MSB 4bit is channel index*/
    ADC_Init(ADC_InitStruct);
    return CSI_OK;
}

csi_error_t csi_adc_sampling_time(csi_adc_t *adc, uint16_t clock_num)
{
    /* no need to set sampling time */
    return CSI_OK;
}

csi_error_t csi_adc_channel_sampling_time(csi_adc_t *adc, uint8_t ch_id, uint16_t clock_num)
{
    /* no need to set sampling time */
    return CSI_OK;
}

csi_error_t csi_adc_continue_mode(csi_adc_t *adc, bool is_enable)
{
    ADC_InitTypeDef *ADC_InitStruct = (ADC_InitTypeDef *)adc->priv;

    if (is_enable) {
        ADC_InitStruct->ADC_OpMode = ADC_AUTO_MODE;
        ADC_Init(ADC_InitStruct);
    }

    return CSI_OK;
}

uint32_t csi_adc_freq_div(csi_adc_t *adc, uint32_t div)
{
    ADC_InitTypeDef *ADC_InitStruct = (ADC_InitTypeDef *)adc->priv;
    uint32_t freq = 0;

    switch (div) {
    case 12:
        ADC_InitStruct->ADC_ClkDiv = ADC_CLK_DIV_12;
        freq = (uint32_t)(2000000 / 12);
        break;
    case 16:
        ADC_InitStruct->ADC_ClkDiv = ADC_CLK_DIV_16;
        freq = (uint32_t)(2000000 / 16);
        break;
    case 32:
        ADC_InitStruct->ADC_ClkDiv = ADC_CLK_DIV_32;
        freq = (uint32_t)(2000000 / 32);
        break;
    case 64:
        ADC_InitStruct->ADC_ClkDiv = ADC_CLK_DIV_64;
        freq = (uint32_t)(2000000 / 64);
        break;
    default:
        ADC_InitStruct->ADC_ClkDiv = ADC_CLK_DIV_12;
        freq = (uint32_t)(2000000 / 12);
        break;
    }

    ADC_Init(ADC_InitStruct);
    return freq;
}

static int32_t AD2MV(int32_t ad, uint16_t offset, uint16_t gain)
{
    return (int32_t)(((10 * ad) - offset) * 1000 / gain);
}

static void adc_get_offset_gain(uint16_t *offset, uint16_t *gain, uint8_t vbat)
{
    uint8_t EfuseBuf[2];
    uint32_t index;
    uint32_t addressOffset;
    uint32_t addressGain;

    if (vbat) {
        addressOffset = 0x1D4;
        addressGain = 0x1D6;
    } else {
        addressOffset = 0x1D0;
        addressGain = 0x1D2;
    }

    for (index = 0; index < 2; index++) {
        EFUSERead8(0, addressOffset + index, EfuseBuf + index, L25EOUTVOLTAGE);
    }
    *offset = EfuseBuf[1] << 8 | EfuseBuf[0];

    for (index = 0; index < 2; index++) {
        EFUSERead8(0, addressGain + index, EfuseBuf + index, L25EOUTVOLTAGE);
    }
    *gain = EfuseBuf[1] << 8 | EfuseBuf[0];

    if (*offset == 0xFFFF) {
        if (vbat)
            *offset = 0x9C4;
        else
            *offset = 0x9B0;
    }

    if (*gain == 0xFFFF) {
        if (vbat)
            *gain = 7860;
        else
            *gain = 0x2F12;
    }
}

csi_error_t csi_adc_get_range(csi_adc_t *adc, uint8_t ch_id, uint32_t *range)
{
    if (ch_id < 4) {
        *range = 3300;  /*CH0~CH3: 3.3V*/
        return CSI_OK;
    } else if (ch_id < 7) {
        *range = 1800;  /*CH4~CH6: 1.8V*/
        return CSI_OK;
    } else if (ch_id == 7) {
        *range = 5000;  /*CH7: 5V*/
        return CSI_OK;
    } else {
        return CSI_ERROR;
    }
}

int32_t csi_adc_read(csi_adc_t *adc)
{
    int32_t value;
    uint32_t data;

    /* Clear FIFO */
    ADC_ClearFIFO();

    /* SW trigger to sample */
    ADC_SWTrigCmd(ENABLE);
    while (ADC_Readable() == 0)
        ;
    ADC_SWTrigCmd(DISABLE);

    data = ADC_Read();
    value = (int32_t)(data & BIT_MASK_DAT_GLOBAL);
    return value;
}

int32_t csi_adc_read_voltage(csi_adc_t *adc)
{
    int32_t adc_data;

    adc_data = csi_adc_read(adc);
    return AD2MV(adc_data, rtl872xd_adc_offset, rtl872xd_adc_gain);
}

static int adc_csi_init(void)
{
    csi_error_t ret;
    static aos_adc_csi_t adc_csi_dev;

    adc_get_offset_gain(&rtl872xd_adc_offset, &rtl872xd_adc_gain, 0);
    if (rtl872xd_adc_gain == 0) {
        printf("%s:%d: rtl872xd_adc_gain is 0\r\n", __func__, __LINE__);
        return -1;
    }

    ret = csi_adc_init(&(adc_csi_dev.csi_adc), 0);
    if (ret != CSI_OK) {
        printf("%s:%d: csi_adc_init fail, ret:%d\r\n", __func__, __LINE__, ret);
        return -1;
    }

    adc_csi_dev.aos_adc.dev.id = 0;
    adc_csi_dev.aos_adc.resolution = 12;
    adc_csi_dev.aos_adc.freq = 2000000 / 12;
    return aos_adc_csi_register(&adc_csi_dev);
}

LEVEL1_DRIVER_ENTRY(adc_csi_init)

/*!
    \file  gd32f30x_dac.c
    \brief DAC driver
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F30x
*/

#include "gd32f30x_dac.h"

/*!
    \brief      deinitialize DAC
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_deinit(void)
{
    rcu_periph_reset_enable(RCU_DACRST);
    rcu_periph_reset_disable(RCU_DACRST);
}

/*!
    \brief      enable DAC
    \param[in]  dac_periph
      \arg        DACx(x =0,1)
    \param[out] none
    \retval     none
*/
void dac_enable(uint32_t dac_periph)
{
    if(DAC0 == dac_periph){
        DAC_CTL |= DAC_CTL_DEN0;
    }else{
        DAC_CTL |= DAC_CTL_DEN1;
    }
} 

/*!
    \brief      disable DAC
    \param[in]  dac_periph
      \arg        DACx(x =0,1)
    \param[out] none
    \retval     none
*/
void dac_disable(uint32_t dac_periph)
{
    if(DAC0 == dac_periph){
        DAC_CTL &= ~DAC_CTL_DEN0;
    }else{
        DAC_CTL &= ~DAC_CTL_DEN1;
    }
}

/*!
    \brief      enable DAC DMA function
    \param[in]  dac_periph
      \arg        DACx(x=0,1)
    \param[out] none
    \retval     none
*/
void dac_dma_enable(uint32_t dac_periph)
{
    if(DAC0 == dac_periph){
        DAC_CTL |= DAC_CTL_DDMAEN0;
    }else{
        DAC_CTL |= DAC_CTL_DDMAEN1;
    }
}

/*!
    \brief      disable DAC DMA function
    \param[in]  dac_periph
      \arg        DACx(x=0,1)
    \param[out] none
    \retval     none
*/
void dac_dma_disable(uint32_t dac_periph)
{
    if(DAC0 == dac_periph){
        DAC_CTL &= ~DAC_CTL_DDMAEN0;
    }else{
        DAC_CTL &= ~DAC_CTL_DDMAEN1;
    }
}

/*!
    \brief      enable DAC output buffer
    \param[in]  dac_periph
      \arg        DACx(x =0,1)
    \param[out] none
    \retval     none
*/
void dac_output_buffer_enable(uint32_t dac_periph)
{
    if(DAC0 == dac_periph){
        DAC_CTL &= ~DAC_CTL_DBOFF0;
    }else{
        DAC_CTL &= ~DAC_CTL_DBOFF1;
    }
}

/*!
    \brief      disable DAC output buffer
    \param[in]  dac_periph
      \arg        DACx(x =0,1)
    \param[out] none
    \retval     none
*/
void dac_output_buffer_disable(uint32_t dac_periph)
{
    if(DAC0 == dac_periph){
        DAC_CTL |= DAC_CTL_DBOFF0;
    }else{
        DAC_CTL |= DAC_CTL_DBOFF1;
    }
}

/*!
    \brief      enable DAC trigger
    \param[in]  dac_periph
      \arg        DACx(x =0,1)
    \param[out] none
    \retval     none
*/
void dac_trigger_enable(uint32_t dac_periph)
{
    if(DAC0 == dac_periph){
        DAC_CTL |= DAC_CTL_DTEN0;
    }else{
        DAC_CTL |= DAC_CTL_DTEN1;
    }
}

/*!
    \brief      disable DAC trigger
    \param[in]  dac_periph
      \arg        DACx(x =0,1)
    \param[out] none
    \retval     none
*/
void dac_trigger_disable(uint32_t dac_periph)
{
    if(DAC0 == dac_periph){
        DAC_CTL &= ~DAC_CTL_DTEN0;
    }else{
        DAC_CTL &= ~DAC_CTL_DTEN1;
    }
}

/*!
    \brief      enable DAC software trigger
    \param[in]  dac_periph
      \arg        DACx(x =0,1)
    \retval     none
*/
void dac_software_trigger_enable(uint32_t dac_periph)
{
    if(DAC0 == dac_periph){
        DAC_SWT |= DAC_SWT_SWTR0;
    }else{
        DAC_SWT |= DAC_SWT_SWTR1;
    }
}

/*!
    \brief      disable DAC software trigger
    \param[in]  dac_periph
      \arg        DACx(x =0,1)
    \param[out] none
    \retval     none
*/
void dac_software_trigger_disable(uint32_t dac_periph)
{
    if(DAC0 == dac_periph){
        DAC_SWT &= ~DAC_SWT_SWTR0;
    }else{
        DAC_SWT &= ~DAC_SWT_SWTR1;
    }
}

/*!
    \brief      set DAC trigger source
    \param[in]  dac_periph
      \arg        DACx(x =0,1)
    \param[in]  triggersource: external triggers of DAC
      \arg        DAC_TRIGGER_T1_TRGO: TIMER1 TRGO
      \arg        DAC_TRIGGER_T2_TRGO: TIMER2 TRGO (for GD32F30X_CL)
      \arg        DAC_TRIGGER_T3_TRGO: TIMER3 TRGO
      \arg        DAC_TRIGGER_T4_TRGO: TIMER4 TRGO
      \arg        DAC_TRIGGER_T5_TRGO: TIMER5 TRGO
      \arg        DAC_TRIGGER_T6_TRGO: TIMER6 TRGO
      \arg        DAC_TRIGGER_T7_TRGO: TIMER7 TRGO (for GD32F30X_HD and GD32F30X_XD)
      \arg        DAC_TRIGGER_EXTI_9: EXTI interrupt line9 event
      \arg        DAC_TRIGGER_SOFTWARE: software trigger
    \param[out] none
    \retval     none
*/
void dac_trigger_source_config(uint32_t dac_periph,uint32_t triggersource)
{
    if(DAC0 == dac_periph){
        DAC_CTL &= ~DAC_CTL_DTSEL0;
        DAC_CTL |= triggersource;
    }else{
        DAC_CTL &= ~DAC_CTL_DTSEL1;
        DAC_CTL |= (triggersource << 16);
    }
}

/*!
    \brief      configure DAC wave mode
    \param[in]  dac_periph
      \arg        DACx(x=0,1)
    \param[in]  wave_mode
      \arg        DAC_WAVE_DISABLE: wave disable
      \arg        DAC_WAVE_MODE_LFSR: LFSR noise mode
      \arg        DAC_WAVE_MODE_TRIANGLE: triangle noise mode
    \param[out] none
    \retval     none
*/
void dac_wave_mode_config(uint32_t dac_periph, uint32_t wave_mode)
{
    if(DAC0 == dac_periph){
        DAC_CTL &= ~DAC_CTL_DWM0;
        DAC_CTL |= wave_mode;
    }else{
        DAC_CTL &= ~DAC_CTL_DWM1;
        DAC_CTL |= wave_mode << 16;
    }
}

/*!
    \brief      configure DAC wave bit width
    \param[in]  dac_periph
      \arg        DACx(x=0,1)
    \param[in]  bit_width
      \arg        DAC_WAVE_BIT_WIDTH_1: bit width of the wave signal is 1
      \arg        DAC_WAVE_BIT_WIDTH_2: bit width of the wave signal is 2
      \arg        DAC_WAVE_BIT_WIDTH_3: bit width of the wave signal is 3
      \arg        DAC_WAVE_BIT_WIDTH_4: bit width of the wave signal is 4
      \arg        DAC_WAVE_BIT_WIDTH_5: bit width of the wave signal is 5
      \arg        DAC_WAVE_BIT_WIDTH_6: bit width of the wave signal is 6
      \arg        DAC_WAVE_BIT_WIDTH_7: bit width of the wave signal is 7
      \arg        DAC_WAVE_BIT_WIDTH_8: bit width of the wave signal is 8
      \arg        DAC_WAVE_BIT_WIDTH_9: bit width of the wave signal is 9
      \arg        DAC_WAVE_BIT_WIDTH_10: bit width of the wave signal is 10
      \arg        DAC_WAVE_BIT_WIDTH_11: bit width of the wave signal is 11
      \arg        DAC_WAVE_BIT_WIDTH_12: bit width of the wave signal is 12
    \param[out] none
    \retval     none
*/
void dac_wave_bit_width_config(uint32_t dac_periph, uint32_t bit_width)
{
    if(DAC0 == dac_periph){
        DAC_CTL &= ~DAC_CTL_DWBW0;
        DAC_CTL |= bit_width;
    }else{
        DAC_CTL &= ~DAC_CTL_DWBW1;
        DAC_CTL |= bit_width << 16;
    }
}

/*!
    \brief      configure DAC LFSR noise mode
    \param[in]  dac_periph
      \arg        DACx(x=0,1)
    \param[in]  unmask_bits
      \arg        DAC_LFSR_BIT0: unmask the LFSR bit0
      \arg        DAC_LFSR_BITS1_0: unmask the LFSR bits[1:0]
      \arg        DAC_LFSR_BITS2_0: unmask the LFSR bits[2:0]
      \arg        DAC_LFSR_BITS3_0: unmask the LFSR bits[3:0]
      \arg        DAC_LFSR_BITS4_0: unmask the LFSR bits[4:0]
      \arg        DAC_LFSR_BITS5_0: unmask the LFSR bits[5:0]
      \arg        DAC_LFSR_BITS6_0: unmask the LFSR bits[6:0]
      \arg        DAC_LFSR_BITS7_0: unmask the LFSR bits[7:0]
      \arg        DAC_LFSR_BITS8_0: unmask the LFSR bits[8:0]
      \arg        DAC_LFSR_BITS9_0: unmask the LFSR bits[9:0]
      \arg        DAC_LFSR_BITS10_0: unmask the LFSR bits[10:0]
      \arg        DAC_LFSR_BITS11_0: unmask the LFSR bits[11:0]
    \param[out] none
    \retval     none
*/
void dac_lfsr_noise_config(uint32_t dac_periph, uint32_t unmask_bits)
{
    if(DAC0 == dac_periph){
        DAC_CTL &= ~DAC_CTL_DWBW0;
        DAC_CTL |= unmask_bits;
    }else{
        DAC_CTL &= ~DAC_CTL_DWBW1;
        DAC_CTL |= unmask_bits << 16;
    }
}

/*!
    \brief      configure DAC triangle noise mode
    \param[in]  dac_periph
      \arg        DACx(x=0,1)
    \param[in]  amplitude
      \arg        DAC_TRIANGLE_AMPLITUDE_1: triangle amplitude is 1
      \arg        DAC_TRIANGLE_AMPLITUDE_3: triangle amplitude is 3
      \arg        DAC_TRIANGLE_AMPLITUDE_7: triangle amplitude is 7
      \arg        DAC_TRIANGLE_AMPLITUDE_15: triangle amplitude is 15
      \arg        DAC_TRIANGLE_AMPLITUDE_31: triangle amplitude is 31
      \arg        DAC_TRIANGLE_AMPLITUDE_63: triangle amplitude is 63
      \arg        DAC_TRIANGLE_AMPLITUDE_127: triangle amplitude is 127
      \arg        DAC_TRIANGLE_AMPLITUDE_255: triangle amplitude is 255
      \arg        DAC_TRIANGLE_AMPLITUDE_511: triangle amplitude is 511
      \arg        DAC_TRIANGLE_AMPLITUDE_1023: triangle amplitude is 1023
      \arg        DAC_TRIANGLE_AMPLITUDE_2047: triangle amplitude is 2047
      \arg        DAC_TRIANGLE_AMPLITUDE_4095: triangle amplitude is 4095
    \param[out] none
    \retval     none
*/
void dac_triangle_noise_config(uint32_t dac_periph, uint32_t amplitude)
{
    if(DAC0 == dac_periph){
        DAC_CTL &= ~DAC_CTL_DWBW0;
        DAC_CTL |= amplitude;
    }else{
        DAC_CTL &= ~DAC_CTL_DWBW1;
        DAC_CTL |= amplitude << 16;
    }
}

/*!
    \brief      get DAC output value
    \param[in]  dac_periph
      \arg        DACx(x=0,1)
    \param[out] none
    \retval     DAC output data
*/
uint16_t dac_output_value_get(uint32_t dac_periph)
{
    uint16_t data = 0U;
    if(DAC0 == dac_periph){
        data = (uint16_t)DAC0_DO;
    }else{
        data = (uint16_t)DAC1_DO;
    }
    return data;
}

/*!
    \brief      enable DAC concurrent mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_concurrent_enable(void)
{
    uint32_t ctl = 0U;
    ctl = DAC_CTL_DEN0 | DAC_CTL_DEN1;
    DAC_CTL |= (ctl);
}

/*!
    \brief      disable DAC concurrent mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_concurrent_disable(void)
{
    uint32_t ctl = 0U;
    ctl = DAC_CTL_DEN0 | DAC_CTL_DEN1;
    DAC_CTL &= (~ctl);
}

/*!
    \brief      enable DAC concurrent software trigger function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_concurrent_software_trigger_enable(void)
{
    uint32_t swt = 0U;
    swt = DAC_SWT_SWTR0 | DAC_SWT_SWTR1;
    DAC_SWT |= (swt); 
}

/*!
    \brief      disable DAC concurrent software trigger function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_concurrent_software_trigger_disable(void)
{
    uint32_t swt = 0U;
    swt = DAC_SWT_SWTR0 | DAC_SWT_SWTR1;
    DAC_SWT &= (~swt);
}

/*!
    \brief      enable DAC concurrent buffer function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_concurrent_output_buffer_enable(void)
{
    uint32_t ctl = 0U;
    ctl = DAC_CTL_DBOFF0 | DAC_CTL_DBOFF1;
    DAC_CTL &= (~ctl);
}

/*!
    \brief      disable DAC concurrent buffer function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_concurrent_output_buffer_disable(void)
{
    uint32_t ctl = 0U;
    ctl = DAC_CTL_DBOFF0 | DAC_CTL_DBOFF1;
    DAC_CTL |= (ctl);
}

/*!
    \brief      set the DAC specified data holding register value
    \param[in]  dac_periph
      \arg        DACx(x=0,1)
    \param[in]  dac_align
      \arg        DAC_ALIGN_8B_R: data right 8b alignment
      \arg        DAC_ALIGN_12B_R: data right 12b alignment
      \arg        DAC_ALIGN_12B_L: data left 12b alignment
    \param[in]  data: data to be loaded
    \param[out] none
    \retval     none
*/
void dac_data_set(uint32_t dac_periph, uint32_t dac_align, uint16_t data)
{
    if(DAC0 == dac_periph){
        switch(dac_align){
        /* data right 12b alignment */
        case DAC_ALIGN_12B_R:
            DAC0_R12DH = data;
            break;
        /* data left 12b alignment */
        case DAC_ALIGN_12B_L:
            DAC0_L12DH = data;
            break;
        /* data right 8b alignment */
        case DAC_ALIGN_8B_R:
            DAC0_R8DH = data;
            break;
        default:
            break;
        }
    }else{
        switch(dac_align){
        /* data right 12b alignment */
        case DAC_ALIGN_12B_R:
            DAC1_R12DH = data;
            break;
        /* data left 12b alignment */
        case DAC_ALIGN_12B_L:
            DAC1_L12DH = data;
            break;
        /* data right 8b alignment */
        case DAC_ALIGN_8B_R:
            DAC1_R8DH = data;
            break;
        default:
            break;
        }
    }
}

/*!
    \brief      set DAC concurrent mode data holding register value
    \param[in]  dac_align
      \arg        DAC_ALIGN_8B_R: data right 8b alignment
      \arg        DAC_ALIGN_12B_R: data right 12b alignment
      \arg        DAC_ALIGN_12B_L: data left 12b alignment
    \param[in]  data0: data to be loaded
    \param[in]  data1: data to be loaded
    \param[out] none
    \retval     none
*/
void dac_concurrent_data_set(uint32_t dac_align, uint16_t data0, uint16_t data1)
{
    uint32_t data = 0U;
    switch(dac_align){
    /* data right 12b alignment */
    case DAC_ALIGN_12B_R:
        data = ((uint32_t)data1 << 16) | data0;
        DACC_R12DH = data;
        break;
    /* data left 12b alignment */
    case DAC_ALIGN_12B_L:
        data = ((uint32_t)data1 << 16) | data0;
        DACC_L12DH = data;
        break;
    /* data right 8b alignment */
    case DAC_ALIGN_8B_R:
        data = ((uint32_t)data1 << 8) | data0;
        DACC_R8DH = data;
        break;
    default:
        break;
    }
}

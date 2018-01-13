
#include "platform.h"
#include "platform_peripheral.h"
#include "platform_logging.h"

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Variables Definitions
 ******************************************************/
static const uint16_t adc_sampling_cycle[] =
{
    [ADC_SampleTime_3Cycles  ] = 3,
    [ADC_SampleTime_15Cycles ] = 15,
    [ADC_SampleTime_28Cycles ] = 28,
    [ADC_SampleTime_56Cycles ] = 56,
    [ADC_SampleTime_84Cycles ] = 84,
    [ADC_SampleTime_112Cycles] = 112,
    [ADC_SampleTime_144Cycles] = 144,
    [ADC_SampleTime_480Cycles] = 480,
};

/******************************************************
 *               Function Declarations
 ******************************************************/


/******************************************************
 *               Function Definitions
 ******************************************************/


OSStatus platform_adc_init( const platform_adc_t* adc, uint32_t sample_cycle )
{
    GPIO_InitTypeDef      gpio_init_structure;
    ADC_InitTypeDef       adc_init_structure;
    ADC_CommonInitTypeDef adc_common_init_structure;
    uint8_t     a;
    OSStatus    err = kNoErr;

    platform_mcu_powersave_disable();

    require_action_quiet( adc != NULL, exit, err = kParamErr);
    
    /* Enable peripheral clock for this port */
    err = platform_gpio_enable_clock( adc->pin );
    require_noerr(err, exit);

    /* Initialize the associated GPIO */
    gpio_init_structure.GPIO_Pin   = (uint32_t)( 1 << adc->pin->pin_number );;
    gpio_init_structure.GPIO_Speed = (GPIOSpeed_TypeDef) 0;
    gpio_init_structure.GPIO_Mode  = GPIO_Mode_AN;
    gpio_init_structure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    gpio_init_structure.GPIO_OType = GPIO_OType_OD;
    GPIO_Init( adc->pin->port, &gpio_init_structure );

    RCC_APB2PeriphClockCmd( adc->adc_peripheral_clock, ENABLE );

    /* Initialize the ADC */
    ADC_StructInit( &adc_init_structure );
    adc_init_structure.ADC_Resolution         = ADC_Resolution_12b;
    adc_init_structure.ADC_ScanConvMode       = DISABLE;
    adc_init_structure.ADC_ContinuousConvMode = DISABLE;
    adc_init_structure.ADC_ExternalTrigConv   = ADC_ExternalTrigConvEdge_None;
    adc_init_structure.ADC_DataAlign          = ADC_DataAlign_Right;
    adc_init_structure.ADC_NbrOfConversion    = 1;
    ADC_Init( adc->port, &adc_init_structure );

    ADC_CommonStructInit( &adc_common_init_structure );
    adc_common_init_structure.ADC_Mode             = ADC_Mode_Independent;
    adc_common_init_structure.ADC_DMAAccessMode    = ADC_DMAAccessMode_Disabled;
    adc_common_init_structure.ADC_Prescaler        = ADC_Prescaler_Div2;
    adc_common_init_structure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInit( &adc_common_init_structure );

    ADC_Cmd( adc->port, ENABLE );

    /* Find the closest supported sampling time by the MCU */
    for ( a = 0; ( a < sizeof( adc_sampling_cycle ) / sizeof(uint16_t) ) && adc_sampling_cycle[a] < sample_cycle; a++ )
    {
    }

    /* Initialize the ADC channel */
    ADC_RegularChannelConfig( adc->port, adc->channel, adc->rank, a );

exit:
    platform_mcu_powersave_enable();
    return err;
}

OSStatus platform_adc_take_sample( const platform_adc_t* adc, uint16_t* output )
{
    OSStatus    err = kNoErr;

    platform_mcu_powersave_disable();

    require_action_quiet( adc != NULL, exit, err = kParamErr);

    /* Start conversion */
    ADC_SoftwareStartConv( adc->port );

    /* Wait until end of conversion */
    while ( ADC_GetFlagStatus( adc->port, ADC_FLAG_EOC ) == RESET )
    {
    }

    /* Read ADC conversion result */
    *output = ADC_GetConversionValue( adc->port );

exit:
    platform_mcu_powersave_enable();
    return err;
}

OSStatus platform_adc_take_sample_stream( const platform_adc_t* adc, void* buffer, uint16_t buffer_length )
{
    UNUSED_PARAMETER(adc);
    UNUSED_PARAMETER(buffer);
    UNUSED_PARAMETER(buffer_length);
    platform_log("unimplemented");
    return kNotPreparedErr;
}

OSStatus platform_adc_deinit( const platform_adc_t* adc )
{
    UNUSED_PARAMETER(adc);
    platform_log("unimplemented");
    return kNotPreparedErr;
}



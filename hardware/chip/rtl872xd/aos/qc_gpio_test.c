#include "aos/hal/gpio.h"
#include "gpio_test.h"

/******************************************************
*               Variables Definitions
******************************************************/

extern const qc_test_gpio_pair_t qc_test_gpio_pairs[];
extern const int qc_test_gpio_pairs_num;

static int gpio_result = 0;

/******************************************************
*               Function Definitions
******************************************************/

static int _gpio_test_one( const qc_test_gpio_pair_t* gpio_test_pair )
{
    gpio_dev_t  gpio_out;
    gpio_dev_t  gpio_in;
    int ret=-1, val;
    
    gpio_out.port = gpio_test_pair->output_pin;
    gpio_out.config = OUTPUT_PUSH_PULL;
    gpio_in.port = gpio_test_pair->input_pin;
    gpio_in.config = INPUT_PULL_DOWN;
    hal_gpio_init(&gpio_out);
    hal_gpio_init(&gpio_in);
    hal_gpio_output_high(&gpio_out);
    aos_msleep(1);
    hal_gpio_input_get(&gpio_in, &val);
    if (val != 1)
        goto EXIT;

    gpio_in.config = INPUT_PULL_UP;
    hal_gpio_init(&gpio_in);
    hal_gpio_output_low(&gpio_out);
    aos_msleep(1);
    hal_gpio_input_get(&gpio_in, &val);
    if (val != 0)
        goto EXIT;

    ret = 0;

EXIT:
    hal_gpio_deinit(&gpio_out);
    hal_gpio_deinit(&gpio_in);
    return ret;
}

static int _gpio_test( const qc_test_gpio_pair_t* gpio_test_pair, int num )
{
    int i;
    int err = 0;
    qc_test_gpio_pair_t * gpio_test = (qc_test_gpio_pair_t *)gpio_test_pair;
    
    gpio_result = 0;
    
    for ( i = 0; i < num; i++ )
    {
        if ( 0 == _gpio_test_one( &gpio_test_pair[i] ) )
        {
            gpio_result |= (1 << i);
        }
        else
        {
            err = -1;
        }
    }
    
    return err;
}

void qc_test_gpio( void )
{
    char test_result[32];
    int gpio_num = 0;

    if (qc_test_gpio_pairs_num == 0)
        return;
    
    if ( 0 == _gpio_test( qc_test_gpio_pairs, qc_test_gpio_pairs_num ) )
    {
        qc_printf( "GPIO TEST: PASS\r\n" );
    }
    else
    {
        sprintf( test_result, "Fail: %d:%x", gpio_num, gpio_result );
        qc_printf( "GPIO TEST: %s\r\n", test_result );
    }

    return;
}


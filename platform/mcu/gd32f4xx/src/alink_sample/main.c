#include "gd32f4xx.h"
#include "gd32f450z_eval.h"


void soc_init(void);
/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{  
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    soc_init();

    while(1){

    }
}

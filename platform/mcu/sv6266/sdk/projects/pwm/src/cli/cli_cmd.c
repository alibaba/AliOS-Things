//#define __SFILE__ "cli_cmd.c"
#include <stdio.h>
#include "soc_types.h"
#include "cli.h"

#include "drv/gpio/hal_gpio.h"
#include "drv/gpio/drv_gpio.h"
#include "drv/pwm/hal_pwm.h"
#include "drv/pwm/drv_pwm.h"

uint32_t pwm_gpio_int_pin = 5;

/*---------------------------------- CMDs -----------------------------------*/

void Cmd_test_pwm(s32 argc, char *argv[]) {    

	int8_t ret = 0x0;
	uint32_t pwm_id = 0;
	uint32_t freq_hz = 0;
	uint32_t duty = 0;
	uint32_t is_invert = 0;

	pwm_id = strtoul(argv[1], NULL, 10);
	freq_hz = strtoul(argv[2], NULL, 10);
	duty = strtoul(argv[3], NULL, 10);
	is_invert = strtoul(argv[4], NULL, 10);

	if ((argc != 5) || (pwm_id < 0) || (pwm_id > 4) || (freq_hz < 5) || (freq_hz > 4000000) || (duty < 0) || (duty > M_PWM_DUTY_STEP) || (is_invert < 0) || (is_invert > 1)) {
		printf("Usage	: test_pwm <id> <freq> <duty> <inv>\n");
		printf("<id>	: pwm id(0-4)\n");
		printf("<freq>	: freq_hz(5-4M)\n");
		printf("<duty>	: duty(0-4096)\n");
		printf("<inv>	: 0:none ; 1:invert the PWM polarity\n");
		return;
	}

	ret = drv_pwm_init(pwm_id);
	ret = drv_pwm_config(pwm_id, freq_hz, duty, is_invert);
	ret = drv_pwm_enable(pwm_id);

	ret = drv_gpio_set_mode(pwm_gpio_int_pin, PIN_MODE_GPIO);
	ret = drv_gpio_set_dir(pwm_gpio_int_pin, GPIO_DIR_OUT);
	ret = drv_gpio_set_logic(pwm_gpio_int_pin, GPIO_LOGIC_HIGH);

	printf("pwm_id		= %d\n", pwm_id);
	printf("freq_hz 	= %d\n", freq_hz);
	printf("duty		= %d\n", duty);
	printf("is_invert	= %d\n", is_invert);

	ret = drv_gpio_set_logic(pwm_gpio_int_pin, GPIO_LOGIC_LOW);
}

void Cmd_test_pwm_reconfig(s32 argc, char *argv[]) {    

    int8_t ret = 0x0;
    uint32_t pwm_id = 0;
    uint32_t freq_hz = 0;
    uint32_t duty = 0;
    uint32_t is_invert = 0;
    
	
	pwm_id = strtoul(argv[1], NULL, 10);
    freq_hz = strtoul(argv[2], NULL, 10);
    duty = strtoul(argv[3], NULL, 10);
    is_invert = strtoul(argv[4], NULL, 10);

    if ((argc != 5) || (pwm_id < 0) || (pwm_id > 4) || (freq_hz < 5) || (freq_hz > 4000000) || (duty < 0) || (duty > M_PWM_DUTY_STEP) || (is_invert < 0) || (is_invert > 1)) {
        printf("Usage   : test_pwm_reconfig <id> <freq> <duty> <inv>\n");
        printf("<id>    : pwm id(0-4)\n");
        printf("<freq>  : freq_hz(5-4M)\n");
        printf("<duty>  : duty(0-4096)\n");
        printf("<inv>   : 0:none ; 1:invert the PWM polarity\n");
        return;
    }
    
    ret = drv_pwm_config(pwm_id, freq_hz, duty, is_invert);

    ret = drv_gpio_set_mode(pwm_gpio_int_pin, PIN_MODE_GPIO);
    ret = drv_gpio_set_dir(pwm_gpio_int_pin, GPIO_DIR_OUT);
    ret = drv_gpio_set_logic(pwm_gpio_int_pin, GPIO_LOGIC_HIGH);
    
    printf("pwm_id      = %d\n", pwm_id);
    printf("freq_hz     = %d\n", freq_hz);
    printf("duty        = %d\n", duty);
    printf("is_invert   = %d\n", is_invert);

    ret = drv_gpio_set_logic(pwm_gpio_int_pin, GPIO_LOGIC_LOW);
    
}

void Cmd_test_pwm_disable(s32 argc, char *argv[]) {    

    int8_t ret = 0x0;
    uint32_t pwm_id = 0;
    
	pwm_id = strtoul(argv[1], NULL, 10);    

    if ((argc != 2) || (pwm_id < 0) || (pwm_id > 4)) {
        printf("Usage   : test_pwm_disable <id> <freq> <duty> <inv>\n");
        printf("<id>    : pwm id(0-4)\n");
        return;
    }
    
    ret = drv_pwm_disable(pwm_id);

    ret = drv_gpio_set_mode(pwm_gpio_int_pin, PIN_MODE_GPIO);
    ret = drv_gpio_set_dir(pwm_gpio_int_pin, GPIO_DIR_OUT);
    ret = drv_gpio_set_logic(pwm_gpio_int_pin, GPIO_LOGIC_HIGH);
    
    printf("pwm_id      = %d\n", pwm_id);

    ret = drv_gpio_set_logic(pwm_gpio_int_pin, GPIO_LOGIC_LOW);
    
}

void Cmd_test_pwm_enable(s32 argc, char *argv[]) {    

    int8_t ret = 0x0;
    uint32_t pwm_id = 0;
    
	pwm_id = strtoul(argv[1], NULL, 10);    

    if ((argc != 2) || (pwm_id < 0) || (pwm_id > 4)) {
        printf("Usage   : test_pwm_enable <id> <freq> <duty> <inv>\n");
        printf("<id>    : pwm id(0-4)\n");
        return;
    }
    
    ret = drv_pwm_enable(pwm_id);

    ret = drv_gpio_set_mode(pwm_gpio_int_pin, PIN_MODE_GPIO);
    ret = drv_gpio_set_dir(pwm_gpio_int_pin, GPIO_DIR_OUT);
    ret = drv_gpio_set_logic(pwm_gpio_int_pin, GPIO_LOGIC_HIGH);
    
    printf("pwm_id      = %d\n", pwm_id);
    
    ret = drv_gpio_set_logic(pwm_gpio_int_pin, GPIO_LOGIC_LOW);
    
}

static void Cmd_help (int32_t argc, char *argv[])
{
    printf ("\n*************************************************************************\n");
    printf ("Test PWM Target : \n");
    printf ("                   GPIO_00 : PWM_id: 0\n");
    printf ("                   GPIO_01 : PWM_id: 1\n");
    printf ("                   GPIO_02 : PWM_id: 2\n");
    printf ("                   GPIO_03 : PWM_id: 3\n");
    printf ("                   GPIO_04 : PWM_id: 4\n");
    
    printf ("\n*************************************************************************\n");
    printf ("Usage: \n");
    printf ("       set_pwm_config  <id> <freq> <duty> <inv>\n");
    printf ("brief:\n");
    printf ("       This function initializes the PWM setting.\n");
    printf ("Options:\n");
    printf ("<id>                       The PWM target number(0-4).\n");
    printf ("<freq>                     Freq_hz(5-4000000).\n");
    printf ("<duty>                     Duty cycle(0-4096).\n");
    printf ("<inv>                      PWM polarity. 0:none ; 1:invert\n");

    printf ("\n*************************************************************************\n");
    printf ("Usage: \n");
    printf ("       set_pwm_reconfig  <id> <freq> <duty> <inv>\n");
    printf ("brief:\n");
    printf ("       This function will reconfig the PWM setting.\n");
    printf ("Options:\n");
    printf ("<id>                       The PWM target number(0-4).\n");
    printf ("<freq>                     Freq_hz(5-4000000).\n");
    printf ("<duty>                     Duty cycle(0-4096).\n");
    printf ("<inv>                      PWM polarity. 0:none ; 1:invert\n");

    printf ("\n*************************************************************************\n");
    printf ("Usage: \n");
    printf ("       pwm_disable  <id>\n");
    printf ("brief:\n");
    printf ("       This function stops the PWM execution.\n");
    printf ("Options:\n");
    printf ("<id>                       The PWM target number(0-4).\n");

    printf ("\n*************************************************************************\n");
    printf ("Usage: \n");
    printf ("       pwm_enable  <id>\n");
    printf ("brief:\n");
    printf ("       This function starts the PWM execution.\n");
    printf ("Options:\n");
    printf ("<id>                       The PWM target number(0-4).\n");
    
}



/* ---------------------- Registered CMDs to CMD Table ---------------------- */
const CLICmds gCliCmdTable[] = {
    { "help",                   Cmd_help,               "pwm test help"                                 },
    { "set_pwm_config",         Cmd_test_pwm,           "set_pwm_config <id> <freq> <duty> <inv>"       },
    { "set_pwm_reconfig",       Cmd_test_pwm_reconfig,  "set_pwm_reconfig <id> <freq> <duty> <inv>"     },
    { "pwm_disable",            Cmd_test_pwm_disable,   "pwm_disable <id>"                              },
    { "pwm_enable",             Cmd_test_pwm_enable,    "pwm_enable <id>"                               },
    
    /*lint -save -e611 */
    { (const char *)NULL, (CliCmdFunc)NULL,   (const char *)NULL },
    /*lint -restore */
};



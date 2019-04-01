//#define __SFILE__ "cli_cmd.c"
#include <stdio.h>
#include "soc_types.h"
#include "cli.h"

#include "drv/gpio/hal_gpio.h"
#include "drv/gpio/drv_gpio.h"
#include "drv/tmr/drv_tmr.h"
#include "drv/tmr/hal_tm.h"

uint8_t gpio_intc_id = 0x0;
uint8_t gpio_intc_mode = 0x0;
/*---------------------------------- CMDs -----------------------------------*/

static void Cmd_gpio_common_usage(const char *cmd) {
    printf("Usage   : %s <id>\n", cmd);
    printf("<id>    : gpio id(0-22)\n");
}

void Cmd_set_gpio_mode(s32 argc, char *argv[]) {

    int8_t ret = 0x0;
    uint32_t gpio_id = 0;
    uint32_t gpio_logic = 0;

    if (argc != 2) {
        Cmd_gpio_common_usage(argv[0]);
        return;
    }

    gpio_id = strtoul(argv[1], NULL, 10);

    if (gpio_id > 22) {
        Cmd_gpio_common_usage(argv[0]);
        return;
    }

    ret = drv_gpio_set_mode(gpio_id, PIN_MODE_GPIO);

    printf("gpio_id = %d, set to gpio with no input & not output\n", gpio_id);
}

static void Cmd_set_gpio_tri_state_usage() {
    printf("Usage   : set_gpio_tri_state <id>\n");
    printf("<id>    : gpio id(0-22)\n");
}

void Cmd_set_gpio_tri_state(s32 argc, char *argv[]) {

    int8_t ret = 0x0;
    uint32_t gpio_id = 0;
    uint32_t gpio_logic = 0;
	
    if (argc != 2) {
        Cmd_set_gpio_tri_state_usage();
        return;
    }

    gpio_id = strtoul(argv[1], NULL, 10);

    if (gpio_id > 22) {
        Cmd_set_gpio_tri_state_usage();
        return;
    }

    ret = drv_gpio_set_mode(gpio_id, PIN_MODE_ZSTATE);

    printf("gpio_id = %d, set zstate\n", gpio_id);
}

static void Cmd_set_gpio_direction_usage(const char *cmd) {
    printf("Usage   : %s <id> <input> <output>\n", cmd);
    printf("<id>    : gpio id(0-22)\n");
    printf("<input> : 0:disable ; 1:enable\n");
    printf("<output> : 0:disable ; 1:enable\n");
}

void Cmd_set_gpio_direction(s32 argc, char *argv[]) {

    uint32_t gpio_id = 0;
    uint32_t gpio_input = 0;
    uint32_t gpio_output = 0;

    if (argc != 4) {
        Cmd_set_gpio_direction_usage(argv[0]);
        return;
    }

	gpio_id = strtoul(argv[1], NULL, 10);
    gpio_input = strtoul(argv[2], NULL, 10);
    gpio_output = strtoul(argv[3], NULL, 10);

    if ((gpio_id > 22) || (gpio_input > 1) || (gpio_output > 1)) {
        Cmd_set_gpio_direction_usage(argv[0]);
        return;
    }

    //drv_gpio_set_mode(gpio_id, PIN_MODE_GPIO);
    if (gpio_input) {
        if (gpio_output) {
            // in & out
            drv_gpio_set_dir(gpio_id, GPIO_DIR_IN_OUT);
        } else {
            // in
            drv_gpio_set_dir(gpio_id, GPIO_DIR_IN);
        }
    } else {
        if (gpio_output) {
            // out
            drv_gpio_set_dir(gpio_id, GPIO_DIR_OUT);
        } else {
            // none
            drv_gpio_set_dir(gpio_id, GPIO_DIR_IN_OUT_OFF);
        }
    }
    printf("gpio_id = %d, set input = %d, set output = %d\n", gpio_id, gpio_input, gpio_output);
}

static void Cmd_set_gpio_logic_usage() {
    printf("Usage   : set_gpio_logic <id> <logic>\n");
    printf("<id>    : gpio id(0-22)\n");
    printf("<logic> : 0:low ; 1:high\n");
}

void Cmd_set_gpio_logic(s32 argc, char *argv[]) {

    int8_t ret = 0x0;
    uint32_t gpio_id = 0;
    uint32_t gpio_logic = 0;
	

    if (argc != 3) {
        Cmd_set_gpio_logic_usage();
        return;
    }

	gpio_id = strtoul(argv[1], NULL, 10);
    gpio_logic = strtoul(argv[2], NULL, 10);

    if ((gpio_id > 22) || (gpio_logic > 1)) {
        Cmd_set_gpio_logic_usage();
        return;
    }

    if ((drv_gpio_get_dir(gpio_id) != GPIO_DIR_OUT) && (drv_gpio_get_dir(gpio_id) != GPIO_DIR_IN_OUT)) {
        printf("gpio_id %d\n", gpio_id);
        printf("not set to direction output\n");
        return;
    }
    ret = drv_gpio_set_logic(gpio_id, gpio_logic);

    printf("gpio_id = %d\n", gpio_id);
    printf("set gpio_logic = %d\n", gpio_logic);
}

void Cmd_get_gpio_logic(s32 argc, char *argv[]) {

    int8_t ret = 0x0;
    uint32_t gpio_id = 0;
    uint32_t gpio_logic = 0;
	
    if (argc != 2) {
        printf("Usage   : get_gpio_logic <id>\n");
        printf("<id>    : gpio id(0-22)\n");
        return;
    }

	gpio_id = strtoul(argv[1], NULL, 10);

    if ((gpio_id > 22)) {
    }

    if ((drv_gpio_get_dir(gpio_id) != GPIO_DIR_IN) && (drv_gpio_get_dir(gpio_id) != GPIO_DIR_IN_OUT)) {
        printf("gpio_id %d\n", gpio_id);
        printf("not set to direction input\n");
        return;
    }
    gpio_logic = drv_gpio_get_logic(gpio_id);

    printf("gpio_id = %d\n", gpio_id);
    printf("gpio_logic = %d\n", gpio_logic);
}

void Cmd_set_gpio_pull_usage(const char *cmd) {
    printf("Usage   : %s <id> <pull>\n", cmd);
    printf("<id>    : gpio id(0-22)\n");
    printf("<pull>  : 0:down ; 1:up ; 2:none\n");
}

void Cmd_set_gpio_pull(s32 argc, char *argv[]) {
    
    uint32_t gpio_id = 0;
    uint32_t gpio_pull = 0;
	
    if (argc != 3) {
        Cmd_set_gpio_pull_usage(argv[0]);
        return;
    }

	gpio_id = strtoul(argv[1], NULL, 10);
    gpio_pull = strtoul(argv[2], NULL, 10);

    if ((gpio_id > 22) || (gpio_pull > 2)) {
        Cmd_set_gpio_pull_usage(argv[0]);
        return;
    }

    drv_gpio_set_pull(gpio_id, gpio_pull);

    printf("gpio_id = %d\n", gpio_id);
    printf("gpio_pull = %d\n", gpio_pull);
}

void Cmd_get_gpio_pull_logic(s32 argc, char *argv[]) {

    int8_t ret = 0x0;
    uint32_t gpio_id = 0;
    gpio_pull_t gpio_pull_logic = GPIO_PULL_NONE;
	
    if (argc != 2) {
        Cmd_gpio_common_usage(argv[0]);
        return;
    }

    gpio_id = strtoul(argv[1], NULL, 10);

    if (gpio_id > 22) {
        Cmd_gpio_common_usage(argv[0]);
        return;
    }

    gpio_pull_logic = drv_gpio_get_pull(gpio_id);

    printf("gpio_id = %d\n", gpio_id);
	if (gpio_pull_logic == GPIO_PULL_DOWN)
		printf("gpio_pull_logic = pull down\n");
	else if (gpio_pull_logic == GPIO_PULL_UP)
		printf("gpio_pull_logic = pull up\n");
	else if (gpio_pull_logic == GPIO_PULL_NONE)
		printf("gpio_pull_logic = none\n");

}


void Cmd_get_gpio_direction(s32 argc, char *argv[]) {

    int8_t ret = 0x0;
    uint32_t gpio_id = 0;
    gpio_dir_t gpio_direction = GPIO_DIR_IN_OUT_OFF;
	
    if (argc != 2) {
        Cmd_gpio_common_usage(argv[0]);
        return;
    }

    gpio_id = strtoul(argv[1], NULL, 10);

    if (gpio_id > 22) {
        Cmd_gpio_common_usage(argv[0]);
        return;
    }

    gpio_direction = drv_gpio_get_dir(gpio_id);

    printf("gpio_id = %d\n", gpio_id);
	if (gpio_direction == GPIO_DIR_IN)
		printf("gpio_dirction = input\n");
	else if (gpio_direction == GPIO_DIR_OUT)
		printf("gpio_dirction = output\n");
	else if (gpio_direction == GPIO_DIR_IN_OUT)
		printf("gpio_dirction = input and output\n");
	else if (gpio_direction == GPIO_DIR_IN_OUT_OFF)
		printf("gpio_dirction = input and output off\n");

}


void irq_test_gpio_ipc(void) {
    int8_t ret = 0x0;
    
    ret = drv_gpio_intc_clear(gpio_intc_id);
    
    if(GPIO_INTC_HIGH_LEVEL == gpio_intc_mode) {
        printf("\nLine = %d\nFunc = %s\nGPIO_INTC_HIGH_LEVEL\n", __LINE__, __FUNCTION__);
    }
    else if(GPIO_INTC_RISING_EDGE == gpio_intc_mode) {
        printf("\nLine = %d\nFunc = %s\nGPIO_INTC_RISING_EDGE\n", __LINE__, __FUNCTION__);
    }
    else if(GPIO_INTC_LOW_LEVEL == gpio_intc_mode) { 
        printf("\nLine = %d\nFunc = %s\nGPIO_INTC_LOW_LEVEL\n", __LINE__, __FUNCTION__);
    }
    else {
        printf("\nLine = %d\nFunc = %s\nGPIO_INTC_FALLING_EDGE\n", __LINE__, __FUNCTION__);
    }    
}

static void Cmd_set_gpio_int_usage(const char *cmd) {
    printf("Usage   : %s <id> <mode>\n", cmd);
    printf("<id>    : gpio id(0-22)\n");
    printf("<mode>  : 2:high_level ; 3:rising_edge ; 4:low_level ; 5:falling_edge\n");
}

void Cmd_set_gpio_int(s32 argc, char *argv[]) {    

    uint32_t gpio_id = 0;
    uint32_t int_mode = 0;
	
    if (argc != 3) {
        Cmd_set_gpio_int_usage(argv[0]);
        return;
    }

	gpio_id = strtoul(argv[1], NULL, 10);
    int_mode = strtoul(argv[2], NULL, 10);

    if ((gpio_id > 22) || (int_mode > 5) || (int_mode < 2)) {
        Cmd_set_gpio_int_usage(argv[0]);
        return;
    }

    gpio_intc_id = gpio_id;
    gpio_intc_mode = int_mode;

    if (drv_gpio_get_dir(gpio_id) == GPIO_DIR_OUT) {
        drv_gpio_set_dir(gpio_id, GPIO_DIR_IN_OUT);
    } else {
        drv_gpio_set_dir(gpio_id, GPIO_DIR_IN);
    }
    drv_gpio_intc_trigger_mode(gpio_id, int_mode);
    drv_gpio_register_isr(gpio_id, irq_test_gpio_ipc);

    printf("gpio_id = %d\n", gpio_id);
    printf("gpio_interrupt_mode = %d\n", int_mode);
}

static void Cmd_help (int32_t argc, char *argv[])
{
    printf ("\n*************************************************************************\n");
    printf ("Test GPIO Target : \n");
    printf ("                   GPIO_00 : GPIO_id: 0\n");
    printf ("                   GPIO_01 : GPIO_id: 1\n");
    printf ("                   GPIO_02 : GPIO_id: 2\n");
    printf ("                   GPIO_05 : GPIO_id: 5\n");
    printf ("                   GPIO_06 : GPIO_id: 6\n");
    printf ("                   SIO_07  : SIO_id : 7\n");
    printf ("                   GPIO_08 : GPIO_id: 8\n");
    printf ("                   GPIO_09 : GPIO_id: 9\n");
    printf ("                   GPIO_10 : GPIO_id: 10\n");
    printf ("                   GPIO_11 : GPIO_id: 11\n");
    printf ("                   GPIO_12 : GPIO_id: 12\n");
    printf ("                   GPIO_13 : GPIO_id: 13\n");
    printf ("                   SIO_20  : SIO_id : 20\n");
    printf ("                   GPIO_21 : GPIO_id: 21\n");
    printf ("                   GPIO_22 : GPIO_id: 22\n");

    printf ("\n*************************************************************************\n");
    printf ("Usage: \n");
    printf ("       set_gpio_logic  <id> <logic>\n");
    printf ("brief:\n");
    printf ("       This function sets the output logic of the target GPIO.\n");
    printf ("Options:\n");
    printf ("   <id>                       The GPIO target number(0-22).\n");
    printf ("   <logic>                    Output logic. 0:low ; 1:high.\n");

    printf ("\n*************************************************************************\n");
    printf ("Usage: \n");
    printf ("       get_gpio_logic  <id>\n");
    printf ("brief:\n");
    printf ("       This function gets the output logic of the target GPIO.\n");
    printf ("Options:\n");
    printf ("<id>                       The GPIO target number(0-22).\n");

    printf ("\n*************************************************************************\n");
    printf ("Usage: \n");
    printf ("       set_gpio_pull  <id> <pull>\n");
    printf ("brief:\n");
    printf ("       This function sets the target GPIO to the pull state.\n");
    printf ("Options:\n");
    printf ("   <id>                       The GPIO target number(0-22).\n");
    printf ("   <pull>                     Pull state. 0:down ; 1:up ; 2:none.\n");

    printf ("\n*************************************************************************\n");
    printf ("Usage: \n");
    printf ("       get_gpio_pull_logic  <id>\n");
    printf ("brief:\n");
    printf ("       This function gets the pull logic of the target GPIO.\n");
    printf ("Options:\n");
    printf ("<id>                       The GPIO target number(0-22).\n");

    printf ("\n*************************************************************************\n");
    printf ("Usage: \n");
    printf ("       set_gpio_tri_state  <id> \n");
    printf ("brief:\n");
    printf ("       This function sets the target GPIO to the tri-state.\n");
    printf ("Options:\n");
    printf ("   <id>                       The GPIO target number(0-22).\n");

    printf ("\n*************************************************************************\n");
    printf ("Usage: \n");
    printf ("       get_gpio_direction  <id>\n");
    printf ("brief:\n");
    printf ("       This function gets the direction of the target GPIO.\n");
    printf ("Options:\n");
    printf ("<id>                       The GPIO target number(0-22).\n");
    printf ("\n*************************************************************************\n");
    printf("Usage   : set_gpio_direction <id> <input> <output>\n");
    printf("<id>    : gpio id(0-22)\n");
    printf("<input> : 0:disable ; 1:enable\n");
    printf("<output> : 0:disable ; 1:enable\n");

    /*
    printf ("\n*************************************************************************\n");
    printf ("Usage: \n");
    printf ("       set_gpio_interrupt  <id> <mode>\n");
    printf ("brief:\n");
    printf ("       This function sets the target GPIO to the pull state.\n");
    printf ("Options:\n");
    printf (    "<id>                       The GPIO target number(0-22).\n");
    printf (    "<mode>                     Trigger mode. 2:high_level ; 3:rising_edge ; 4:low_level ; 5:falling_edge.\n");
    */
}


/* ---------------------- Registered CMDs to CMD Table ---------------------- */
const CLICmds gCliCmdTable[] = {
    { "help",                       Cmd_help,                       "gpio test help"                            },
    { "set_gpio_logic",             Cmd_set_gpio_logic,             "set_gpio_logic <id> <logic>"               },
    { "get_gpio_logic",             Cmd_get_gpio_logic,             "get_gpio_logic <id>"                       },
    { "set_gpio_pull",              Cmd_set_gpio_pull,              "set_gpio_pull <id> <pull>"                 },
    { "get_gpio_pull_logic",        Cmd_get_gpio_pull_logic,        "get_gpio_pull_logic <id>"                  },
    { "set_gpio_tri_state",         Cmd_set_gpio_tri_state,         "set_gpio_tri_state <id>"                   },
    { "set_gpio_mode",              Cmd_set_gpio_mode,              "set_gpio_mode <id>"                        },
	{ "get_gpio_direction",         Cmd_get_gpio_direction,		    "get_gpio_direction <id>"			        },
	{ "set_gpio_direction",         Cmd_set_gpio_direction,		    "get_set_direction <id> <input> <output>"   },
    //{ "set_gpio_interrupt",         Cmd_set_gpio_int,               "set_gpio_interrupt <id> <mode>"    },
    /*lint -save -e611 */
    { (const char *)NULL, (CliCmdFunc)NULL,   (const char *)NULL },
    /*lint -restore */
};



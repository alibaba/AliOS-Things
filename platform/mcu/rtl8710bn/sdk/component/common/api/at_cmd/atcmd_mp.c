#include <platform_stdlib.h>
#include <platform_opts.h>
#include <gpio_api.h>
#include "log_service.h"
#include "atcmd_mp.h"

#if CONFIG_ATCMD_MP_EXT0
	extern void fATM0(void *arg);	// MP ext0 AT command
#endif

#if CONFIG_ATCMD_MP
//-------- AT MP commands ---------------------------------------------------------------
void fATMG(void *arg)
{
    gpio_t gpio_test;
    int argc = 0, val, cnts, i, write=0, data=0;
	char *argv[MAX_ARGC] = {0}, port, num;
	PinName pin = NC;
	u32 tConfigDebugInfo = ConfigDebugInfo;
    
	AT_DBG_MSG(AT_FLAG_GPIO, AT_DBG_ALWAYS, "[ATMG]: _AT_MP_GPIO_TEST_");
	if(!arg){
		AT_DBG_MSG(AT_FLAG_GPIO, AT_DBG_ALWAYS, "[ATMG] Usage: ATSG=w,PINNAMES(ex:A0B1C2...),VALUE(0/1)");
		AT_DBG_MSG(AT_FLAG_GPIO, AT_DBG_ALWAYS, "[ATMG] Usage: ATSG=r,PINNAMES(ex:A0B1C2...)");
		return;
	}
	
	argc = parse_param(arg, argv);
	cnts = strlen(argv[2]);
	if(cnts % 2) return;
	cnts /= 2;
	if(cnts == 0) return;
	
	if(strcmp(argv[1], "w") == 0){
		write = 1;
		if(strcmp(argv[3], "1") == 0)
			data = 1;
	}
	// Remove debug info massage
	_AT_DBG_MSG(AT_FLAG_GPIO, AT_DBG_ALWAYS, "[ATMG] %s: ", argv[1]);
	ConfigDebugInfo = 0;
	for(i=0; i<(cnts*2); i+=2){
		pin = NC;
		port = argv[2][i];
		num = argv[2][i+1];
		if(port >= 'a' && port <= 'z')
			port -= ('a' - 'A');
		if(num >= 'a' && num <= 'z')
			num -= ('a' - 'A');
		switch(port){
			case 'A':
				switch(num){
					case '0': pin = PA_0; break; case '1': pin = PA_1; break; case '2': pin = PA_2; break; case '3': pin = PA_3; break;
					case '4': pin = PA_4; break; case '5': pin = PA_5; break; case '6': pin = PA_6; break; case '7': pin = PA_7; break;
				}
				break;
			case 'B':
				switch(num){
					case '0': pin = PB_0; break; case '1': pin = PB_1; break; case '2': pin = PB_2; break; case '3': pin = PB_3; break;
					case '4': pin = PB_4; break; case '5': pin = PB_5; break; case '6': pin = PB_6; break; case '7': pin = PB_7; break;
				}
				break;
			case 'C':
				switch(num){
					case '0': pin = PC_0; break; case '1': pin = PC_1; break; case '2': pin = PC_2; break; case '3': pin = PC_3; break;
					case '4': pin = PC_4; break; case '5': pin = PC_5; break; case '6': pin = PC_6; break; case '7': pin = PC_7; break;
					case '8': pin = PC_8; break; case '9': pin = PC_9; break;
				}
				break;
			case 'D':
				switch(num){
					case '0': pin = PD_0; break; case '1': pin = PD_1; break; case '2': pin = PD_2; break; case '3': pin = PD_3; break;
					case '4': pin = PD_4; break; case '5': pin = PD_5; break; case '6': pin = PD_6; break; case '7': pin = PD_7; break;
					case '8': pin = PD_8; break; case '9': pin = PD_9; break;
				}
				break;
			case 'E':
				switch(num){
					case '0': pin = PE_0; break; case '1': pin = PE_1; break; case '2': pin = PE_2; break; case '3': pin = PE_3; break;
					case '4': pin = PE_4; break; case '5': pin = PE_5; break; case '6': pin = PE_6; break; case '7': pin = PE_7; break;
					case '8': pin = PE_8; break; case '9': pin = PE_9; break; case 'A': pin = PE_A; break;
				}
				break;
			case 'F':
				switch(num){
					case '0': pin = PF_0; break; case '1': pin = PF_1; break; case '2': pin = PF_2; break; case '3': pin = PF_3; break;
					case '4': pin = PF_4; break; case '5': pin = PF_5; break;
				}
				break;
			case 'G':
				switch(num){
					case '0': pin = PG_0; break; case '1': pin = PG_1; break; case '2': pin = PG_2; break; case '3': pin = PG_3; break;
					case '4': pin = PG_4; break; case '5': pin = PG_5; break; case '6': pin = PG_6; break; case '7': pin = PG_7; break;
				}
				break;
			case 'H':
				switch(num){
					case '0': pin = PH_0; break; case '1': pin = PH_1; break; case '2': pin = PH_2; break; case '3': pin = PH_3; break;
					case '4': pin = PH_4; break; case '5': pin = PH_5; break; case '6': pin = PH_6; break; case '7': pin = PH_7; break;
				}
				break;
			case 'I':
				switch(num){
					case '0': pin = PI_0; break; case '1': pin = PI_1; break; case '2': pin = PI_2; break; case '3': pin = PI_3; break;
					case '4': pin = PI_4; break; case '5': pin = PI_5; break; case '6': pin = PI_6; break; case '7': pin = PI_7; break;
				}
				break;
			case 'J':
				switch(num){
					case '0': pin = PJ_0; break; case '1': pin = PJ_1; break; case '2': pin = PJ_2; break; case '3': pin = PJ_3; break;
					case '4': pin = PJ_4; break; case '5': pin = PJ_5; break; case '6': pin = PJ_6; break;
				}
				break;
			case 'K':
				switch(num){
					case '0': pin = PK_0; break; case '1': pin = PK_1; break; case '2': pin = PK_2; break; case '3': pin = PK_3; break;
					case '4': pin = PK_4; break; case '5': pin = PK_5; break; case '6': pin = PK_6; break;
				}
				break;
		}
		if(pin == NC){
			_AT_DBG_MSG(AT_FLAG_GPIO, AT_DBG_ALWAYS, "X,");
			continue;
		}
		// Initial input control pin
		gpio_init(&gpio_test, pin);
		if(write){
			gpio_dir(&gpio_test, PIN_OUTPUT);    // Direction: Output
			gpio_mode(&gpio_test, PullNone);     // No pull
			gpio_write(&gpio_test, data);
			_AT_DBG_MSG(AT_FLAG_GPIO, AT_DBG_ALWAYS, "%d,", data);
		}else{
			gpio_dir(&gpio_test, PIN_INPUT);     // Direction: Input
			gpio_mode(&gpio_test, PullUp);       // Pull-High
			val = gpio_read(&gpio_test);
			_AT_DBG_MSG(AT_FLAG_GPIO, AT_DBG_ALWAYS, "%d,", val);
		}
	}
	_AT_DBG_MSG(AT_FLAG_GPIO, AT_DBG_ALWAYS, "\n");
	// Recover debug info massage
	ConfigDebugInfo = tConfigDebugInfo;
}

void fATMR(void *arg)
{
	u32		idx;
	
	AT_PRINTK("[ATMR]: _AT_MP_SDR_TEST_");
#ifdef CONFIG_SDR_EN      
    for (idx = 0; idx < 0x200000; idx = idx+4){
        HAL_WRITE32(0x30000000, idx, 0x12345678);
        if (HAL_READ32(0x30000000, idx) != 0x12345678) {
            AT_PRINTK("[ATMR]: SDR test fail addr 0x08x, value 0x08%x",(0x30000000+idx),HAL_READ32(0x30000000, idx));
			return;
        }
    }
    AT_PRINTK("[ATMR]: SDR test success");
#endif
}

void fATMt(void *arg)
{
	int argc = 0;
	char *argv[MAX_ARGC] = {0};
	
	AT_PRINTK("[ATM#]: _AT_MP_TEST_");
	argc = parse_param(arg, argv);
}

void fATMx(void *arg)
{
	AT_PRINTK("[ATM?]: _AT_MP_HELP_");
}

log_item_t at_mp_items[] = {
	{"ATMG", fATMG,},	// MP GPIO test
	{"ATMR", fATMR,},	// MP SDR test
	{"ATM#", fATMt,},	// test command
	{"ATM?", fATMx,},	// Help
#if CONFIG_ATCMD_MP_EXT0
	{"ATM0", fATM0,},	// MP ext0 AT command
#endif
};

void at_mp_init(void)
{
	log_service_add_table(at_mp_items, sizeof(at_mp_items)/sizeof(at_mp_items[0]));
}

#if SUPPORT_LOG_SERVICE
log_module_init(at_mp_init);
#endif

#endif // #if CONFIG_ATCMD_MP


//#define __SFILE__ "cli_cmd.c"
#include <stdio.h>
#include "soc_types.h"
#include "cli.h"

#include "drv/adc/drv_adc.h"
#include "drv/tmr/drv_tmr.h"
#include "drv/tmr/hal_tm.h"

/*---------------------------------- CMDs -----------------------------------*/

void ms_irq_handler() 
{ 
  drv_tmr_clear_interrupt_status(TM_TM1_MS);

  static uint32_t second = 0;
  second ++;
  
  uint32_t adc_value = 0;
  uint16_t adc_code = 0;
  if(0 != drv_adc_read(&adc_code)){
      printf("read out fail!\n");
      return ;
  }    
  adc_value = adc_code;
  printf("%ld mv @ %ld\n", (ADC_CONVERT_OFFSET - (adc_value)*(ADC_CONVERT_SLOPE))/10000 ,second);

  drv_tmr_enable(TM_TM1_MS, TM_MODE_ONE_SHOT, 1000); 
} 

void Cmd_adc_22_start(s32 argc, char *argv[])
{
    

    uint32_t adc_id = 0;
	
	adc_id = strtoul(argv[1], NULL, 10);

    if ((argc != 2) || (adc_id > 3) || (adc_id < 0)) {
        printf("Usage   : adc_start <id>\n");
        printf("<id>    : adc id(0-3)\n");
        return;
    }

    printf("run ADC_%d start...%s %s\n" , adc_id, __DATE__ , __TIME__);

    if(0 != drv_adc_init(adc_id)){
        printf("init fail!\n");
        return ;
    };
     
    drv_tmr_sw_rst(TM_TM1_MS); 
    drv_tmr_init(TM_TM1_MS); 
    drv_tmr_register_irq_handler(TM_TM1_MS, ms_irq_handler); 
    drv_tmr_enable(TM_TM1_MS, TM_MODE_ONE_SHOT, 1000); 
}

void Cmd_adc_22_stop(s32 argc, char *argv[])
{

    uint32_t adc_id = 0;
	
	adc_id = strtoul(argv[1], NULL, 10);

    if ((argc != 2) || (adc_id > 3) || (adc_id < 0)) {
        printf("Usage   : adc_stop <id>\n");
        printf("<id>    : adc id(0-3)\n");
        return;
    }

    printf("run ADC_%d stop...%s %s\n" , adc_id, __DATE__ , __TIME__);

    if(0 != drv_adc_deinit(adc_id)){
        printf("deinit fail!\n");
        return ;
    }

    drv_tmr_disable(TM_TM1_MS); 
    
}

static void Cmd_help (int32_t argc, char *argv[])
{
    printf ("\n*************************************************************************\n");
    printf ("Test ADC Target : \n");
    printf ("                   GPIO_00 : ADC_id:3\n");
    printf ("                   SIO_20  : ADC_id:2\n");
    printf ("                   GPIO_21 : ADC_id:1\n");
    printf ("                   GPIO_22 : ADC_id:0\n");
    
    printf ("\n*************************************************************************\n");
    printf ("Usage: \n");
    printf ("       adc_start  <id>\n");
    printf ("brief:\n");
    printf ("       This function start the ADC measure.\n");
    printf ("Options:\n");
    printf (    "<id>                       The ADC target number(0-3).\n");

    printf ("\n*************************************************************************\n");
    printf ("Usage: \n");
    printf ("       adc_stop  <id>\n");
    printf ("brief:\n");
    printf ("       This function stop the ADC measure.\n");
    printf ("Options:\n");
    printf (    "<id>                       The ADC target number(0-3).\n");
    
}


/* ---------------------- Registered CMDs to CMD Table ---------------------- */
const CLICmds gCliCmdTable[] =
{
    { "help",               Cmd_help,           "ADC test help"         },
	{ "adc_start",   Cmd_adc_22_start,          "adc_start <id>"        },
    { "adc_stop",    Cmd_adc_22_stop,           "adc_stop <id>"         },
    /*lint -save -e611 */
    { (const char *)NULL, (CliCmdFunc)NULL,   (const char *)NULL },
    /*lint -restore */
};



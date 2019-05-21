#ifndef __TEMP_DETECT_H__
#define __TEMP_DETECT_H__

typedef  struct temp_detect_config_st
{
    UINT16 last_detect_val;
    UINT16 detect_intval;   
    UINT16 detect_thre;
    UINT16 inital_data;
    UINT16 dist_inital;
    UINT16 last_xtal_val;
	#if (CFG_SOC_NAME != SOC_BK7231)
    UINT16 xtal_thre_val;
    UINT16 xtal_init_val; 
	#endif  
    UINT32 detect_intval_change;    
    beken_timer_t detect_timer;
} TEMP_DETECT_CONFIG_ST, TEMP_DETECT_CONFIG_PTR;

#endif
// eof


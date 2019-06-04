#include "include.h"
#include "cmd_evm.h"
#include "mem_pub.h"
#include "str_pub.h"
#include "uart_debug_pub.h"
#include "tx_evm_pub.h"
#include "reg_mdm_cfg.h"

#include "udebug.h"
#include "uart_pub.h"
#include "schedule_pub.h"

#if CFG_SUPPORT_CALIBRATION
#include "bk7011_cal_pub.h"
#include "power_save_pub.h"
#endif
#if CFG_USE_TEMPERATURE_DETECT
#include "temp_detect_pub.h"
#endif

#include "ate_app.h"
#include "param_config.h"

typedef enum {
    TXEVM_E_STOP     = 0,
    TXEVM_E_REBOOT,
    TXEVM_E_DOFITTING,
    TXEVM_E_CLR_OPT,
    TXEVM_E_MAX
} TXEVM_E_TYPE;

typedef enum {
    TXEVM_G_TEMP     = 0,
    TXEVM_G_MAC,
    TXEVM_G_OPT,
    TXEVM_G_TEMP_FLASH,
    TXEVM_G_XTAL_FLASH,
    TXEVM_G_DIFF_FLASH,    
    TXEVM_G_MAX
} TXEVM_G_TYPE;

extern void sctrl_cali_dpll(UINT8 flag);
extern void sctrl_dpll_int_open(void);

UINT32 g_rate = EVM_DEFUALT_RATE;

#if CFG_TX_EVM_TEST
static UINT32 evm_translate_tx_rate(UINT32 rate)
{
    UINT32 param;

    switch(rate)
    {
    case 1 :
        param = 0x0;
        break;  // 1Mbps
    case 2 :
        param = 0x1;
        break;  // 2Mbps
    case 5 :
        param = 0x2;
        break;	// 5.5Mbps
    case 11:
        param = 0x3;
        break;	// 11Mbps
    case 6 :
        param = 0x4;
        break;	// 6Mbps
    case 9 :
        param = 0x5;
        break;	// 9Mbps
    case 12:
        param = 0x6;
        break;	// 12Mbps
    case 18:
        param = 0x7;
        break;	// 18Mbps
    case 24:
        param = 0x8;
        break;	// 24Mbps
    case 36:
        param = 0x9;
        break;	// 36Mbps
    case 48:
        param = 0xa;
        break;	// 48Mbps
    case 54:
        param = 0xb;
        break;	// 54Mbps
    default:
        param = rate;
        break;	// 54Mbps
    }

    if(rate < 5 || rate == 11)
        g_rate = EVM_DEFUALT_B_RATE;
    else
        g_rate = EVM_DEFUALT_RATE;        

    return param;
}
#endif

/*txevm [-m mode] [-c channel] [-l packet-length] [-r physical-rate]*/
int do_evm(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
#if CFG_TX_EVM_TEST
    char cmd0 = 0;
    char cmd1 = 0;
    UINT8 fail = 0;
    UINT32 packet_len = EVM_DEFUALT_PACKET_LEN;
    UINT32 channel = EVM_DEFUALT_CHANNEL;
    UINT32 ble_channel = EVM_DEFUALT_BLE_CHANNEL;
    UINT32 mode = EVM_DEFUALT_MODE;
    UINT32 rate = EVM_DEFUALT_RATE;
    UINT32 bandwidth = EVM_DEFUALT_BW;
    UINT32 pwr_mod = EVM_DEFUALT_PWR_MOD;
    UINT32 ble_pwr_mod = EVM_DEFUALT_BLE_PWR_MOD;
    UINT32 pwr_pa = EVM_DEFUALT_PWR_PA; 
    UINT32 ble_pwr_pa = EVM_DEFUALT_PWR_PA; 
    UINT32 modul_format = EVM_DEFUALT_MODUL_FORMAT;
    UINT32 guard_i_tpye = EVM_DEFUALT_GI_TYPE;
    UINT32 single_carrier = EVM_DEFUALT_SINGLE_CARRIER;    
    UINT32 dif_g = 0; 
    UINT32 test_mode = 0; 
    UINT8  trigger_pll= 1; 
    UINT32 arg_id = 1;
    UINT32 arg_cnt = argc;
    UINT32 lpfcapcal_i = 0x80, lpfcapcal_q = 0x80;
    UINT32 xtal = 0x10;
    UINT32 is_ble_test = 0;;
    UINT32 ble_test = 0;

    if(arg_cnt == 1)
        return 0;

    /*step0, parameter conversion*/
    while(arg_cnt > 1)
    {
        if(arg_cnt > 1)
        {
            cmd0 = argv[arg_id][0];
            cmd1 = argv[arg_id][1];
        }

        switch(cmd0)
        {
        case '-':
        {
            arg_cnt -= 1;

            if(arg_cnt < 1)
            {
                fail = 1;
                break;
            }

            arg_cnt -= 1;
            switch(cmd1)
            {
            case 'm':
                mode = os_strtoul(argv[arg_id + 1], NULL, 10);
                break;

            case 'c':
                if(is_ble_test)
                {
                    ble_channel = os_strtoul(argv[arg_id + 1], NULL, 10);
                }
                else
                {
                    channel = os_strtoul(argv[arg_id + 1], NULL, 10);
                }
                break;

            case 'l':
                packet_len = os_strtoul(argv[arg_id + 1], NULL, 10);
                break;

            case 'r': // mcs 0-7:  MCS0 =128, MCS1=129 t0 CS7=135.
                rate = os_strtoul(argv[arg_id + 1], NULL, 10);
                break;

            case 'b':
                bandwidth = os_strtoul(argv[arg_id + 1], NULL, 10);
                break;

            case 'f':  // 0x0: Non-HT; 0x1:Non-HT-DUP; 0x2: HT-MM;  0x3: HT-GF 
                modul_format = os_strtoul(argv[arg_id + 1], NULL, 10);
                break;

            case 'i':  //GI Type. 0x0: 800ns;  0x1: 400ns
                guard_i_tpye = os_strtoul(argv[arg_id + 1], NULL, 10);
                break;

            case 'w':
                single_carrier = os_strtoul(argv[arg_id + 1], NULL, 10);
                break; 

            case 't':
                test_mode = os_strtoul(argv[arg_id + 1], NULL, 10);
                break; 

            case 'u':
                trigger_pll = os_strtoul(argv[arg_id + 1], NULL, 10);
                break;                 

#if CFG_SUPPORT_MANUAL_CALI
            case 'g':  {// get system current temperature
                UINT32 op = os_strtoul(argv[arg_id + 1], NULL, 10);
                if(op < TXEVM_G_MAX) {
                    if(op == TXEVM_G_TEMP){
                        manual_cal_get_current_temperature();
                    }else if(op == TXEVM_G_MAC) {
                        UINT8 sysmac[6] = {0};
                        //manual_cal_get_macaddr_from_flash(&sysmac[0]);
                        cfg_load_mac(&sysmac[0]);
                        os_printf("sys MAC:%02x:%02x:%02x:%02x:%02x:%02x\r\n",
                            sysmac[0], sysmac[1], sysmac[2], sysmac[3], sysmac[4], sysmac[5]);
                    }else if(op == TXEVM_G_OPT) {
                        manual_cal_show_otp_flash();
                    }else if(op == TXEVM_G_TEMP_FLASH) {
                        manual_cal_load_temp_tag_from_flash();
                    }else if(op == TXEVM_G_DIFF_FLASH) {
                        manual_cal_load_differ_tag_from_flash();
                    }
                    #if (CFG_SOC_NAME != SOC_BK7231)
                    else if(op == TXEVM_G_XTAL_FLASH) {
                        manual_cal_load_xtal_tag_from_flash();
                    }
                    #endif // #if (CFG_SOC_NAME != SOC_BK7231)
                    return 0;
                } else {
                    return -1;
                }    
                break; 
            }

            case 'd':{
                UINT32 n20_n40 = 0;
                n20_n40 = os_strtoul(argv[arg_id + 1], NULL, 10);
                dif_g = (os_strtoul(argv[arg_id + 2], NULL, 10));
                os_printf("set dif g and n20: dif:%d\r\n", dif_g);
                if(n20_n40)
                    manual_cal_set_dif_g_n40(dif_g);
                else
                    manual_cal_set_dif_g_n20(dif_g);
                }
                return 0;
                break;

            case 'p': {  // power: mod, pa
                if(is_ble_test)
                {
                    ble_pwr_mod = (os_strtoul(argv[arg_id + 1], NULL, 10));
                    ble_pwr_pa = (os_strtoul(argv[arg_id + 2], NULL, 10));
                    arg_cnt -= 1;
                    arg_id += 1;
                    break;
                }
                else
                {
                    pwr_mod = (os_strtoul(argv[arg_id + 1], NULL, 10));
                    pwr_pa = (os_strtoul(argv[arg_id + 2], NULL, 10));
                    arg_cnt -= 1;
                    arg_id += 1;
                    os_printf("set pwr: gain:%d, unused:%d, rate:%d\r\n", pwr_mod, pwr_pa, g_rate);
                    rwnx_cal_set_txpwr(pwr_mod, g_rate);
                    return 0;
                }
                }

            case 's': { // save txpwr: rate:b or g? channel mod pa
                rate = os_strtoul(argv[arg_id + 1], NULL, 10);
                channel = os_strtoul(argv[arg_id + 2], NULL, 10);
                pwr_mod = (os_strtoul(argv[arg_id + 3], NULL, 10));
                pwr_pa = (os_strtoul(argv[arg_id + 4], NULL, 10));
                arg_cnt -= 3;
                arg_id += 3;
                os_printf("save pwr: rate:%d, ch:%d, gain:%d, unused:%d\r\n", 
                    rate, channel, pwr_mod, pwr_pa);
                manual_cal_save_txpwr(rate, channel, pwr_mod);
                return 0;
                }
#endif
            
            case 'e': { // 0: exit TXEVM,  1: reboot  -enter  2: do fitting
                UINT32 op = os_strtoul(argv[arg_id + 1], NULL, 10);
                if(op < TXEVM_E_MAX) {
                    if(op == TXEVM_E_STOP){
                        evm_stop_bypass_mac();
                    } 
                    #if CFG_SUPPORT_MANUAL_CALI
                    else if(op == TXEVM_E_DOFITTING) {
                        manual_cal_fitting_txpwr_tab();
                        manual_cal_save_chipinfo_tab_to_flash();
                        manual_cal_save_txpwr_tab_to_flash();
                    }
                    else if(op == TXEVM_E_CLR_OPT){
                        manual_cal_clear_otp_flash();
                    }
                    #endif
                    else {
                        //FUNCPTR reboot = 0;
                        //os_printf("reboot\r\n");
                        //(*reboot)();

                        #if ATE_APP_FUN
                        if(get_ate_mode_state())
                            os_printf("RF ATE mode!!!\r\n");
                        else
                            os_printf("Normal mode!!!\r\n");
                        #endif

                        #if CFG_USE_TEMPERATURE_DETECT
                        temp_detect_uninit();
                        #endif
                    }
                    return 0;
                } else {
                    return -1;
                }              
            } 
			break;

#if (CFG_SOC_NAME != SOC_BK7231)
            case 'q': {  // lpfcapcal i&q
            lpfcapcal_i = (os_strtoul(argv[arg_id + 1], NULL, 10));
            lpfcapcal_q = (os_strtoul(argv[arg_id + 2], NULL, 10));
            arg_cnt -= 1;
            arg_id += 1;
            os_printf("set lpfcapcal: I:%d, Q:%d\r\n", lpfcapcal_i, lpfcapcal_q);
            manual_cal_set_lpf_iq(lpfcapcal_i, lpfcapcal_q);
            return 0;
            }

            case 'x': {  // xtal_cali
            xtal = os_strtoul(argv[arg_id + 1], NULL, 10);
            os_printf("xtal_cali:%d\r\n", xtal);
            manual_cal_set_xtal(xtal);
            return 0;
            }            
#endif // (CFG_SOC_NAME != SOC_BK7231)
            case 'o': {
                is_ble_test = 1;
                ble_test = os_strtoul(argv[arg_id + 1], NULL, 10); // 1:start 0:stop                
            }
            break;
            
            default:
                fail = 1;
                break;
            }
        }
        break;

        default:
            fail = 1;
            break;
        }

        if(fail)
        {
            return 1;
        }

        arg_id += 2;
    }

    /*step1, parameter check*/
    if(!(((1 == mode)
            || (0 == mode))
            && ((1 == bandwidth)
                || (0 == bandwidth))
            && (modul_format <= 3)
            && (guard_i_tpye <= 1)
            && ((1 <= channel)
                && (14 >= channel))
            && ((0 < packet_len)
                && (4095 >= packet_len))
            && ((1 == rate)
                || (2 == rate)
                || (5 == rate)
                || (6 == rate)
                || (9 == rate)
                || (11 == rate)
                || (12 == rate)
                || (18 == rate)
                || (24 == rate)
                || (36 == rate)
                || (48 == rate)
                || (54 == rate)
                || (128 == rate)      // MCS0
                || (129 == rate)
                || (130 == rate)
                || (131 == rate)      // MCS3 
                || (132 == rate)
                || (133 == rate)
                || (134 == rate)
                || (135 == rate))))  // MCS7
    {
        return 1;
    }

    /*step2, handle*/
    if(!is_ble_test)
    {
        if(mode)
        {
            mdm_scramblerctrl_set(0x95);
            evm_bypass_mac_set_tx_data_length(modul_format, packet_len);
            if(rate <= 54) {
                modul_format = 0;
            }
            evm_bypass_mac_set_rate_mformat(rate, modul_format);
            evm_bypass_mac_set_channel(channel);
            evm_set_bandwidth(bandwidth);
            evm_bypass_mac_set_guard_i_type(guard_i_tpye);
            
            evm_bypass_mac_test();

#if CFG_SUPPORT_CALIBRATION
            CHECK_OPERATE_RF_REG_IF_IN_SLEEP();
            rwnx_cal_set_txpwr_by_rate(evm_translate_tx_rate(rate), test_mode);
            CHECK_OPERATE_RF_REG_IF_IN_SLEEP_END();
#endif
            evm_start_bypass_mac();

            if(single_carrier)
                evm_bypass_set_single_carrier();
            
            if(trigger_pll) {
                os_printf("cal dpll and open int\r\n");
                sctrl_cali_dpll(0);
                sctrl_dpll_int_open();
            }
            
        }
        else
        {
            evm_via_mac_set_rate((HW_RATE_E)rate, 1);
            evm_set_bandwidth(bandwidth);
            evm_via_mac_set_channel(channel);

            evm_via_mac_begin();
        }
        
    }
    else
    {
        if(ble_test)
        {
            //os_printf("ble_test\r\n");
            rwnx_cal_set_txpwr(ble_pwr_mod, 11);
            evm_bypass_ble_test_start(ble_channel);
        }
        else
        {
            evm_bypass_ble_test_stop();
        }
    }
    
#endif // CFG_TX_EVM_TEST 

    return 0;
}

// eof


#include "include.h"
#include "rx_sensitivity.h"
#include "rx_sensitivity_pub.h"

#include "uart_pub.h"
#include "arm_arch.h"
#include "ble_pub.h"
#include "icu_pub.h"
#include "sys_ctrl_pub.h"

#include "mac.h"
#include "phy.h"
#include "hal_machw.h"
#include "me.h"
#include "mm.h"
#include "ke_task.h"
#include "vif_mgmt.h"
#include "drv_model_pub.h"
#include "target_util_pub.h"
#include "mac_phy_bypass_pub.h"

#if CFG_RX_SENSITIVITY_TEST
struct mac_addr const rs_mac_addr = {
								    {0x7112, 0x7111, 0x7111}
								 };

UINT32 g_txbw20; 
UINT32 g_txbw40; 
UINT32 g_txbw80; 
UINT32 g_txbw160;	
UINT32 g_txoctectinampdu; 
UINT32 g_txampdu;
UINT32 g_rxbw20;
UINT32 g_rxbw40;
UINT32 g_rxbw80;
UINT32 g_rxbw160;
UINT32 g_rxoctectinampdu;
UINT32 g_rxampdu;
UINT32 g_fcserror;
UINT32 g_phyerror;
UINT32 g_rxflowc; 

UINT32 g_s_rxbw20;
UINT32 g_s_rxbw40;


const UINT16 rs_freq_2_4_G[RS_2_4_G_CHANNEL_NUM] = {
						2412,
						2417,
						2422,
						2427,
						2432,
						2437,
						2442,
						2447,
						2452,
						2457,
						2462,
						2467,
						2472,
						2484
};

UINT32 rs_channel = 2437;
UINT32 rs_mode = PHY_CHNL_BW_20;

void rs_init(UINT32 channel, UINT32 mode)
{
	//BOOL p2p = 0;
	//UINT8 vif_idx;
	//UINT8 vif_type = 2;
	struct phy_cfg_tag cfg;
	
	/*reset mm*/
	RS_PRT("[RS]reset_mm\r\n");
	hal_machw_stop();
	phy_stop();
	me_init();
	mm_init();
	ke_state_set(TASK_MM, MM_IDLE);
	
	/*config me*/
	RS_PRT("[RS]config_me\r\n");
	
	/*config me channel*/
	RS_PRT("[RS]config_me_channel\r\n");
	
	/*start mm*/
	RS_PRT("[RS]start_mm\r\n");
	cfg.parameters[0] = 1;
	cfg.parameters[1] = 0;
    phy_init(&cfg);
	
    phy_set_channel(PHY_BAND_2G4, mode, channel, channel, 0, PHY_PRIM);
	
	/*add mm interface*/
	//RS_PRT("[RS]add_mm_interface\r\n");
	//vif_mgmt_register(&rs_mac_addr, vif_type, p2p, &vif_idx);

	/* Put the HW in active state*/
	mm_active();
}

void rs_rx_monitor(void)
{
	hal_machw_enter_monitor_mode();
	
	RS_PRT("[RS]rs_rx_monitor\r\n");
}

UINT32 rs_set_channel(UINT32 channel_id)
{
	if((0 == channel_id)
		|| (channel_id > RS_2_4_G_CHANNEL_NUM))
	{
		return 1;
	}
	
	rs_channel = rs_freq_2_4_G[channel_id - 1];

	return 0;
}

UINT32 rs_set_mode(UINT32 mode)
{
	rs_mode = mode;
	return 0;
}

void rs_test(void)
{
	rs_init(rs_channel, rs_mode);
	rs_rx_monitor();
}

void rx_get_rx_result_begin(void)
{
	g_txbw20 = REG_READ(0xc0000b70);
	g_txbw40 = REG_READ(0xc0000b74);
	g_txbw80 = REG_READ(0xc0000b78);
	g_txbw160 = REG_READ(0xc0000b7c);

	(void)REG_READ(0xc0000b34);
	g_txoctectinampdu = REG_READ(0xc0000b38);
	g_txampdu = REG_READ(0xc0000b30);

	g_rxbw20 = REG_READ(0xc0000b80);
	g_rxbw40 = REG_READ(0xc0000b84);
	g_rxbw80 = REG_READ(0xc0000b88);
	g_rxbw160 = REG_READ(0xc0000b8c);

	(void)REG_READ(0xc0000b48);
	g_rxoctectinampdu = REG_READ(0xc0000b4c);
	g_rxampdu = REG_READ(0xc0000b3c);

	g_fcserror = REG_READ(0xc0000804); 
	g_phyerror = REG_READ(0xc0000808); 
	g_rxflowc = REG_READ(0xc000080c); 

	REG_WRITE(0xc0000220, 0x0);
	REG_WRITE(0xc0000290, 0x0);
	REG_WRITE(0xc0000294, 0x0);
	REG_WRITE(0xc0000298, 0x0);
	REG_WRITE(0xc0000040, 0x00011881);
}

void rx_clean_rx_statistic_result(void)
{
    RS_PRT("[RS]RXSENS_RTYPTE_CLEAN\r\n");
    g_s_rxbw20 = g_s_rxbw40 = 0;

    rx_get_rx_result_begin();
}

UINT32 rx_get_rx20M_statistic_result(void)
{
    RS_PRT("[RS]RXSENS_RTYPTE_20M :%d\r\n", g_s_rxbw20);
    return g_s_rxbw20;
}

UINT32 rx_get_rx40M_statistic_result(void)
{
    RS_PRT("[RS]RXSENS_RTYPTE_40M :%d\r\n", g_s_rxbw40);
    return g_s_rxbw40;
}

void rx_get_rx_result_end(void)
{	
	UINT32 txbw20new;
	UINT32 txbw40new;
	UINT32 txbw80new;
	UINT32 txbw160new;

	UINT32 txoctectinampdunew;
	UINT32 txampdunew;

	UINT32 rxbw20new;
	UINT32 rxbw40new;
	UINT32 rxbw80new;
	UINT32 rxbw160new;

	UINT32 rxoctectinampdunew;
	UINT32 rxampdunew;

	UINT32 fcserrornew;
	UINT32 phyerrornew;
	UINT32 rxflowcnew;
	UINT32 ccapri;
	UINT32 ccasec20;
	UINT32 ccasec40;
	UINT32 ccasec80;

	UINT32 txbw20cur;
	UINT32 txbw40cur;
	UINT32 txbw80cur;
	UINT32 txbw160cur;
	UINT32 txtot;

	UINT32 txampducur;
	UINT32 txoctectinampducur;
	
	UINT32 rxbw20cur;
	UINT32 rxbw40cur;
	UINT32 rxbw80cur;
	UINT32 rxbw160cur;

	UINT32 rxampducur;
	UINT32 rxoctectinampducur;

	UINT32 fcserrcur;
	UINT32 phyerrcur;
	UINT32 rxflowccur;
	UINT32 rxtot;

	txbw20new = REG_READ(0xc0000b70);
	txbw40new = REG_READ(0xc0000b74);
	txbw80new = REG_READ(0xc0000b78);
	txbw160new = REG_READ(0xc0000b7c);

	(void)REG_READ(0xc0000b34);
	txoctectinampdunew = REG_READ(0xc0000b38);
	txampdunew = REG_READ(0xc0000b30);

	rxbw20new = REG_READ(0xc0000b80);
	rxbw40new = REG_READ(0xc0000b84);
	rxbw80new = REG_READ(0xc0000b88);
	rxbw160new = REG_READ(0xc0000b8c);

	(void)REG_READ(0xc0000b48);
	rxoctectinampdunew = REG_READ(0xc0000b4c);
	rxampdunew = REG_READ(0xc0000b3c);

	fcserrornew = REG_READ(0xc0000804); 
	phyerrornew = REG_READ(0xc0000808); 
	rxflowcnew = REG_READ(0xc000080c); 
	ccapri = REG_READ(0xc0000220);
	ccasec20 = REG_READ(0xc0000290);
	ccasec40 = REG_READ(0xc0000294);
	ccasec80 = REG_READ(0xc0000298);
	
	txbw20cur = ((txbw20new - g_txbw20));
	txbw40cur = ((txbw40new - g_txbw40));
	txbw80cur = ((txbw80new - g_txbw80));
	txbw160cur = ((txbw160new - g_txbw160));
	txtot = ((txbw40cur + txbw20cur + txbw80cur + txbw160cur));

	txampducur = ((txampdunew - g_txampdu));
	txoctectinampducur = ((txoctectinampdunew - g_txoctectinampdu));

	if(0 == txtot)
	{
		txtot = 1;
	}

	if(0 == txampducur)
	{
		txampducur = 1;
	}

	rxbw20cur =  (rxbw20new - g_rxbw20);
    g_s_rxbw20 = rxbw20cur;
	rxbw40cur =  (rxbw40new - g_rxbw40);
    g_s_rxbw40 = rxbw40cur;
    
	rxbw80cur =  (rxbw80new - g_rxbw80);
	rxbw160cur =  (rxbw160new - g_rxbw160);

	rxampducur =  (rxampdunew - g_rxampdu);
	rxoctectinampducur =  (rxoctectinampdunew - g_rxoctectinampdu);

	fcserrcur =  (fcserrornew - g_fcserror);
	phyerrcur =  (phyerrornew - g_phyerror);
	rxflowccur =  (rxflowcnew - g_rxflowc);
	rxtot =  (rxbw40cur + rxbw20cur + rxbw80cur + rxbw160cur + fcserrcur + phyerrcur + rxflowccur);

	if(0 == rxtot)
	{
		rxtot = 1;
	} 

	RS_PRT("\r\nrx\r\n");
	RS_PRT("20mhz  : %0d (%0d %%)\r\n",  rxbw20cur, (100*rxbw20cur/rxtot));
	RS_PRT("40mhz  : %0d (%0d %%)\r\n",  rxbw40cur, (100*rxbw40cur/rxtot));    
 
	if(0 == rxampducur)
	{
		rxampducur = 1;
	} 

	RS_PRT("fcserr : %0d (%0d %%), ",  fcserrcur, (100*fcserrcur/rxtot));
	RS_PRT("phyerr : %0d (%0d %%), ",  phyerrcur, (100*phyerrcur/rxtot)); 
	RS_PRT("rxflowerr: %0d (%0d %%)\r\n",  rxflowccur, (100*rxflowccur/rxtot));
	(void) ccapri;
	(void) ccasec20;
	(void) ccasec40;
	(void) ccasec80;
}

/* refer to txrx_stat.sh*/
void rs_get_rx_result(void)
{
	UINT32 TXBW20;                       
	UINT32 TXBW40;                       
	UINT32 TXBW80;
	UINT32 TXBW160;
	                                     
	UINT32 TXOCTECTINAMPDU;
	UINT32 TXAMPDU;
	                                     
	UINT32 RXBW20;
	UINT32 RXBW40;
	UINT32 RXBW80;
	UINT32 RXBW160;
	                                     
	UINT32 RXOCTECTINAMPDU;
	UINT32 RXAMPDU;
	                                     
	UINT32 FCSERROR;
	UINT32 PHYERROR;
	UINT32 RXFLOWC;

	UINT32 TXBW20NEW;
	UINT32 TXBW40NEW;
	UINT32 TXBW80NEW;
	UINT32 TXBW160NEW;

	UINT32 TXOCTECTINAMPDUNEW;
	UINT32 TXAMPDUNEW;

	UINT32 RXBW20NEW;
	UINT32 RXBW40NEW;
	UINT32 RXBW80NEW;
	UINT32 RXBW160NEW;

	UINT32 RXOCTECTINAMPDUNEW;
	UINT32 RXAMPDUNEW;

	UINT32 FCSERRORNEW;
	UINT32 PHYERRORNEW;
	UINT32 RXFLOWCNEW;
	UINT32 CCAPRI;
	UINT32 CCASEC20;
	UINT32 CCASEC40;
	UINT32 CCASEC80;

	UINT32 TXBW20CUR;
	UINT32 TXBW40CUR;
	UINT32 TXBW80CUR;
	UINT32 TXBW160CUR;
	UINT32 TXTOT;

	UINT32 TXAMPDUCUR;
	UINT32 TXOCTECTINAMPDUCUR;
	
	UINT32 RXBW20CUR;
	UINT32 RXBW40CUR;
	UINT32 RXBW80CUR;
	UINT32 RXBW160CUR;

	UINT32 RXAMPDUCUR;
	UINT32 RXOCTECTINAMPDUCUR;

	UINT32 FCSERRCUR;
	UINT32 PHYERRCUR;
	UINT32 RXFLOWCCUR;
	UINT32 RXTOT;
		
	TXBW20 = REG_READ(0xc0000b70);
	TXBW40 = REG_READ(0xc0000b74);
	TXBW80 = REG_READ(0xc0000b78);
	TXBW160 = REG_READ(0xc0000b7C);

	(void)REG_READ(0xc0000b34);
	TXOCTECTINAMPDU = REG_READ(0xc0000b38);
	TXAMPDU = REG_READ(0xc0000b30);

	RXBW20 = REG_READ(0xc0000b80);
	RXBW40 = REG_READ(0xc0000b84);
	RXBW80 = REG_READ(0xc0000b88);
	RXBW160 = REG_READ(0xc0000b8C);

	(void)REG_READ(0xc0000b48);
	RXOCTECTINAMPDU = REG_READ(0xc0000b4C);
	RXAMPDU = REG_READ(0xc0000b3C);

	FCSERROR = REG_READ(0xc0000804); 
	PHYERROR = REG_READ(0xc0000808); 
	RXFLOWC = REG_READ(0xc000080c); 

	REG_WRITE(0xc0000220, 0x0);
	REG_WRITE(0xc0000290, 0x0);
	REG_WRITE(0xc0000294, 0x0);
	REG_WRITE(0xc0000298, 0x0);
	REG_WRITE(0xc0000040, 0x00011881);

	delay(100);

	TXBW20NEW = REG_READ(0xc0000b70);
	TXBW40NEW = REG_READ(0xc0000b74);
	TXBW80NEW = REG_READ(0xc0000b78);
	TXBW160NEW = REG_READ(0xc0000b7C);

	(void)REG_READ(0xc0000b34);
	TXOCTECTINAMPDUNEW = REG_READ(0xc0000b38);
	TXAMPDUNEW = REG_READ(0xc0000b30);

	RXBW20NEW = REG_READ(0xc0000b80);
	RXBW40NEW = REG_READ(0xc0000b84);
	RXBW80NEW = REG_READ(0xc0000b88);
	RXBW160NEW = REG_READ(0xc0000b8C);

	(void)REG_READ(0xc0000b48);
	RXOCTECTINAMPDUNEW = REG_READ(0xc0000b4C);
	RXAMPDUNEW = REG_READ(0xc0000b3C);

	FCSERRORNEW = REG_READ(0xc0000804); 
	PHYERRORNEW = REG_READ(0xc0000808); 
	RXFLOWCNEW = REG_READ(0xc000080c); 
	CCAPRI = REG_READ(0xc0000220);
	CCASEC20 = REG_READ(0xc0000290);
	CCASEC40 = REG_READ(0xc0000294);
	CCASEC80 = REG_READ(0xc0000298);
	
	TXBW20CUR = ((TXBW20NEW - TXBW20));
	TXBW40CUR = ((TXBW40NEW - TXBW40));
	TXBW80CUR = ((TXBW80NEW - TXBW80));
	TXBW160CUR = ((TXBW160NEW - TXBW160));
	TXTOT = ((TXBW40CUR + TXBW20CUR + TXBW80CUR + TXBW160CUR));

	TXAMPDUCUR = ((TXAMPDUNEW - TXAMPDU));
	TXOCTECTINAMPDUCUR = ((TXOCTECTINAMPDUNEW - TXOCTECTINAMPDU));

	if(0 == TXTOT)
	{
		TXTOT = 1;
	}
	
	RS_PRT("TX\n");
	RS_PRT("20MHz  : %0d (%0d %%)\r\n", TXBW20CUR, 100*TXBW20CUR/TXTOT);
	RS_PRT("40MHz  : %0d (%0d %%)\r\n", TXBW40CUR, 100*TXBW40CUR/TXTOT);
	RS_PRT("80MHz  : %0d (%0d %%)\r\n", TXBW80CUR, 100*TXBW80CUR/TXTOT);
	RS_PRT("160MHz : %0d (%0d %%)\r\n", TXBW160CUR, 100*TXBW160CUR/TXTOT);

	RS_PRT("AMPDU  : %0d (%0d %%)\r\n", TXAMPDUCUR, 100*TXAMPDUCUR/TXTOT);

	if(0 == TXAMPDUCUR)
	{
		TXAMPDUCUR = 1;
	}
	
	RS_PRT("Bytes in AMPDU : %0d\r\n", (TXOCTECTINAMPDUCUR/TXAMPDUCUR));
	RS_PRT("AMPDU Throughput : %0.1f Mbps\r\n", ((float)8*TXOCTECTINAMPDUCUR/1000000));

	RXBW20CUR =  (RXBW20NEW - RXBW20);
	RXBW40CUR =  (RXBW40NEW - RXBW40);
	RXBW80CUR =  (RXBW80NEW - RXBW80);
	RXBW160CUR =  (RXBW160NEW - RXBW160);

	RXAMPDUCUR =  (RXAMPDUNEW - RXAMPDU);
	RXOCTECTINAMPDUCUR =  (RXOCTECTINAMPDUNEW - RXOCTECTINAMPDU);

	FCSERRCUR =  (FCSERRORNEW - FCSERROR);
	PHYERRCUR =  (PHYERRORNEW - PHYERROR);
	RXFLOWCCUR =  (RXFLOWCNEW - RXFLOWC);
	RXTOT =  (RXBW40CUR + RXBW20CUR + RXBW80CUR + RXBW160CUR + FCSERRCUR + PHYERRCUR + RXFLOWCCUR);

	if(0 == RXTOT)
	{
		RXTOT = 1;
	} 

	RS_PRT("\nRX\n");
	RS_PRT("20MHz  : %0d (%0d %%)\r\n",  RXBW20CUR, (100*RXBW20CUR/RXTOT));
	RS_PRT("40MHz  : %0d (%0d %%)\r\n",  RXBW40CUR, (100*RXBW40CUR/RXTOT));
	RS_PRT("80MHz  : %0d (%0d %%)\r\n",  RXBW80CUR, (100*RXBW80CUR/RXTOT));
	RS_PRT("160MHz : %0d (%0d %%)\r\n",  RXBW160CUR, (100*RXBW160CUR/RXTOT));

	RS_PRT("AMPDU  : %0d (%0d %%)\r\n",  RXAMPDUCUR, (100*RXAMPDUCUR/RXTOT));
	  
	if(0 == RXAMPDUCUR)
	{
		RXAMPDUCUR = 1;
	} 

	RS_PRT("Bytes in AMPDU : %0d\r\n", (RXOCTECTINAMPDUCUR/RXAMPDUCUR));
	RS_PRT("AMPDU Throughput : %0.1f Mbps\r\n", ((float)8*RXOCTECTINAMPDUCUR/1000000));

	RS_PRT("FCSErr   : %0d (%0d %%)\r\n",  FCSERRCUR, (100*FCSERRCUR/RXTOT));
	RS_PRT("PHYErr   : %0d (%0d %%)\r\n",  PHYERRCUR, (100*PHYERRCUR/RXTOT)); 
	RS_PRT("RxFlowErr: %0d (%0d %%)\r\n",  RXFLOWCCUR, (100*RXFLOWCCUR/RXTOT)); 
	RS_PRT("CCAPrim  Busy : %3d %%\r\n", (CCAPRI / 10000));
	RS_PRT("CCASec20 Busy : %3d %%\r\n", (CCASEC20 / 10000));
	RS_PRT("CCASec40 Busy : %3d %%\r\n", (CCASEC40 / 10000));
	RS_PRT("CCASec80 Busy : %3d %%\r\n", (CCASEC80 / 10000));
	RS_PRT("\r\n");
}

void rx_clean_ble_rx_result(void)
{
#if (CFG_SOC_NAME != SOC_BK7231)
    sddev_control(BLE_DEV_NAME, CMD_BLE_STOP_COUNTING, NULL);
#endif
}

void rx_start_ble_rx_counting()
{
#if (CFG_SOC_NAME != SOC_BK7231)
    sddev_control(BLE_DEV_NAME, CMD_BLE_START_COUNTING, NULL);
#endif
}

void rs_ble_test_start(UINT32 channel)
{
#if (CFG_SOC_NAME != SOC_BK7231)
    UINT32 param;
    param = PWD_BLE_CLK_BIT;
    
    sddev_control(SCTRL_DEV_NAME, CMD_BLE_RF_BIT_SET, NULL);
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_BLE_POWERUP, NULL);
    sddev_control(ICU_DEV_NAME, CMD_TL410_CLK_PWR_UP, &param);

    param = 0x3ba7a940;
    sddev_control(BLE_DEV_NAME, CMD_BLE_SET_GFSK_SYNCWD, &param);
    sddev_control(BLE_DEV_NAME, CMD_BLE_AUTO_CHANNEL_DISABLE, NULL);
    sddev_control(BLE_DEV_NAME, CMD_BLE_AUTO_SYNCWD_DISABLE, NULL);

    if(channel > 39)
    {
        channel = 39;
    }
    param = (channel + 1) * 2;
    sddev_control(BLE_DEV_NAME, CMD_BLE_SET_CHANNEL, &param);
    param = PN9_RX;
    sddev_control(BLE_DEV_NAME, CMD_BLE_SET_PN9_TRX, &param);
#endif
}

void rs_ble_test_stop(void)
{
#if (CFG_SOC_NAME != SOC_BK7231)
    sddev_control(SCTRL_DEV_NAME, CMD_BLE_RF_BIT_CLR, NULL);
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_BLE_POWERDOWN, NULL);
#endif
}

void rx_get_ble_rx_result(void)
{
#if (CFG_SOC_NAME != SOC_BK7231)
    UINT32 rx_ble_total = 0xFFFFFFFF;
    UINT32 rx_ble_total_old = 0xFFFFFFFF;
    UINT32 rx_ble_err_total = 0xFFFFFFFF;
    UINT32 rx_ble_err_total_old = 0xFFFFFFFF;
    
    sddev_control(BLE_DEV_NAME, CMD_BLE_HOLD_PN9_ESTIMATE, NULL);
    
    rx_ble_total = REG_READ(0x0080B454);
    while(rx_ble_total != rx_ble_total_old)
    {
        rx_ble_total_old = rx_ble_total;
        rx_ble_total = REG_READ(0x0080B454);
    }

    rx_ble_err_total = REG_READ(0x0080B458);
    while(rx_ble_err_total != rx_ble_err_total_old)
    {
        rx_ble_err_total_old = rx_ble_err_total;
        rx_ble_err_total = REG_READ(0x0080B458);
    }

    RS_PRT("\r\nTotal  : %d\r\n", rx_ble_total);
    RS_PRT("Error  : %d\r\n", rx_ble_err_total);

    if(rx_ble_err_total > 0)
    {
        RS_PRT("BER    : %d.%02d %%\r\n", (10000 * rx_ble_err_total / rx_ble_total) / 100,
            (1000 * rx_ble_err_total / rx_ble_total) % 100);
    }
    else
    {
        RS_PRT("BER    : %d.%02d %%\r\n", 0, 0);
    }

    sddev_control(BLE_DEV_NAME, CMD_BLE_STOP_COUNTING, NULL);
    sddev_control(BLE_DEV_NAME, CMD_BLE_START_COUNTING, NULL);
#endif
}

#endif // CFG_RX_SENSITIVITY_TEST
// eof


#ifndef __INC_RTL8721D_HP_SYS_ON_BIT_H
#define __INC_RTL8721D_HP_SYS_ON_BIT_H

/* REG_HS_PWR_ISO_CTRL 0x0000 */
#define BIT_HSYS_ISO_AUXPLL_PCM			BIT(19)	/* R/W	1		1: isolation signal from PCM 45.1584MHz PLL  When this PLL is power off; 0: release isolation */
#define BIT_HSYS_ISO_AUXPLL_I2S			BIT(18)	/* R/W	1		1: isolation signal from I2S 98.304MHz PLL  When this PLL is power off; 0: release isolation */
#define BIT_HSYS_ISO_BT_EN 					BIT(16)	/* R/W	1		1: isolation signal from BT When BT is power off; 0: release isolation */

#define BIT_HSYS_PWC_BT_EN_BIT1			BIT(1)		/* R/W	0		"2'b11: Enable BT power cut 2'b00: Disable. Note:  When enable this power cut,  bit0 should be set to 1'b1 first, then delay 100us set bit1 to 1'b1" */
#define BIT_HSYS_PWC_BT_EN_BIT0			BIT(0)

/* REG_HS_PLATFORM_PARA 0x000C */
#define BIT_HSYS_PLFM_WDTRST_OPT				BIT(23)	/*  R/W	0		HS watchdog reset option, 0:  watchdog reset hs platform only,   1: reset hs/ls/spic platform */
#define BIT_HSYS_GDMA_CLK_ALWAYS_DISABLE	BIT(22)	/*  R/W	0		Disable dmac clk all the time. if clk_always_enable=0 and clk_always_disable=1,  the whole dmac¡¯s clk will be shut down to save power. */
#define BIT_HSYS_GDMA_CLK_ALWAYS_ENABLE		BIT(21)	/*  R/W	0		"Enable dmac clk all the time. It has the highest priority. If clk_always_enable=0 and clk_always_disable=0, dmac can shut any channel¡¯s clk when this channel has no job to save power. If clk_always_enable=1, all the channel¡¯s clk will be valid." */
#define BIT_HSYS_PSRAM_SPDUPSIM				BIT(20)	/*  R/W	0		 */
#define BIT_HSYS_PLFM_AUTO_ICG_EN				BIT(19)	/*  R/W	0		1: Enable HS platform auto clock gating for power saving */
#define BIT_HSYS_SHARE_BT_MEM					BIT(18)	/*  R/W	0		1: Enable KM4 to share BT memory  */
#define BIT_HSYS_SHARE_WL_MEM				BIT(17)	/*  R/W	0		1: Enable KM4 to share WL memory  */
#define BIT_KM4_RETENTION_MODE				BIT(16)	/*  R/W	0		"1: enable,  when KM4 reset,  Cache will not be clear 0: Disable, when KM4 reset, Cache will be clear by HW" */
#define BIT_KM4_DBGRESTARTED_STATUS			BIT(3)	/*  R	0		BIT_KM4_DBGRESTARTED_STATUS	KM4 debug restart status */
#define BIT_KM4_HALTED_STATUS					BIT(2)	/*  R	0		KM4 halt status */
#define BIT_KM4_LOCKUP_STATUS					BIT(1)	/*  R	0		KM4 lockup status */
#define BIT_KM4_SLEEP_STATUS					BIT(0)	/*  R	0		KM4 sleep status used for __WFE, tm_sl_sleepsys_r */


/* #define REG_HS_RFAFE_IND_VIO1833			0x0020 */
#define BIT_RFAFE_IND_VIO1833					BIT(0)	/* R/W	0		"RFAFE voltage indication: 0: 1.8v 1: 3.3v" */

/* REG_HS_PLL_I2S_CTRL0				0x0080 */
#define BIT_PLL_I2S_POWCUT_EN				BIT(31)	/* R/W	0		erc enable. */
#define BIT_PLL_I2S_DIV_EN					BIT(30)	/* R/W	0		input divider enable  (need pwl signal) 0->1 */
#define BIT_PLL_I2S_EN						BIT(29)	/* R/W	0		  (need pwl signal) 0->1 */
#define BIT_PLL_SHIFT_I2S_CPC_SEL			26		/* R/W	000		"charge pump current selection . Default=5u CP current select : 000: Icp=5uA,    001: Icp=10uA, 010: Icp=15uA, 011: Icp=20uA; 100:Icp=25uA,   101: Icp=30uA, 110: Icp=35uA, 111: Icp=40uA; " */
#define BIT_PLL_MASK_I2S_CPC_SEL			0x07	/* R/W	000		"charge pump current selection . Default=5u CP current select : 000: Icp=5uA,    001: Icp=10uA, 010: Icp=15uA, 011: Icp=20uA; 100:Icp=25uA,   101: Icp=30uA, 110: Icp=35uA, 111: Icp=40uA; " */
#define BIT_PLL_I2S_WDG_EN					BIT(25)	/* R/W	1		0: watch dog on ,1: watch dog off */
#define BIT_PLL_SHIFT_I2SS_PS_SEL			22
#define BIT_PLL_MASK_I2SS_PS_SEL			0x07	/* R/W	000	*/
#define BIT_PLL_SHIFT_I2S_CP_SEL			20
#define BIT_PLL_MASK_I2S_CP_SEL			0x03	/* R/W	00		"Cp selection. Default=3p 00: Cp=3p, 01: Cp=4p, 10: Cp=5p, 11: Cp=6p" */
#define BIT_PLL_SHIFT_I2S_RS_SEL			17
#define BIT_PLL_MASK_I2S_RS_SEL			0x07	/* R/W	110		"Rs selection. Default=14k  000: Rs=2k,   001: Rs=4k,   010: Rs=6k,   011: Rs=8k,  100: Rs=10k, 101: Rs=12k, 110: Rs=14k, 111: Rs=16k; " */
#define BIT_PLL_SHIFT_I2S_CS_SEL			15
#define BIT_PLL_MASK_I2S_CS_SEL			0x03	/* R/W	10		"Cs selection. Default=50p 00: Cs=30p, 01: Cs=40p, 10: Cs=50p, 11: Cs=60p" */
#define BIT_PLL_SHIFT_I2S_R3_SEL			12
#define BIT_PLL_MASK_I2S_R3_SEL			0x07	/* R/W	010		"R3 selection. Default=1k 000: R3=0k, 001: R3=0.5k, 010: R3=1k, 011: R3=1.5k, 100: R3=2k, 101: R3=2.5k, 110: R3=3k, 111: R3=3.5k;  " */
#define BIT_PLL_SHIFT_I2S_C3_SET			10
#define BIT_PLL_MASK_I2S_C3_SET			0x03	/* R/W	10		"Cp selection. Default=5p 00: Cp=3p, 01: Cp=4p, 10: Cp=5p, 11: Cp=6p" */
#define BIT_PLL_I2S_FREF_EDGE_SEL			BIT(9)	/* R/W	1		0:fref,1:frefb */
#define BIT_PLL_I2S_CLK_EN					BIT(8)	/* R/W	0		output clk enable */
#define BIT_PLL_I2S_DIV2_EN					BIT(7)	/* R/W	0		output clk div 2 enable */
#define BIT_PLL_I2S_PS_EN					BIT(6)	/* R/W	0		phase selector enable */
#define BIT_PLL_SHIFT_I2S_DIV_SEL			3
#define BIT_PLL_MASK_I2S_DIV_SEL			0x07	/* R/W	110		output divider selection. Default=000, 000=/1, 100=/4,  110=/8, 111=/16 */

/* REG_HS_PLL_I2S_CTRL1				0x0084 */
#define BIT_PLL_SHIFT_I2S_XTAL_SEL					28
#define BIT_PLL_MASK_I2S_XTAL_SEL					0x0F	/* R/W	0		"xtal selection, Default=40MHz 0000: 40MHz,     0001: 25MHz,0010: 13MHz,     0011: 19.2MHz0100: 20MHz,     0101: 26MHz,0110: 38.4MHz,  0111: 17.664MHz1000: 16MHz,     1001: 14.138MHz1010: 12MHz,     1011:  52MHz1100: 48MHz,     1101: 27MHz,1110: 24MHz,     1111: none   " */
#define BIT_PLL_SHIFT_I2S_FREQ_SEL					24
#define BIT_PLL_MASK_I2S_FREQ_SEL					0x0F	/* R/W	0101		output clk selection, Default=98.304MHz/24.576MHz */
#define BIT_PLL_SHIFT_I2S_TBASE_FREQ_SEL			20
#define BIT_PLL_MASK_I2S_TBASE_FREQ_SEL			0x0F	/* R/W	1000		for output clk step up or step down */
#define BIT_PLL_SHIFT_I2S_STEP_FREQ_SEL			16
#define BIT_PLL_MASK_I2S_STEP_FREQ_SEL			0x0F	/* R/W	0101		step size selection for switching freq. */
#define BIT_PLL_I2S_TRIG_RREQ_EN					BIT(15)	/* R/W	0		freq. step up or step down enable */
#define BIT_PLL_SHIFT_I2S_BB_DBG_SEL_AFE_SDM		11
#define BIT_PLL_MASK_I2S_BB_DBG_SEL_AFE_SDM		0x0F	/* R/W	0 */
#define BIT_PLL_SHIFT_I2S_DIVN_SDM				5
#define BIT_PLL_MASK_I2S_DIVN_SDM					0x3F	/* R/W	011100	*/
#define BIT_PLL_I2S_POW_SDM_FCODE				BIT(0)

/* REG_HS_PLL_I2S_CTRL2				0x0088 */
#define BIT_PLL_SHIFT_I2S_SSC_STEP_SEL			19
#define BIT_PLL_MASK_I2S_SSC_STEP_SEL				0x1FFF	/* R/W	0		ssc step size selection. */
#define BIT_PLL_SHIFT_I2S_SSC_FREQ_SEL			6
#define BIT_PLL_MASK_I2S_SSC_FREQ_SEL				0x1FFF	/* R/W	0		ssc freq selection */
#define BIT_PLL_I2S_SSC_EN							BIT(5)	/* R/W	0		ssc enable. Default=0 */

/* REG_HS_PLL_I2S_CTRL3				0x008C */				
#define BIT_PLL_SHIFT_I2S_SDM_FOF					19
#define BIT_PLL_MASK_I2S_SDM_FOF					0x1FFF	/* R/W	0		F code, feedback divider number 1for 1/8*1/2^13 */
#define BIT_PLL_SHIFT_I2S_SDM_FON					16
#define BIT_PLL_MASK_I2S_SDM_FON					0x07	/* R/W	0		N code, feedback divider number 1 for 1/8 */
#define BIT_PLL_I2S_SDM_ORDER_SEL					BIT(15)	/* R/W	0		SDM order: 0:2nd order, 1:3rd order */

/* REG_HS_PLL_PCM_CTRL0				0x0090 */
#define BIT_PLL_PCM_POWCUT_EN				BIT(31)
#define BIT_PLL_PCM_DIV_EN					BIT(30)
#define BIT_PLL_PCM_EN						BIT(29)
#define BIT_PLL_PCM_CLK_EN					BIT(8)
#define BIT_PLL_PCM_DIV2_EN				BIT(7)

/* REG_HS_PLL_PCM_CTRL1				0x0094 */
#define BIT_PLL_PCM_TRIG_RREQ_EN					BIT(15)	/* R/W	0		freq. step up or step down enable */
#define BIT_PLL_SHIFT_PCM_DIVN_SDM				5
#define BIT_PLL_MASK_PCM_DIVN_SDM					0x3F	/* R/W	011100	*/
#define BIT_PLL_PCM_POW_SDM_FCODE				BIT(0)

/* REG_HS_PLL_PCM_CTRL3				0x009C */	
#define BIT_PLL_SHIFT_PCM_SDM_FOF					19
#define BIT_PLL_MASK_PCM_SDM_FOF					0x1FFF	/* R/W	0		F code, feedback divider number 1for 1/8*1/2^13 */
#define BIT_PLL_SHIFT_PCM_SDM_FON					16
#define BIT_PLL_MASK_PCM_SDM_FON					0x07	/* R/W	0		N code, feedback divider number 1 for 1/8 */

/* REG_HS_PLL_TEST					0x00A0 */
#define BIT_PLL_PCM_RDY						BIT(30) /* PCM PLL ready(45.1584MHz) */
#define BIT_PLL_I2S_RDY						BIT(29) /* I2S PLL ready(98.304MHz) */

/*REG_HS_WAKE_EVENT_MSK0   0x120*/
#define BIT_HS_WEVT_BT_MSK				BIT(24) 	/* [24]	R/W		"1: enable BT Wakeup event; 0: disable TIMER wakeup event" */
#define BIT_HS_WEVT_UART_MSK			BIT(20) 	/* [20]	R/W		"1: enable UART Wakeup event; 0: disable UART wakeup event" */
#define BIT_HS_WEVT_USB_MSK			BIT(16) 	/* [16]	R/W		"1: enable USB Wakeup event; 0: disable UART wakeup event" */
#define BIT_HS_WEVT_SDIO_MSK			BIT(14) 	/* [14]	R/W		"1: enable SDIO Wakeup event; 0: disable UART wakeup event" */
#define BIT_HS_WEVT_TIMER_MSK			BIT(1) 	/* [1]	R/W		"1: enable TIMER Wakeup event; 0: disable TIMER wakeup event" */

/*REG_HS_WAKE_EVENT_STATUS0   0x124*/
#define BIT_HS_WEVT_BT_STATUS			BIT(24) 	/* [24]	R/W		"1: indicate BT Wakeup event" */
#define BIT_HS_WEVT_UART_STATUS		BIT(20) 	/* [20]	R/W		"1: indicate UART Wakeup event" */
#define BIT_HS_WEVT_USB_STATUS		BIT(16) 	/* [16]	R/W		"1: indicate USB Wakeup event" */
#define BIT_HS_WEVT_SDIO_STATUS		BIT(14) 	/* [14]	R/W		"1: indicate SDIO Wakeup event" */
#define BIT_HS_WEVT_TIMER_STATUS		BIT(1) 	/* [1]	R/W		"1: indicate TIMER Wakeup event" */

/* REG_HS_WAKE_EVENT_STATUS0	SW define register 0x012C */
#define BIT_HP_WEVT_MISC_STS			BIT(7) /* [7]	R/W	0		1: Indicate MISC Wakeup  event  */
#define BIT_HP_WEVT_CAPTOUCH_STS		BIT(6) /* [6]	R/W	0		1: Indicate captouch Wakeup  event  */
#define BIT_HP_WEVT_KEYSCAN_STS		BIT(5) /* [5]	R/W	0		1: Indicate keyscan Wakeup  event */
#define BIT_HP_WEVT_RTC_STS			BIT(4) /* [4]	R/W	0		1: Indicate RTC Wakeup  event */
#define BIT_HP_WEVT_UART_STS			BIT(3) /* [3] R/W 0		1: Indicate UART  Wakeup  event */
#define BIT_HP_WEVT_WLAN_STS			BIT(2) /* [2] R/W	0		1: Indicate WLAN Wakeup  event */
#define BIT_HP_WEVT_GPIO_STS			BIT(1) /* [1] R/W	0		1: Indicate GPIO Wakeup event */
#define BIT_HP_WEVT_TIMER_STS			BIT(0) /* [0] R/W	0		1: Indicate Timer Wakeup  event; */

/* REG_HS_PERI_FUNC_CTRL1 0x0200 */
#define  BIT_HSYS_PSRAM_FEN				BIT(26)
#define  BIT_HSYS_AC_FEN					BIT(24)
#define  BIT_HS_VNDR_FEN					BIT(20)
#define  BIT_HS_USI_FEN						BIT(19)
#define  BIT_HS_IRDA_FEN					BIT(18)
#define  BIT_HS_IPC_FEN						BIT(17)
#define  BIT_HS_TIMER0_FEN					BIT(16)
#define  BIT_HS_SPI1_FEN					BIT(9)
#define  BIT_HS_SPI0_FEN					BIT(8)
#define  BIT_HS_UART1_FEN_FUN				BIT(3)
#define  BIT_HS_UART1_FEN_GLB				BIT(2)
#define  BIT_HS_UART0_FEN_FUN				BIT(1)
#define  BIT_HS_UART0_FEN_GLB				BIT(0)

/* REG_HS_PERI_FUNC_CTRL2 0x0204 */
#define  BIT_HSYS_BT_FEN					BIT(24)
#define  BIT_HSYS_WLAN_FEN_AXIF			BIT(16)
#define  BIT_HSYS_GDMA0_FEN				BIT(9)
#define  BIT_HSYS_LCDC_FEN					BIT(8)
#define  BIT_HSYS_I2S0_FEN					BIT(2)
#define  BIT_HSYS_IPSEC_FEN					BIT(1)
#define  BIT_HSYS_LX1BUS_FEN				BIT(0)

/* REG_HS_PERI_FUNC_CTRL3 0x0208 */
#define  BIT_HSYS_SPORT_FEN				BIT(19)
#define  BIT_HSYS_USBOTG_FEN				BIT(16)
#define  BIT_HSYS_SDH_FEN_SCKGEN			BIT(9)
#define  BIT_HSYS_SDH_FEN					BIT(8)
#define  BIT_HSYS_SDD_FEN_OFF				BIT(1)
#define  BIT_HSYS_SDD_FEN_ON				BIT(0)


/* REG_HS_PERI_CLK_CTRL1 0x0210 */
#define  BIT_MASK_HSYS_PSRAM_CKSL			0x03	/* [28:27]	R/W	0		"PSRAM PHY Clock sel2'b00: 100MHz2'b01: 133.3MHz2'b10/2'b11:   200MHz" */
#define  BIT_SHIFT_HSYS_PSRAM_CKSL		27
#define  BIT_SHIFT_HSYS_PSRAM_CKSL_100	(0 << BIT_SHIFT_HSYS_PSRAM_CKSL)
#define  BIT_SHIFT_HSYS_PSRAM_CKSL_133	(1 << BIT_SHIFT_HSYS_PSRAM_CKSL)
#define  BIT_SHIFT_HSYS_PSRAM_CKSL_200	(3 << BIT_SHIFT_HSYS_PSRAM_CKSL)

#define  BIT_HSYS_PSRAM_CKE				BIT(26)
#define  BIT_HSYS_AC_CK						BIT(24)
#define  BIT_HS_VNDR_CKE					BIT(20)
#define  BIT_HS_USI_CKE						BIT(19)
#define  BIT_HS_IRDA_CKE					BIT(18)
#define  BIT_HS_IPC_CKE						BIT(17)
#define  BIT_HS_TIMER0_CKE					BIT(16)
#define  BIT_HS_SPI1_CKE					BIT(9)
#define  BIT_HS_SPI0_CKE					BIT(8)
#define  BIT_SHIFT_HSUART0_SCLK_SEL		4
#define  BIT_MASK_HSUART0_SCLK_SEL		0x03	/* [5:4] lp uart1 rx clock, 00: xtal;    01: osc 2m;   10: xtal 2M */
#define  BIT_HS_UART1_CKE					BIT(2)
#define  BIT_HS_UART0_CKE					BIT(0)

/* REG_HS_PERI_CLK_CTRL2 0x0214 */
#define  BIT_HSYS_BT_CKE					BIT(24)
#define  BIT_HSYS_WLAN_CKSL_AXIF			BIT(17) /* "WLAN AXI DAM clock sel0: 50MHz1: 100MHz" */
#define  BIT_HSYS_WLAN_CKE_AXIF			BIT(16)
#define  BIT_HSYS_GDMA0_CKE				BIT(9)
#define  BIT_HSYS_LCDC_CKE					BIT(8)
#define  BIT_HSYS_I2S0_CKE					BIT(2)
#define  BIT_HSYS_IPSEC_CKE					BIT(1)
#define  BIT_HSYS_LX1BUS_CKE				BIT(0)

/* REG_HS_PERI_CLK_CTRL3 0x0218 */
#define  BIT_MASK_HSYS_AC_SPORT_CKSL		0x03	/* R/W	0	BIT_HSYS_AC_SPORT_CKSL	"Audio Codec sport clock selection:2'b00:  Divided clock from 98.304MHz PLL2'b01:  Divided clock from 45.1584MHz PLL2'b10/2'b11:  40M clock " */
#define  BIT_SHIFT_HSYS_AC_SPORT_CKSL		28
#define  BIT_MASK_HSYS_I2S_CLKDIV			0x7f
#define  BIT_SHIFT_HSYS_I2S_CLKDIV			20
#define  BIT_HSYS_SPORT_CKE				BIT(19)
#define  BIT_HSYS_USBOTG_CKE				BIT(16)
#define  BIT_MASK_HSYS_SDH_SCK2_PHSEL	0x07		/* [15:13]	R/W	0	 */
#define  BIT_SHIFT_HSYS_SDH_SCK2_PHSEL	13
#define  BIT_MASK_HSYS_SDH_SCK1_PHSEL	0x07		/* [12:10]	R/W	0	 */
#define  BIT_SHIFT_HSYS_SDH_SCK1_PHSEL	10
#define  BIT_HSYS_SDH_CKE_SCLK				BIT(9)
#define  BIT_HSYS_SDH_CKE_BCLK				BIT(8)
#define  BIT_HSYS_SDD_CKE					BIT(0)

/* REG_HS_BT_CTRL						0x0228 */
#define BIT_HSYS_BT_I2C_TEST_EN			BIT(1)	/* R/W	0		1: Enable I2C for BT TEST */
#define BIT_HSYS_HOST_WAKE_BT				BIT(0)	/* R/W	0		1:  Host wake BT request */

/* #define REG_HS_WL_CTRL						0x0240 */
#define BIT_HS_WLAFE_POD33					BIT(24)	/* R/W	0		0: power down 33 WLAFE */

/* #define REG_HS_OTG_CTRL					0x0244 */
#define BIT_HS_USB2_DIGOTGPADEN		BIT(28)	/* R/W	1		1: Enable USB OTG PAD shared as GPIO */
#define BIT_HS_USB_OTGMODE			BIT(27)	/* R/W	0		*/
#define BIT_HS_USB2_DIGPADEN			BIT(26)	/* R/W	1		1: Enable USB analog PAD shared as GPIO */
#define BIT_HS_ISO_USBA_EN				BIT(25)	/* R/W	1		1: enable  usb analogy isolation */
#define BIT_HS_ISO_USBD_EN				BIT(24)	/* R/W	1		1: enable  usb digital isolation */
#define BIT_HS_USBA_LDO_EN				BIT(23)	/* R/W	0		1: enable USB APHY LDO */
#define BIT_USB_IBX2USB_EN				BIT(21)	/* R/W	0		1: enable IBX to USB*/
#define BIT_USB_PDN33					BIT(20)	/* R/W	1		1: power down USB (LDOIO power down share it) */
#define BIT_HS_UABG_EN					BIT(19)	/* R/W	0		1. Enable bandgap */
#define BIT_HS_UAHV_EN					BIT(18)	/* R/W	0		1: USB PHY analog 3.3V power cut enable */
#define BIT_HS_UALV_EN					BIT(17)	/* R/W	0		1: Enable USB APHY function */
#define BIT_HS_USBD_EN					BIT(16)	/* R/W	0		1: Enable USB digital power */
#define BIT_HS_OTG_CLK_EN				BIT(4)	/* R/W	0		1: Enable OTG clock */

/* REG_HS_SDIO_CTRL 0x248 */
#define BIT_HS_SDIOH_PIN_EN			BIT(28)	/* R/W	0		SDIO Host Mode PINMUX enable */
#define BIT_HS_SDIOD_PIN_EN			BIT(5)	/* R/W	0		SDIO Device Mode PINMUX enable (GPIOB_0~5) */
#define BIT_HS_SDIOH_CLK_READY		BIT(24)

/* REG_HS_SPI_CTRL 0x268*/
#define BIT_HS_SPI1_ROLE_SEL				BIT(11)	/* R/W	0		1: master; 0: slave */
#define BIT_HS_SPI0_ROLE_SELECT			BIT(3)	/* R/W	0		1: master; 0: slave */

/* REG_HS_MEM_CTRL0 0x300*/
#define BIT_HSYS_SNPS_DPRAM_TEST1				BIT(22)	/* R/W	0		Vendor Dual-port Memory parameter: TEST1  */
#define BIT_HSYS_SNPS_DPRAM_LS					BIT(21)	/* R/W	0		"Vendor Dual-port Memory light-sleep enable,  1: enable light-sleep 0: Disable"  */
#define BIT_HSYS_SNPS_DPRAM_RME				BIT(20)	/* R/W	0		Vendor Dual-port Memory parameter  */
#define BIT_HSYS_SHIFT_SNPS_DPRAM_RM			16
#define BIT_HSYS_MASK_SNPS_DPRAM_RM			0x0F		/* R/W	0000	Vendor Dual-port Memory parameter  */
#define BIT_HSYS_SHIFT_SNPS_SPRAM_RA			14
#define BIT_HSYS_MASK_SNPS_SPRAM_RA			0x03		/* R/W	00		Vendor single-port Memory parameter  */
#define BIT_HSYS_SHIFT_SNPS_SPRAM_WA			11
#define BIT_HSYS_MASK_SNPS_SPRAM_WA			0x07		/* R/W	100		Vendor single-port Memory parameter  */
#define BIT_HSYS_SHIFT_SNPS_SPRAM_WPS			8
#define BIT_HSYS_MASK_SNPS_SPRAM_WPS			0x07		/* R/W	0		Vendor single-port Memory parameter  */
#define BIT_HSYS_SNPS_SPRAM_TEST1				BIT(6)	/* R/W	0		Vendor single-port Memory parameter: TEST1  */
#define BIT_HSYS_SNPS_SPRAM_RME				BIT(4)	/* R/W	1		Vendor single-port Memory parameter  */
#define BIT_HSYS_SHIFT_SNPS_SPRAM_RM			0
#define BIT_HSYS_MASK_SNPS_SPRAM_RM			0x0F		/* R/W	0011	Vendor single-port Memory parameter  */

/* REG_HS_MEM_CTRL1 0x304*/
#define BIT_HSYS_SNPS_ROM_TEST1				BIT(30)	/* R/W	0		Vendor ROM parameter: TEST1  */
#define BIT_HSYS_SNPS_ROM_LS					BIT(29)	/* R/W	0		"Vendor ROM parameter: LS (light-sleep) 1: Enable ROM light-sleep 0: Disable"  */
#define BIT_HSYS_SNPS_ROM_RME					BIT(28)	/* R/W	1		Vendor ROM parameter  */
#define BIT_HSYS_SHIFT_SNPS_ROM_RM			24
#define BIT_HSYS_MASK_SNPS_ROM_RM			0x0F		/* R/W	0011	Vendor ROM parameter  */
#define BIT_HSYS_SHIFT_RTK_MEM_RM				16
#define BIT_HSYS_MASK_RTK_MEM_RM				0x0F		/* R/W	1000	In-house Memory parameter  */
#define BIT_HSYS_SHIFT_RTK_MEM_RA				12
#define BIT_HSYS_MASK_RTK_MEM_RA				0x03		/* R/W	00		In-house Memory parameter  */
#define BIT_HSYS_SHIFT_RTK_MEM_WA				9
#define BIT_HSYS_MASK_RTK_MEM_WA				0x07		/* R/W	010		In-house Memory parameter  */
#define BIT_HSYS_RTK_MEM_WAE					BIT(8)	/* R/W	0		In-house Memory parameter  */
#define BIT_HSYS_SHIFT_RTK_MEM_SAW			4
#define BIT_HSYS_MASK_RTK_MEM_SAW			0x03		/* R/W	11		In-house Memory parameter  */
#define BIT_HSYS_SHIFT_RTK_MEM_WM				0
#define BIT_HSYS_MASK_RTK_MEM_WM				0x0F		/* R/W	1000	In-house Memory parameter  */

/* REG_HS_MEM_CTRL2 0x308 */
#define BIT_HSYS_RFC_MEM_LS					BIT(31)	/* R/W	0		1: Enable RFC memory enter into light-sleep mode 0: Disable  */
#define BIT_HSYS_RFC_MEM_DS					BIT(30)	/* R/W	0		1: Enable RFC memory enter into deep-sleep mode 0: Disable  */
#define BIT_HSYS_RFC_MEM_SD					BIT(29)	/* R/W	0		1: Shut-down RFC memory 0: Power on RFC memory   */
#define BIT_HSYS_IDCACHE_LS						BIT(28)	/* R/W	0		1: Enable ICACHE/DCACHE enter light-sleep mode 0: Disable  */
#define BIT_HSYS_WL_SHARE_MEM_LS				BIT(26)	/* R/W	0		1: Enable WL shared memory enter into light-sleep mode, only valid when WL memory shared as KM4 RAM 0: Disable  */
#define BIT_HSYS_WL_SHARE_MEM_DS				BIT(25)	/* R/W	0		1: Enable WL shared memory enter into deep-sleep mode, only valid when WL memory shared as KM4 RAM 0: Disable  */
#define BIT_HSYS_WL_SHARE_MEM_SD				BIT(24)	/* R/W	0		1: Shut-down WL shared memory, only valid when WL memory shared as KM4 RAM 0: Power on  WL shared memory, only valid when WL memory shared as KM4 RAM  */
#define BIT_HSYS_BT_SHARE_MEM_LS				BIT(22)	/* R/W	0		1: Enable BT shared memory enter into light-sleep mode, only valid when BT memory shared as KM4 RAM 0: Disable  */
#define BIT_HSYS_BT_SHARE_MEM_DS				BIT(21)	/* R/W	0		1: Enable BT shared memory enter into deep-sleep mode, only valid when BT memory shared as KM4 RAM 0: Disable  */
#define BIT_HSYS_BT_SHARE_MEM_SD				BIT(20)	/* R/W	0		1: Shut-down BT shared memory, only valid when BT memory shared as KM4 RAM 0: Power on  BT shared memory, only valid when BT memory shared as KM4 RAM  */
#define BIT_HSYS_K4RAM2_LS						BIT(18)	/* R/W	0		1: Enable KM4 RAM2 enter into light-sleep mode 0: Disable  */
#define BIT_HSYS_K4RAM2_DS						BIT(17)	/* R/W	0		1: Enable KM4 RAM2 enter into deep-sleep mode 0: Disable  */
#define BIT_HSYS_K4RMA2_SD						BIT(16)	/* R/W	0		1: Shut-down KM4 RAM2 0: Power on  KM4 RAM2  */
#define BIT_HSYS_K4RAM1_LS						BIT(14)	/* R/W	0		1: Enable KM4 RAM1 enter into light-sleep mode 0: Disable  */
#define BIT_HSYS_K4RAM1_DS						BIT(13)	/* R/W	0		1: Enable KM4 RAM1 enter into deep-sleep mode 0: Disable  */
#define BIT_HSYS_K4RMA1_SD						BIT(12)	/* R/W	0		1: Shut-down KM4 RAM1 0: Power on  KM4 RAM1  */
#define BIT_HSYS_K4RAM0_LS						BIT(10)	/* R/W	0		1: Enable KM4 RAM0 enter into light-sleep mode 0: Disable  */
#define BIT_HSYS_K4RAM0_DS						BIT(9)	/* R/W	0		1: Enable KM4 RAM0 enter into deep-sleep mode 0: Disable  */
#define BIT_HSYS_K4RMA0_SD						BIT(8)	/* R/W	0		1: Shut-down KM4 RAM0 0: Power on  KM4 RAM0  */
#define BIT_HSYS_PERI_MEM_LS					BIT(6)	/* R/W	0		1: Enable Peripheral memory enter into light-sleep mode 0: Disable  */
#define BIT_HSYS_PERI_MEM_DS					BIT(5)	/* R/W	0		1: Enable Peripheral memory enter into deep-sleep mode 0: Disable  */
#define BIT_HSYS_PERI_MEM_SD					BIT(4)	/* R/W	0		1: Shut-down Peripheral memory 0: Power on Peripheral memory  */
#define BIT_HSYS_USB_MEM_LS					BIT(2)	/* R/W	0		1: Enable USB memory enter into light-sleep mode 0: Disable  */
#define BIT_HSYS_USB_MEM_DS					BIT(1)	/* R/W	0		1: Enable USB memory enter into deep-sleep mode 0: Disable  */
#define BIT_HSYS_USB_MEM_SD					BIT(0)	/* R/W	0		1: Shut-down USB memory 0: Power on USB memory  */

/* HP Security Register */
#define BIT_SEC_EF_RWFLAG					BIT(31)	/* R/W	0 Write "1" for Program; Write "0" for Read Access */		
#define BIT_SHIFT_SEC_EF_ADDR				8		/* [17:8]	R/W	0		Access Address */
#define BIT_MASK_SEC_EF_ADDR				0x3FF	/* [17:8]	R/W	0		Access Address */
#define BIT_SHIFT_SEC_EF_DATA				0		/* [7:0] R/W	0		Access Data	*/
#define BIT_MASK_SEC_EF_DATA				0xFF	/* [7:0] R/W	0		Access Data	*/

/* HS Secure Boot Status Register */
#define BIT_SECURE_BOOT_DONE				BIT(0)  /* Write Only, Read as 0*/
//================= SYSON Register Address Definition =====================//
#define REG_HS_PWR_ISO_CTRL				0x0000 /*!< 0x00: REG_SYS_PWR_CTRL, 0x02: REG_SYS_ISO_CTRL */
#define REG_HS_PLATFORM_PARA				0x000C
#define REG_HS_RFAFE_IND_VIO1833			0x0020

#define REG_HS_PLL_I2S_CTRL0				0x0080
#define REG_HS_PLL_I2S_CTRL1				0x0084
#define REG_HS_PLL_I2S_CTRL2				0x0088
#define REG_HS_PLL_I2S_CTRL3				0x008C

#define REG_HS_PLL_PCM_CTRL0				0x0090
#define REG_HS_PLL_PCM_CTRL1				0x0094
#define REG_HS_PLL_PCM_CTRL2				0x0098
#define REG_HS_PLL_PCM_CTRL3				0x009C
#define REG_HS_PLL_TEST					0x00A0

#define REG_HS_WAKE_EVENT_MSK0			0x0120
#define REG_HS_WAKE_EVENT_STATUS0		0x0124
#define REG_HS_WAKE_EVENT_MSK1			0x0128
#define REG_HS_WAKE_EVENT_STATUS1		0x012C

/* HP_PER */
#define REG_HS_PERI_FUNC_CTRL1			0x0200
#define REG_HS_PERI_FUNC_CTRL2			0x0204
#define REG_HS_PERI_FUNC_CTRL3			0x0208
#define REG_HS_PERI_FUNC_CTRL4			0x020C
#define REG_HS_PERI_CLK_CTRL1				0x0210
#define REG_HS_PERI_CLK_CTRL2				0x0214
#define REG_HS_PERI_CLK_CTRL3				0x0218
#define REG_HS_BT_CTRL						0x0228
#define REG_HS_WL_CTRL						0x0240
#define REG_HS_OTG_CTRL					0x0244
#define REG_HS_SDIO_CTRL					0x0248

/*SPI Role Select*/
#define REG_HS_SPI_CTRL					0x268

/*HP_MEM_CTRL*/
#define REG_HS_MEM_CTRL0					0x300
#define REG_HS_MEM_CTRL1					0x304
#define REG_HS_MEM_CTRL2					0x308

/* HP Security Register */
#define REG_HS_SEC_EFUSE_CTRL0			0x10000810

/* HS Secure Boot Status Register */
#define REG_HS_SECURE_BOOT_STA		0x0800
#endif //__INC_RTL8721D_HP_SYS_ON_BIT_H

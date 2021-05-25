#ifndef _RTL8721D_SIMULATION_H_
#define _RTL8721D_SIMULATION_H_

/* CPUID */
#define SIMULATION_KM0_CPUID			0		
#define SIMULATION_KM4_CPUID			1

/* REG */
#define KM0_SIMULATION_STAGE_REG		0x144
#define KM4_SIMULATION_STAGE_REG		0x148

/* KM0 STAGE */
#define BIT_KM0_RUN_INTO_FLASH			BIT(0) /* boot into flash */
#define BIT_KM0_IMG1_BSS_CLR				BIT(1) /* img1 bss clear done */
#define BIT_KM0_FLASH_CLK_PLL_EN			BIT(2) /* flash clk change from xtal to pll  */
#define BIT_KM0_FLASH_QIO_EN				BIT(3) /* flash Qaurd IO EN  */
#define BIT_KM0_FLASH_CALI_START			BIT(4) /* flash Calibration start  */
#define BIT_KM0_FLASH_CALI_END			BIT(5) /* flash Calibration end  */
#define BIT_KM0_FUNCTIONS_EN				BIT(6) /* Enable KM0 periperals clock & function ok */
#define BIT_KM0_SYSTIMER_EN				BIT(7) /* Enable KM0 systimer ok  */
#define BIT_KM0_KM4_FREERUN				BIT(8) /* Let KM4 Free run into flash  */
#define BIT_KM0_IMG2_VALID					BIT(9) /* check image2 valid done  */
#define BIT_KM0_IMG2_LOAD					BIT(10) /* image2 load done  */
#define BIT_KM0_TO_IMG2					BIT(11) /* image1 call image2 entry  */
#define BIT_KM0_ENTER_IMG2					BIT(12) /* image2 enter  */
#define BIT_KM0_IMG2_BSS_CLR				BIT(13) /* image2 bss clear done  */
#define BIT_KM0_OSC_CALI_START			BIT(14) /* osc calibration start */
#define BIT_KM0_OSC_CALI_END				BIT(15) /* osc calibration end */
#define BIT_KM0_RTC_INIT_DONE				BIT(16) /* RC init done */
#define BIT_KM0_APP_ENTER					BIT(17) /* app enter */
#define BIT_KM0_MAIN_ENTER					BIT(18) /* main enter */
#define BIT_KM0_WIFIFW_INIT_START			BIT(19) /* wififw init start */
#define BIT_KM0_WIFIFW_INIT_END			BIT(20) /* wififw init end */
#define BIT_KM0_SIMULATION_START			BIT(21) /* simulation start */
#define BIT_KM0_SIMULATION_END			BIT(22) /* simulation end */


/* KM4 STAGE */
#define BIT_KM4_RUN_INTO_FLASH			BIT(0)
#define BIT_KM4_IMG1_BSS_CLR				BIT(1) /* img1 bss clear done */
#define BIT_KM4_IMG2_VALID					BIT(2) /* check image2 valid done  */
#define BIT_KM4_IMG2_LOAD					BIT(3) /* image2 load done  */
#define BIT_KM4_IMG3_LOAD_START			BIT(4) /* img3 load start */
#define BIT_KM4_IMG3_LOAD_END				BIT(5) /* img3 load end */
#define BIT_KM4_TZ_CFG_DONE				BIT(6) /* trustzone config done */
#define BIT_KM4_TO_IMG2					BIT(7) /* image1 call image2 entry  */
#define BIT_KM4_ENTER_IMG2					BIT(8) /* image2 enter  */
#define BIT_KM4_IMG2_BSS_CLR				BIT(9) /* image2 bss clear done  */
#define BIT_KM4_ENTER_IMG3					BIT(10) /* image2 enter  */
#define BIT_KM4_IMG3_BSS_CLR				BIT(11) /* image2 bss clear done  */
#define BIT_KM4_APP_ENTER					BIT(12) /* app enter */
#define BIT_KM4_MAIN_ENTER					BIT(13) /* main enter */
#define BIT_KM4_WIFI_INIT_START			BIT(14) /* wififw init start */
#define BIT_KM4_WIFI_INIT_END				BIT(15) /* wififw init end */
#define BIT_KM4_SIMULATION_START			BIT(16) /* simulation start */
#define BIT_KM4_SIMULATION_END			BIT(17) /* simulation end */

/* FUNCTION */
u32 simulation_stage_set(u32 cpuid, u32 sim_stage_bit);

#endif /* _RTL8721D_SIMULATION_H_ */

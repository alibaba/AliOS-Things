#include <PinNames.h>
#include <pinmap.h>
#include <gpio_api.h>
#include <wifi_wowlan.h>
#include <freertos_pmu.h>
#include <wifi_conf.h>

#define CONFIG_WOWLAN_DEV_NT96658 //build for Nova NT96658
//#define CONFIG_WOWLAN_DEV_OV788     //build for OmniVision OV788

#if defined(CONFIG_WOWLAN_DEV_NT96658) && defined(CONFIG_WOWLAN_DEV_OV788)
#error "CONFIG_WOWLAN_DEV_NT96658 and CONFIG_WOWLAN_DEV_OV788 are mutually exclusive. "
#endif

#ifdef CONFIG_WOWLAN_DEV_NT96658
#define WOW_WIFI_IN_PIN			PE_4 //JTAG pin, so JTAG must be disable before using this pin as wakeup pin
#define WOW_TRIGGER_INTERVAL		500
#elif defined(CONFIG_WOWLAN_DEV_OV788)
#define WOW_WIFI_IN_PIN			PD_5
#define WOW_WLAN_ON_PIN			PB_3
#define WOW_TRIGGER_INTERVAL		200
#else
#error "Either CONFIG_WOWLAN_DEV_NT96658 or CONFIG_WOWLAN_DEV_OV788 should be defined, but not both. "
#endif

//pin assignment for SDIO, default pull high
#define SD_D2	PA_0
#define SD_D3	PA_1
#define SD_CMD	PA_2
#define SD_CLK	PA_3
#define SD_D0	PA_4
#define SD_D1	PA_5
#define SD_CD	PA_6

gpio_t wow_gpio_wifi_in; //WOWLAN WAKEUP TRIGGER PORT
gpio_t wow_gpio_wlan_on; //RECORD WOWLAN STATUS: 1:OFF, 0:ON

int dev_wowlan_init(void){
	WOWLAN_PRINTK("WOWLAN: device init!");

#ifdef CONFIG_WOWLAN_DEV_OV788	       
	// Initial WLAN_ON pin
	gpio_init(&wow_gpio_wlan_on, WOW_WLAN_ON_PIN);
	gpio_dir(&wow_gpio_wlan_on, PIN_OUTPUT); 
	gpio_mode(&wow_gpio_wlan_on, PullNone); 
	gpio_write(&wow_gpio_wlan_on, 1);
#endif

	return 0;
}

int dev_wowlan_enable(void){
	WOWLAN_PRINTK("WOWLAN: device enable!");

	// Init WIFI_IN pin (wakeup pin)
	gpio_init(&wow_gpio_wifi_in, WOW_WIFI_IN_PIN);
	gpio_dir(&wow_gpio_wifi_in, PIN_OUTPUT); 
	gpio_mode(&wow_gpio_wifi_in, PullNone);
	gpio_write(&wow_gpio_wifi_in, 0);
	
#ifdef CONFIG_WOWLAN_DEV_OV788
	gpio_write(&wow_gpio_wlan_on, 0);
#endif

#if CONFIG_WLAN
	wifi_set_power_mode(0xff, 1);
#endif

	return 0;
}

int dev_wowlan_wakeup_process(void){
	WOWLAN_PRINTK("WOWLAN: device wake up!");

#if defined(CONFIG_WOWLAN_DEV_NT96658) || defined(CONFIG_WOWLAN_DEV_OV788)
#if defined(configUSE_WAKELOCK_PMU) && (configUSE_WAKELOCK_PMU == 1)
	//acquire wakelock to keep system awake
	pmu_acquire_wakelock(PMU_SDIO_DEVICE);
#endif	
#endif

#ifdef CONFIG_WOWLAN_DEV_OV788
	//record wowlan status
	gpio_write(&wow_gpio_wlan_on, 1);
#endif

#if defined(CONFIG_WOWLAN_DEV_NT96658)
	//restore SDIO pin status for bus communication
	pin_mode(SD_D0, PullUp);
	pin_mode(SD_D1, PullUp);
	pin_mode(SD_D2, PullUp);
	pin_mode(SD_D3, PullUp);
	pin_mode(SD_CMD, PullUp);
	pin_mode(SD_CLK, PullDown);
#endif

	//send signal to awake host
	gpio_write(&wow_gpio_wifi_in, 0);
	wowlan_mdelay_os(WOW_TRIGGER_INTERVAL);
	gpio_write(&wow_gpio_wifi_in, 1);
	wowlan_mdelay_os(WOW_TRIGGER_INTERVAL);
	gpio_write(&wow_gpio_wifi_in, 0);
	wowlan_mdelay_os(WOW_TRIGGER_INTERVAL);

	return 0;
}

int dev_wowlan_sleep_process(void){

#if defined(CONFIG_WOWLAN_DEV_NT96658)
	//pull control for SDIO pin only when host is already power off 
	if(rtw_wowlan_is_enabled() && (rtw_wowlan_get_wk_reason() == 0)){
		WOWLAN_PRINTK("pull control");
		//configure SDIO pin status for avoiding current leakage 
		pin_mode(SD_D0, PullNone);
		pin_mode(SD_D1, PullNone);
		pin_mode(SD_D2, PullNone);
		pin_mode(SD_D3, PullNone);
		pin_mode(SD_CMD, PullNone);
		pin_mode(SD_CLK, PullNone);
	}
#endif

	return 0;
}

int dev_wowlan_disable(void){
	WOWLAN_PRINTK("WOWLAN: device disable!");

#if CONFIG_WLAN
	wifi_set_power_mode(0xff, 0);
#endif

#ifdef CONFIG_WOWLAN_DEV_OV788
	gpio_write(&wow_gpio_wlan_on, 1);
#endif

	return 0;
}

void dev_wowlan_ops_init(void *dev_ops){
	struct rtw_wowlan_ops *ops = (struct rtw_wowlan_ops *)dev_ops;
	WOWLAN_PRINTK("WOWLAN: device ops init!");
	ops->DevWowlanInit = dev_wowlan_init;
	ops->DevWowlanEnable = dev_wowlan_enable;
	ops->DevWowlanDisable = dev_wowlan_disable;
	ops->DevWowlanWakeUp = dev_wowlan_wakeup_process;
	ops->DevWowlanSleep = dev_wowlan_sleep_process;
}

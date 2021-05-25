#include "FreeRTOS.h"
#include "task.h"
#include <platform/platform_stdlib.h>
#include <platform_opts.h>
#include <wifi_conf.h>
#include <osdep_service.h>
#include <platform_opts_bt.h>
#include <gap_conn_le.h>

#if CONFIG_BT

#if defined(CONFIG_PLATFORM_8710C) && defined(CONFIG_FTL_ENABLED)
#include <ftl_int.h>

const uint8_t ftl_phy_page_num = 3;	/* The number of physical map pages, default is 3: BT_FTL_BKUP_ADDR, BT_FTL_PHY_ADDR1, BT_FTL_PHY_ADDR0 */
const uint32_t ftl_phy_page_start_addr = BT_FTL_BKUP_ADDR;

void app_ftl_init(void)
{
	DBG_INFO_MSG_OFF(_DBG_SPI_FLASH_);
	DBG_ERR_MSG_ON(_DBG_SPI_FLASH_);
	
	ftl_init(ftl_phy_page_start_addr, ftl_phy_page_num);
}
#endif //CONFIG_PLATFORM_8710C && CONFIG_FTL_ENABLED

extern void bt_coex_init(void);
extern void wifi_btcoex_set_bt_on(void);

void bt_example_init_thread(void *param)
{
	T_GAP_DEV_STATE new_state;
	
	/*Wait WIFI init complete*/
	while( !wifi_is_up(RTW_STA_INTERFACE) ) {
		vTaskDelay(1000 / portTICK_RATE_MS);
	}
	wifi_disable_powersave();
	
	/*Init BT*/
#if CONFIG_BT_PERIPHERAL
	ble_app_main();
#endif
#if CONFIG_BT_CENTRAL
	ble_central_app_main();
#endif
#if CONFIG_BT_BEACON
	bt_beacon_app_main();
#endif
#if CONFIG_BT_MESH_PROVISIONER
	ble_app_main();
#endif
#if CONFIG_BT_MESH_DEVICE
	ble_app_main();
#endif
	bt_coex_init();

	/*Wait BT init complete*/
	do {
		vTaskDelay(100 / portTICK_RATE_MS);
		le_get_gap_param(GAP_PARAM_DEV_STATE , &new_state);
	}while(new_state.gap_init_state != GAP_INIT_STATE_STACK_READY);

	/*Start BT WIFI coexistence*/
	wifi_btcoex_set_bt_on();
	
	vTaskDelete(NULL);
}

void bt_example_init()
{
#if ((defined(CONFIG_BT_PERIPHERAL) && CONFIG_BT_PERIPHERAL) || \
	(defined(CONFIG_BT_CENTRAL) && CONFIG_BT_CENTRAL) || \
	(defined(CONFIG_BT_BEACON) && CONFIG_BT_BEACON) || \
	(defined(CONFIG_BT_MESH_PROVISIONER) && CONFIG_BT_MESH_PROVISIONER) || \
	(defined(CONFIG_BT_MESH_DEVICE) && CONFIG_BT_MESH_DEVICE))
	
	if(xTaskCreate(bt_example_init_thread, ((const char*)"bt example init"), 1024, NULL, tskIDLE_PRIORITY + 5 + PRIORITIE_OFFSET, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate(bt example init) failed", __FUNCTION__);
#endif // BT example flags
}

#endif //CONFIG_BT

#include <sys/unistd.h>
#include <sys/errno.h>

#include <stdarg.h>

#include "aos/hal/flash.h"
#include "aos/hal/uart.h"
#include "aos/hal/wifi.h"
#include "CheckSumUtils.h"
#include <wifi/wifi_conf.h>
#include <wifi/wifi_util.h>
#include <wifi/wifi_ind.h>

static uart_dev_t qc_uart;
extern uart_dev_t uart_0;

#ifndef SERIAL_NUMBER
#define SERIAL_NUMBER       "UNDF.0000.0000"
#endif

#ifndef BOOT_VERSION
#define BOOT_VERSION "1.0.0"
#endif

#define WEAK             __attribute__((weak))

WEAK char *get_sn(void)
{
    return SERIAL_NUMBER;
}

void qc_printf(const char *fmt, ...)
{
    va_list ap;
    char string[128];
    
    va_start(ap, fmt);
    vsprintf(string, fmt, ap);
    string[127] = 0;
    //hal_uart_send(&qc_uart, string, strlen(string), 0);
    hal_uart_send(&uart_0, string, strlen(string), 0);
    va_end(ap);
}

static void qc_scan_completed_event(hal_wifi_module_t *m,
                                        hal_wifi_scan_result_t *result,
                                        void *arg)
{
    int i;
    char ssid[33];

    ssid[32] = 0;
    if (result->ap_num <= 0) {
        qc_printf("Scan AP Fail");
        return;
    }
    qc_printf( "Scan AP Success:\r\n" );
    for (i = 0; i < (result->ap_num); i++) {
        memcpy(ssid, result->ap_list[i].ssid, 32);// fix ssid len is 32 bytes.
        qc_printf("  SSID: %s, RSSI: %d\r\n", ssid, result->ap_list[i].ap_power);
    }
}



typedef struct _scan_rst_t_ {
	rtw_scan_result_t result;
	
	struct _scan_rst_t_ *next;
}mxchip_scan_rst_t;

static mxchip_scan_rst_t *p_result_head = NULL;

static void free_ap_list(void)
{
    mxchip_scan_rst_t* p, *q;

    p = p_result_head;
    p_result_head = NULL;
    
    while(p != NULL) {
        q = p->next;
        aos_free(p);
        p = q;
    }   
}

static void insert_result_by_rssi(rtw_scan_result_t* result)
{
    mxchip_scan_rst_t *p, *next, *prev;
  
    /* New BSSID - add it to the list */
    p = (mxchip_scan_rst_t*)aos_malloc(sizeof(mxchip_scan_rst_t));
    if (p == NULL) {
		return;
    }
    memcpy(&p->result, result, sizeof(rtw_scan_result_t));
    p->next = NULL;
    if (p_result_head == NULL) {
        p->next = p_result_head;
        p_result_head = p;
        goto DONE;
    }
    
    prev = p_result_head;
    while(prev->next != NULL) {
        prev = prev->next;
    }
    prev->next = p;
DONE:
    return;
}

static rtw_result_t scan_result_handler( rtw_scan_handler_result_t* malloced_scan_result )
{
	if (malloced_scan_result->scan_complete != RTW_TRUE) {
		rtw_scan_result_t* record = &malloced_scan_result->ap_details;
		record->SSID.val[record->SSID.len] = 0; /* Ensure the SSID is null terminated */
        insert_result_by_rssi(record);
	} else{
	    mxchip_scan_rst_t *p;
	    rtw_scan_result_t* result;
	    p = p_result_head;
        if (p == NULL) {
            qc_printf("Scan AP Fail\r\n");
            return RTW_SUCCESS;
        }
	    while (p!=NULL) {
	        result = &p->result;
	        qc_printf("  SSID: %s, RSSI: %d\r\n", result->SSID.val, result->signal_strength);
			p = p->next;
	    }
        free_ap_list();
	}
	return RTW_SUCCESS;
}

static void qc_scan(void)
{
    int ret;
    hal_wifi_module_t *module;
    uint8_t mac[6];
    
    module = hal_wifi_get_default_module();
    hal_wifi_get_mac_addr(module, mac );
    qc_printf( "MAC:%02X-%02X-%02X-%02X-%02X-%02X\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] );

    ret = wifi_scan_networks(scan_result_handler, NULL);

    if (ret != RTW_SUCCESS) {
        qc_printf("Scan AP Fail\r\n");
    }

}

static char *get_bootloader_ver(void)
{
    return BOOT_VERSION;
}

static uint16_t qc_crc(void)
{
    hal_logic_partition_t* part;
    int total_len, len, offset=0;
    uint8_t data[1024];
    CRC16_Context contex;
    uint16_t crc = 0;
    
    CRC16_Init( &contex );
    part = hal_flash_get_info((hal_partition_t)HAL_PARTITION_APPLICATION);
    total_len = part->partition_length;
    
    while(total_len > 0){
        if( 1024 < total_len ){
            len = 1024;
        } else {
            len = total_len;
        }
        hal_flash_read( HAL_PARTITION_APPLICATION, &offset, data , len);

        total_len -= len;

        CRC16_Update( &contex, data, len );
    }
    CRC16_Final( &contex, &crc );
}

void qc_test(void)
{
    uint8_t mac[6];
    uint32_t prov_res, ret;

    printf( "\r\n" );
    qc_uart.port                = 4;
    qc_uart.config.baud_rate    = 921600;
    qc_uart.config.data_width   = DATA_WIDTH_8BIT;
    qc_uart.config.parity       = NO_PARITY;
    qc_uart.config.stop_bits    = STOP_BITS_1;
    qc_uart.config.flow_control = FLOW_CONTROL_DISABLED;

    //hal_uart_init(&qc_uart);
    qc_printf( "==== MXCHIP Manufacture Test ====\r\n" );
    qc_printf( "Serial Number: %s\r\n", get_sn() );
    qc_printf( "App CRC: %04X\r\n", qc_crc() );
    qc_printf( "Bootloader Version: %s\r\n", get_bootloader_ver() );
// GPIO test
    qc_test_gpio();
// BLE test
    bt_get_mac_address(mac);
    qc_printf( "Local Bluetooth Address: %02X-%02X-%02X-%02X-%02X-%02X\r\n",
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] );

    qc_scan();

    while(1) {// dead loop, DONOT exit QC mode
        aos_msleep(1000);
        hal_wdg_reload(NULL);
    }
}


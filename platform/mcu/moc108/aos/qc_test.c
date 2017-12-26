#include <sys/unistd.h>
#include <sys/errno.h>
#include "hal/soc/soc.h"
#include <stdarg.h>
#include <hal/hal.h>
#include "CheckSumUtils.h"
#include "platform_toolchain.h"

static uart_dev_t qc_uart;

#ifndef SERIAL_NUMBER
#define SERIAL_NUMBER       "UNDF.0000.0000"
#endif

WEAK char *get_sn(void)
{
    return SERIAL_NUMBER;
}

static void qc_printf(const char *fmt, ...)
{
    va_list ap;
    char string[128];
    
    va_start(ap, fmt);
    vsprintf(string, fmt, ap);
    string[127] = 0;
    hal_uart_send(&qc_uart, string, strlen(string), 0);
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

static const hal_wifi_event_cb_t g_wifi_hal_event = {
    .connect_fail        = NULL,
    .ip_got              = NULL,
    .stat_chg            = NULL,
    .scan_compeleted     = qc_scan_completed_event,
    .scan_adv_compeleted = NULL,
    .para_chg            = NULL,
    .fatal_err           = NULL,
};

static void qc_scan(void)
{
    hal_wifi_module_t *module;
    uint8_t mac[6];
    
    module = hal_wifi_get_default_module();
    hal_wifi_install_event(module, &g_wifi_hal_event);

    
    hal_wifi_get_mac_addr(module, mac );
    qc_printf( "MAC:%02X-%02X-%02X-%02X-%02X-%02X\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] );
    hal_wifi_start_scan(module);
}

static char *get_bootloader_ver(void)
{
  static char boot_ver[64];
  uint16_t reg_status;
  uint8_t blocks;

  snprintf(boot_ver, sizeof(boot_ver)-3, "MOC108-%s", (const char *)0x00000020);

  flash_ctrl(0xe240000 + 6, &reg_status);
  blocks = (reg_status >> 2) & 0x0F;
  if(blocks != 1)
  {
    strcat(boot_ver, "-x");
  }
  return boot_ver;
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
    
    qc_uart.port                = 1;
    qc_uart.config.baud_rate    = 921600;
    qc_uart.config.data_width   = DATA_WIDTH_8BIT;
    qc_uart.config.parity       = NO_PARITY;
    qc_uart.config.stop_bits    = STOP_BITS_1;
    qc_uart.config.flow_control = FLOW_CONTROL_DISABLED;

    hal_uart_init(&qc_uart);

    qc_printf( "==== MXCHIP Manufacture Test ====\r\n" );
    qc_printf( "Serial Number: %s\r\n", get_sn() );
    qc_printf( "App CRC: %04X\r\n", qc_crc() );
    qc_printf( "Bootloader Version: %s\r\n", get_bootloader_ver() );
    ret = prov_get_prov_res(&prov_res);
    if ((ret != 0) || (prov_res != 0)) {
        qc_printf("ID2 FAIL\r\n", ret, prov_res);
    } else {
        qc_printf("ID2 PASS");
    }

    qc_scan();

    while(1) // dead loop, DONOT exit QC mode
        aos_msleep(10000);
}


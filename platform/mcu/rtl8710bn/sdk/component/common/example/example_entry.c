/******************************************************************************
 *
 * Copyright(c) 2007 - 2015 Realtek Corporation. All rights reserved.
 *
 *
 ******************************************************************************/
#include <platform_opts.h>
#include "main.h"

#if ATCMD_VER == ATVER_2
#include "flash_api.h"
#include "device_lock.h"
#endif
   
#if CONFIG_EXAMPLE_MDNS
#include <mdns/example_mdns.h>
#endif

#if CONFIG_EXAMPLE_MCAST
#include <mcast/example_mcast.h>
#endif

#if CONFIG_EXAMPLE_XML
#include <xml/example_xml.h>
#endif

#if CONFIG_EXAMPLE_SOCKET_SELECT
#include <socket_select/example_socket_select.h>
#endif

#if CONFIG_EXAMPLE_NONBLOCK_CONNECT
#include <nonblock_connect/example_nonblock_connect.h>
#endif

#if CONFIG_EXAMPLE_SOCKET_TCP_TRX
#include <socket_tcp_trx/example_socket_tcp_trx.h>
#endif

#if CONFIG_EXAMPLE_SSL_DOWNLOAD
#include <ssl_download/example_ssl_download.h>
#endif

#if CONFIG_EXAMPLE_HTTP_DOWNLOAD
#include <http_download/example_http_download.h>
#endif

#if CONFIG_EXAMPLE_HTTPC
#include <httpc/example_httpc.h>
#endif

#if CONFIG_EXAMPLE_HTTPD
#include <httpd/example_httpd.h>
#endif

#if CONFIG_EXAMPLE_TCP_KEEPALIVE
#include <tcp_keepalive/example_tcp_keepalive.h>
#endif

#if CONFIG_EXAMPLE_SNTP_SHOWTIME
#include <sntp_showtime/example_sntp_showtime.h>
#endif

#if CONFIG_EXAMPLE_PPPOE
#include <pppoe/example_pppoe.h>
#endif

#if CONFIG_EXAMPLE_MEDIA_SIGNLE_STREAM
#include <media_single_stream/example_media_ss.h>
#endif

#if CONFIG_EXAMPLE_MEDIA_MULTI_STREAM
#include <media_multi_stream/example_media_ms.h>
#endif

#if CONFIG_EXAMPLE_MEDIA_AUDIO_FROM_RTP
#include <media_audio_from_rtp/example_media_audio_from_rtp.h>
#endif

#if CONFIG_EXAMPLE_AUDIO_MP3
#include <audio_mp3/example_audio_mp3.h>
#endif

#if CONFIG_EXAMPLE_GOOGLE_NEST
#include <googlenest/example_google.h>  
#define FromDevice            1
#define ToDevice     		2 
#define BOTH     		3 
#define TYPE         "ToDevice"
#endif   

#if CONFIG_EXAMPLE_MJPEG_CAPTURE
#include <mjpeg_capture/example_mjpeg.h>
#endif

#if CONFIG_EXAMPLE_WLAN_FAST_CONNECT
#include <wlan_fast_connect/example_wlan_fast_connect.h>
#endif

#if CONFIG_EXAMPLE_WIGADGET
#include <wigadget/wigadget.h>
#endif

#if CONFIG_EXAMPLE_MQTT
#include <mqtt/example_mqtt.h>
#endif

#if CONFIG_EXAMPLE_FATFS
#include <fatfs/example_fatfs.h>
#endif

#if CONFIG_EXAMPLE_DCT
#include <dct/example_dct.h>
#endif

#if CONFIG_EXAMPLE_INIC_GSPI_HOST
#include <inic_gspi/example_inic_gspi.h>
#endif

#if CONFIG_EXAMPLE_ARDUINO_WIFI
#include <arduino_wifi/ard_spi.h>
#endif

#if CONFIG_EXAMPLE_GET_BEACON_FRAME
#include <get_beacon_frame/example_get_beacon_frame.h>
#endif

#if CONFIG_EXAMPLE_USB_MASS_STORAGE
#include <usb_mass_storage/example_usb_msc.h>
#endif

#if CONFIG_EXAMPLE_EAP
#include <eap/example_eap.h>
#endif

#if CONFIG_EXAMPLE_AJ_BASIC
#include <alljoyn/example_aj_basic.h>
#endif 

#if CONFIG_EXAMPLE_AJ_AMEBA_LED
#include <alljoyn/example_aj_ameba_led.h>
#endif 

#if CONFIG_EXAMPLE_WEBSOCKET
#include <websocket/example_wsclient.h>
#endif

#if CONFIG_EXAMPLE_WLAN_SCENARIO
#include <wlan_scenario/example_wlan_scenario.h>
#endif

#if CONFIG_EXAMPLE_BCAST
#include <bcast/example_bcast.h>
#endif

#if CONFIG_EXAMPLE_AUDIO
#include <audio/example_audio.h>
#endif

#if CONFIG_EXAMPLE_HIGH_LOAD_MEMORY_USE
#include <high_load_memory_use/example_high_load_memory_use.h>
#endif

#if CONFIG_EXAMPLE_WIFI_MAC_MONITOR
#include <wifi_mac_monitor/example_wifi_mac_monitor.h>
#endif

#if CONFIG_EXAMPLE_RARP
#include <rarp/example_rarp.h>
#endif

#if CONFIG_EXAMPLE_SSL_SERVER
#include <ssl_server/example_ssl_server.h>
#endif

#if CONFIG_EXAMPLE_OTA_HTTP
#include <ota_http/example_ota_http.h>
#endif

#if CONFIG_EXAMPLE_AMAZON_AWS_IOT
#include <amazon_awsiot/example_amazon_awsiot.h>
#endif

/*
	Preprocessor of example
*/
void pre_example_entry(void)
{
#if ATCMD_VER == ATVER_2
	flash_t flash;
	struct wlan_fast_reconnect read_data = {0};
	device_mutex_lock(RT_DEV_LOCK_FLASH);
	flash_stream_read(&flash, FAST_RECONNECT_DATA, sizeof(struct wlan_fast_reconnect), (u8 *) &read_data);
	device_mutex_unlock(RT_DEV_LOCK_FLASH);
#endif

#if CONFIG_EXAMPLE_WLAN_FAST_CONNECT
#if ATCMD_VER == ATVER_2
	if(read_data.enable == 1)
#endif
	{
		example_wlan_fast_connect();
	}
#endif
#if CONFIG_JD_SMART
	example_jdsmart_init();
#endif
#if CONFIG_EXAMPLE_UART_ADAPTER
	example_uart_adapter_init();
#endif
#if CONFIG_EXAMPLE_ARDUINO_WIFI
    /*To ensure the application has enough heap size, please goto FreeRTOSConfig.h to change configTOTAL_HEAP_SIZE at least to 80*1024 */
    example_arduino_wifi_init();
#endif

#if defined(FREERTOS_PMU_TICKLESS_PLL_RESERVED) && (FREERTOS_PMU_TICKLESS_PLL_RESERVED==1)
    pmu_set_pll_reserved(1);
#endif

#if (configGENERATE_RUN_TIME_STATS == 1)
    pmu_enable_wakelock_stats(1);
#endif
}

/*
  	All of the examples are disabled by default for code size consideration
   	The configuration is enabled in platform_opts.h
*/
void example_entry(void)
{
#if (CONFIG_EXAMPLE_MDNS && !CONFIG_EXAMPLE_UART_ADAPTER) 
	example_mdns();
#endif

#if CONFIG_EXAMPLE_MCAST
	example_mcast();
#endif

#if CONFIG_EXAMPLE_XML
	example_xml();
#endif

#if CONFIG_EXAMPLE_SOCKET_SELECT
	example_socket_select();
#endif

#if CONFIG_EXAMPLE_NONBLOCK_CONNECT
	example_nonblock_connect();
#endif

#if   CONFIG_EXAMPLE_SOCKET_TCP_TRX == 1
	example_socket_tcp_trx_1();
#elif CONFIG_EXAMPLE_SOCKET_TCP_TRX == 2
	example_socket_tcp_trx_2();
#endif

#if CONFIG_EXAMPLE_SSL_DOWNLOAD
	example_ssl_download();
#endif

#if CONFIG_EXAMPLE_HTTP_DOWNLOAD
	example_http_download();
#endif

#if CONFIG_EXAMPLE_HTTPC
	example_httpc();
#endif

#if CONFIG_EXAMPLE_HTTPD
	example_httpd();
#endif

#if CONFIG_EXAMPLE_TCP_KEEPALIVE
	example_tcp_keepalive();
#endif

#if CONFIG_EXAMPLE_SNTP_SHOWTIME
	example_sntp_showtime();
#endif

#if CONFIG_EXAMPLE_PPPOE
	example_pppoe();
#endif

#if CONFIG_EXAMPLE_MEDIA_SS
	example_media_ss();
#endif
	
#if CONFIG_EXAMPLE_MEDIA_MS
	example_media_ms();
#endif   

#if CONFIG_EXAMPLE_MEDIA_AUDIO_FROM_RTP
        example_media_audio_from_rtp();
#endif     
        
#if CONFIG_EXAMPLE_AUDIO_MP3
	example_audio_mp3();
#endif     
#if CONFIG_EXAMPLE_GOOGLE_NEST
	example_google(TYPE);
#endif

#if CONFIG_UART_UPDATE
	example_uart_update();
#endif  

#if CONFIG_EXAMPLE_WIGADGET
    /*To ensure the application has enough heap size, please goto FreeRTOSConfig.h to change configTOTAL_HEAP_SIZE at least to 115*1024 */
    example_wigadget();         
#endif

#if CONFIG_EXAMPLE_MQTT
	example_mqtt();
#endif

#if CONFIG_QQ_LINK
	example_qq_link();
#endif

#if CONFIG_JOYLINK
	example_joylink();
#endif

#if CONFIG_AIRKISS_CLOUD
	example_airkiss_cloud();
#endif

#if CONFIG_EXAMPLE_WIFI_MAC_MONITOR
	example_wifi_mac_monitor();
#endif

#if CONFIG_EXAMPLE_HTTP_CLIENT        
        example_http_client();
#endif
        
#if CONFIG_JOINLINK
	example_joinlink();
#endif

#if CONFIG_EXAMPLE_GET_BEACON_FRAME
	example_get_beacon_frame();
#endif

#if CONFIG_EXAMPLE_FATFS
	example_fatfs();
#endif

#if CONFIG_EXAMPLE_DCT
	example_dct();
#endif

#if CONFIG_GAGENT
	example_gagent();
#endif

#if CONFIG_EXAMPLE_INIC_GSPI_HOST
	example_inic_gspi();
#endif

#if CONFIG_EXAMPLE_USB_MASS_STORAGE
	example_mass_storage();
#endif

#if CONFIG_EXAMPLE_UART_ATCMD
	example_uart_atcmd();
#endif

#if CONFIG_EXAMPLE_SPI_ATCMD
    example_spi_atcmd();
#endif

#if CONFIG_EXAMPLE_MJPEG_CAPTURE
	example_mjpeg();
#endif

// supported eap methods: tls, peap, ttls
// make sure the corresponding config has been turned on before you use it
// e.g. set CONFIG_ENABLE_TLS to 1 if you want to use TLS eap method
#if CONFIG_EXAMPLE_EAP
	example_eap("tls");
	//example_eap("peap");
	//example_eap("ttls");
#endif

#if CONFIG_EXAMPLE_AJ_BASIC
        example_aj_basic();
#endif
           
#if CONFIG_EXAMPLE_AJ_AMEBA_LED
        example_aj_ameba_led();
#endif

#if CONFIG_EXAMPLE_WEBSOCKET
    example_wsclient();
#endif

#if CONFIG_EXAMPLE_WLAN_SCENARIO
	// For Network Scan, Authentication, Mode Switch, Sequence Senario cases
	// Para: S/ A/ M1/ M2/ M3/ M4/ M5/ M6/ M7/ S1/ S2/ S3/ S4
	example_wlan_scenario("S");
#endif
	
#if CONFIG_EXAMPLE_BCAST
	example_bcast();
#endif
#if CONFIG_EXAMPLE_AUDIO
	example_audio();
#endif
#if CONFIG_EXAMPLE_HIGH_LOAD_MEMORY_USE
	example_high_load_memory_use();
#endif

#if CONFIG_EXAMPLE_RARP
	example_rarp();
#endif 

#if CONFIG_EXAMPLE_SSL_SERVER
	example_ssl_server();
#endif  
        
#if CONFIG_EXAMPLE_TIMELAPSE
        example_media_tl();
#endif

#if CONFIG_EXAMPLE_OTA_HTTP
	example_ota_http();
#endif 

#if CONFIG_EXAMPLE_AMAZON_AWS_IOT
    example_amazon_awsiot();
#endif

#if CONFIG_ALINK
	example_alink();
#endif

#if CONFIG_HILINK
example_hilink();
#endif

}

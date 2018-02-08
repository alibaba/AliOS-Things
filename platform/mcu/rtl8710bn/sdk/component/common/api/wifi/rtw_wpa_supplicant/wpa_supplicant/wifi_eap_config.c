#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include "FreeRTOS.h"
//#include "task.h"
#include "main.h"
//#include "queue.h"
#include "utils/os.h"
#include <lwip_netconf.h>
#include <lwip/netif.h>
#include "wifi/wifi_conf.h"
#include <platform/platform_stdlib.h>

#define WLAN0_NAME "wlan0"
#ifndef ENABLE
#define ENABLE	(1)
#endif
#ifndef DISABLE
#define	DISABLE (0)
#endif

u8 eap_phase = 0;
u8 eap_method = 0;

// eap config arguments
char *eap_target_ssid = NULL;
char *eap_identity = NULL;
char *eap_password = NULL;
// if set eap_ca_cert and defined(EAP_SSL_VERIFY_SERVER), client will verify server's cert
const unsigned char *eap_ca_cert = NULL;
// if set eap_client_cert, eap_client_key, and defined(EAP_SSL_VERIFY_CLIENT), client will send its cert to server
const unsigned char *eap_client_cert = NULL;
const unsigned char *eap_client_key = NULL;
char *eap_client_key_pwd = NULL;

void eap_eapol_recvd_hdl(char *buf, int buf_len, int flags, void* handler_user_data);
void eap_eapol_start_hdl(char *buf, int buf_len, int flags, void* handler_user_data);

void set_eap_phase(unsigned char is_trigger_eap){
	eap_phase = is_trigger_eap;
}

int get_eap_phase(void){
	return eap_phase;
}

int get_eap_method(void){
	return eap_method;
}

void reset_config(void){
	eap_target_ssid = NULL;
	eap_identity = NULL;
	eap_password = NULL;
	eap_ca_cert = NULL;
	eap_client_cert = NULL;
	eap_client_key = NULL;
	eap_client_key_pwd = NULL;
}

void judge_station_disconnect(void) 
{
	int mode = 0;
	unsigned char ssid[33];

	wext_get_mode(WLAN0_NAME, &mode);

	switch(mode) {
	case IW_MODE_MASTER:	//In AP mode
		wifi_off();
		vTaskDelay(20);
		wifi_on(RTW_MODE_STA);
		break;
	case IW_MODE_INFRA:		//In STA mode
		if(wext_get_ssid(WLAN0_NAME, ssid) > 0)
			wifi_disconnect();
	}	
}

void eap_disconnected_hdl(char *buf, int buf_len, int flags, void* handler_user_data){
//	printf("disconnected\n");
	wifi_unreg_event_handler(WIFI_EVENT_EAPOL_RECVD, eap_eapol_recvd_hdl);
	wifi_unreg_event_handler(WIFI_EVENT_DISCONNECT, eap_disconnected_hdl);	
	eap_peer_unregister_methods();
	eap_sm_deinit();
	//reset_config();
}

/*
void eap_config(void){
	eap_target_ssid = "Test_eap";
	eap_identity = "guest2";
	eap_password = "test2";

	eap_client_cert = \
"-----BEGIN CERTIFICATE-----\r\n" \
"MIIC9zCCAd8CAQMwDQYJKoZIhvcNAQEEBQAwgZMxCzAJBgNVBAYTAkZSMQ8wDQYD\r\n" \
"VQQIEwZSYWRpdXMxEjAQBgNVBAcTCVNvbWV3aGVyZTEVMBMGA1UEChMMRXhhbXBs\r\n" \
"ZSBJbmMuMSAwHgYJKoZIhvcNAQkBFhFhZG1pbkBleGFtcGxlLmNvbTEmMCQGA1UE\r\n" \
"AxMdRXhhbXBsZSBDZXJ0aWZpY2F0ZSBBdXRob3JpdHkwHhcNMTYwMzE1MDgwNzEx\r\n" \
"WhcNMTcwMzE1MDgwNzExWjBzMQswCQYDVQQGEwJGUjEPMA0GA1UECBMGUmFkaXVz\r\n" \
"MRUwEwYDVQQKEwxFeGFtcGxlIEluYy4xGjAYBgNVBAMUEXVzZXIyQGV4YW1wbGUu\r\n" \
"Y29tMSAwHgYJKoZIhvcNAQkBFhF1c2VyMkBleGFtcGxlLmNvbTCBnzANBgkqhkiG\r\n" \
"9w0BAQEFAAOBjQAwgYkCgYEAqESlV4OYfBcIgZ+Cs8mWpiBjhvKoa0/kIe7saqhC\r\n" \
"e5q4snox0jdkUpLcc4vOs3vQ7ZGnimqTltA9oF6XNUzTWW4vlJTKEfrCWK085l7c\r\n" \
"DHFvHavH3E6vuP71lI7jq4PLXbo2TvZK+uBul4ozjzVWihaZBtz8eLHq446h/D/p\r\n" \
"kzkCAwEAATANBgkqhkiG9w0BAQQFAAOCAQEAAfhVAIkNdeeUNJud720uUHVnIcxz\r\n" \
"GXWI+Svi1qchuTEnRNhLwXmnE+A0WWSHyfdR6FvzdT3xtz3K50iOif8jY2gCGkSK\r\n" \
"8RjKr97228SwbrGO9y9+dYIjH1uz9cBpoVKcpzdsWpKObrDPDYyReHSWo99jM2+O\r\n" \
"vfJxnBw4PLiBj7Q0/dpd6o4JXyp7Cxa0mB4/+cZqjCzzuKfuK3WP7j6laMCV6mg4\r\n" \
"wRZ528IdwDqB7OOqsDm1PVQM8vzny9PM6ikWUCRTVNQJN8RDLkrHR3FRjy15YLdt\r\n" \
"yOfDqVnT/z0wGBaxnNziSJjqPGHPpRi4bJFGXwXOhtknKmciKzfj9/npoQ==\r\n" \
"-----END CERTIFICATE-----\r\n";

	eap_client_key = \
"-----BEGIN RSA PRIVATE KEY-----\r\n" \
"MIICXQIBAAKBgQCoRKVXg5h8FwiBn4KzyZamIGOG8qhrT+Qh7uxqqEJ7mriyejHS\r\n" \
"N2RSktxzi86ze9DtkaeKapOW0D2gXpc1TNNZbi+UlMoR+sJYrTzmXtwMcW8dq8fc\r\n" \
"Tq+4/vWUjuOrg8tdujZO9kr64G6XijOPNVaKFpkG3Px4serjjqH8P+mTOQIDAQAB\r\n" \
"AoGARI+LyweshssfxSkIKVc3EcNaqi6PHwJzUrw2ChM624AkR1xwllXJg7ehKVdK\r\n" \
"xmjprRLO8CASuL1qjsBb3fTKnBl+sIVxIFS0AI4Y3ri8VUKbangvSsI7pCzAFry7\r\n" \
"p1gmy9WWRV2ZEa+dV8xcrjb3bloT7hcdeLehgBCvExJIQM0CQQDXlSAKdW3AhYyj\r\n" \
"1A+pfyBSGxJbpSwNyyWgwHIHHjxendxmdUbrc8EbAu1eNKbP58TLgdCZsKcMonAv\r\n" \
"MY1Y2/nnAkEAx9CrUaCU8pJqXTRypM5JtexLKnYMJhpnA9uUILBQOq4Oe0eruyF5\r\n" \
"SaSxhyJYXY491ahWYPF0PTb3jkUhoN+l3wJBAJZthjgGDJlEFwjSFkOtYz4nib3N\r\n" \
"GVpeoFj1MBvrazCScpJDz0LIOLzCZCNSFfwIu3dNk+NKMqZMSn+D0h9pD40CQQC5\r\n" \
"K9n4NXaTLbjAU2CC9mE85JPr76XmkcUxwAWQHZTcLH1jJdIyAx1hb+zNLLjzSmRn\r\n" \
"Yi9ae6ibKhtUjyBQ87HFAkA2Bb3z7NUx+AA2g2HZocFZFShBxylACyQkl8FAFZtf\r\n" \
"osudmKdFQHyAWuBMex4tpz/OLTqJ1ecL1JQeC7OvlpEX\r\n" \
"-----END RSA PRIVATE KEY-----\r\n";
	
	eap_ca_cert = \
"-----BEGIN CERTIFICATE-----\r\n" \
"MIIEpzCCA4+gAwIBAgIJAPvZaozpdfjkMA0GCSqGSIb3DQEBCwUAMIGTMQswCQYD\r\n" \
"VQQGEwJGUjEPMA0GA1UECBMGUmFkaXVzMRIwEAYDVQQHEwlTb21ld2hlcmUxFTAT\r\n" \
"BgNVBAoTDEV4YW1wbGUgSW5jLjEgMB4GCSqGSIb3DQEJARYRYWRtaW5AZXhhbXBs\r\n" \
"ZS5jb20xJjAkBgNVBAMTHUV4YW1wbGUgQ2VydGlmaWNhdGUgQXV0aG9yaXR5MB4X\r\n" \
"DTE2MDMxNDExMjU0OVoXDTE2MDQxMzExMjU0OVowgZMxCzAJBgNVBAYTAkZSMQ8w\r\n" \
"DQYDVQQIEwZSYWRpdXMxEjAQBgNVBAcTCVNvbWV3aGVyZTEVMBMGA1UEChMMRXhh\r\n" \
"bXBsZSBJbmMuMSAwHgYJKoZIhvcNAQkBFhFhZG1pbkBleGFtcGxlLmNvbTEmMCQG\r\n" \
"A1UEAxMdRXhhbXBsZSBDZXJ0aWZpY2F0ZSBBdXRob3JpdHkwggEiMA0GCSqGSIb3\r\n" \
"DQEBAQUAA4IBDwAwggEKAoIBAQC9pireu0aCDLNfMaGv3vId7RXjUhQwSK0jV2Oc\r\n" \
"SyvlKWH3P/N+5kLrP2iL6SCzyETVDXZ0vOsAMjcBF0zHp16prXV0d51cTUqeWBb0\r\n" \
"I5UnGxleIuuOfSg8zLUJoBWZPqLv++eZ5WgOKHt7SXocjvg7TU5t/TMB0Y8OCz3H\r\n" \
"CW2vJ/XKMgMA9HDUu4g57cJu88i1JPRpyFaz/HIQBc7+UNb9z+q09uTZKWTmEMqi\r\n" \
"E2U0EEIs7EtbxnOze1/8C4XNlmztrEdwvu6UEBU/TFkUoh9M646NkkBK7wP9n9pv\r\n" \
"T0nPQRJiiCrICzVqUtlEi9lIKpbBSMbQ0KzrGF7lGTgm4rz9AgMBAAGjgfswgfgw\r\n" \
"HQYDVR0OBBYEFIVyecka74kvOKIW0BjlTc/B+a2NMIHIBgNVHSMEgcAwgb2AFIVy\r\n" \
"ecka74kvOKIW0BjlTc/B+a2NoYGZpIGWMIGTMQswCQYDVQQGEwJGUjEPMA0GA1UE\r\n" \
"CBMGUmFkaXVzMRIwEAYDVQQHEwlTb21ld2hlcmUxFTATBgNVBAoTDEV4YW1wbGUg\r\n" \
"SW5jLjEgMB4GCSqGSIb3DQEJARYRYWRtaW5AZXhhbXBsZS5jb20xJjAkBgNVBAMT\r\n" \
"HUV4YW1wbGUgQ2VydGlmaWNhdGUgQXV0aG9yaXR5ggkA+9lqjOl1+OQwDAYDVR0T\r\n" \
"BAUwAwEB/zANBgkqhkiG9w0BAQsFAAOCAQEAZYHM26sxbKOckVqJJ1QY0U2QFlGP\r\n" \
"1GYd8v27znxdnRmSonDvv3GjFfhwoyDk0JUuxkK/33ikCxihrgoO/EQTY9BV2OpW\r\n" \
"qkB1PDtb3i5ZRNvfjmW0pVA4p+GmdTGaEE5pTlcVnorzVrUeFKaZakb+IDFYzmeF\r\n" \
"xp8B3Bb5wvinDligLOaJnSlgS8QeeIab9HZfaVTTuPmVK6zE6D54Y0dJPnykvDdE\r\n" \
"cGN0FC+migfilFjJgkDJ0r78nwes55L8zjoofiZuO03rrHww6ARc3v1jYzAufddk\r\n" \
"QTiZHgjlMQb2XXMmXLn8kBgoDnqkXFNe8j0h8uxIJSrjOoIyn1h1wvX5/w==\r\n" \
"-----END CERTIFICATE-----\r\n";
}
*/

int eap_start(char *method){
#ifdef CONFIG_ENABLE_EAP
	int ret = -1;

	//unsigned long tick1 = xTaskGetTickCount();
	//unsigned long tick2;
	
	if(rltk_wlan_running(WLAN1_IDX)){
		printf("\n\rNot support con-current mode!\n\r");
		return -1;
	}

	judge_station_disconnect();

#if CONFIG_ENABLE_PEAP
	if(strcmp(method,"peap") == 0){
		ret = set_eap_peap_method();
	}
#endif

#if CONFIG_ENABLE_TLS
	if(strcmp(method,"tls") == 0){
		ret = set_eap_tls_method();
	}
#endif

#if CONFIG_ENABLE_TTLS
	if(strcmp(method,"ttls") == 0){
		ret = set_eap_ttls_method();
	}
#endif

	if(ret == -1){
		printf("\r\neap method %s not supported\r\n", method);
		return -1;
	}

	eap_method = get_eap_ctx_method();

	printf("\n==================== %s_start ====================\n", method);

	//eap_config();

	set_eap_phase(ENABLE);
	wifi_reg_event_handler(WIFI_EVENT_EAPOL_START, eap_eapol_start_hdl, NULL);
	wifi_reg_event_handler(WIFI_EVENT_EAPOL_RECVD, eap_eapol_recvd_hdl, NULL);

	

	ret = connect_by_open_system(eap_target_ssid);

#if CONFIG_LWIP_LAYER
		/* Start DHCPClient */
	if(ret == 0)
		LwIP_DHCP(0, DHCP_START);
#endif

	wifi_unreg_event_handler(WIFI_EVENT_EAPOL_START, eap_eapol_start_hdl);

	// for re-authentication when session timeout
	wifi_reg_event_handler(WIFI_EVENT_DISCONNECT, eap_disconnected_hdl, NULL);
	//wifi_unreg_event_handler(WIFI_EVENT_EAPOL_RECVD, eap_eapol_recvd_hdl);

	set_eap_phase(DISABLE);
	
	// eap failed, disconnect
	if(ret != 0){
		judge_station_disconnect();
		eap_disconnected_hdl(NULL, 0, 0, NULL);
		rtw_msleep_os(200);	//wait handler done
		printf("\r\nERROR: connect to AP by %s failed\n", method);
	}

	eap_sm_deinit();
	printf("\n==================== %s_finish ====================\n", method);

	//tick2 = xTaskGetTickCount();
	//printf("\r\nConnected after %dms.\n", (tick2-tick1));
	
	return ret;
#else
	return -1;
#endif
}

static int connect_by_open_system(char *target_ssid)
{
	int retry_count = 0, ret;
	
	if (target_ssid != NULL) {
		while (1) {
			rtw_msleep_os(500);	//wait scan complete.
			ret = wifi_connect(target_ssid,
							RTW_SECURITY_OPEN,
							NULL,
							strlen(target_ssid),
							0,
							0,
							NULL);
			if (ret == RTW_SUCCESS) {
			  	//printf("\r\n[EAP]Associate with AP success\n");
				break;
			}
			if (retry_count == 0) {
				//printf("\r\n[EAP]Associate with AP failed %d\n", ret);
				return -1;
			}
			retry_count --;
			printf("Retry connection...\n");

			judge_station_disconnect();
			set_eap_phase(ENABLE);
		}
	} else {
		printf("\r\n[EAP]Target SSID is NULL\n");
		return -1;
	}

	return 0;
}

static void eap_autoreconnect_thread(void *method)
{
	eap_start((char*)method);
	vTaskDelete(NULL);
}

void eap_autoreconnect_hdl(u8 method_id){
#ifdef CONFIG_ENABLE_EAP	
	char *method;
	switch(method_id){
		case 25: // EAP_TYPE_PEAP
			method = "peap";
			break;
		case 13: // EAP_TYPE_TLS
			method = "tls";
			break;
		case 21: // EAP_TYPE_TTLS
			method = "ttls";
			break;
		default:
			printf("invalid eap method\n");
			return;
	}
	if(xTaskCreate(eap_autoreconnect_thread, ((const char*)"eap_autoreconnect_thread"), 1024, (void*) method, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate failed\n", __FUNCTION__);
#endif
}

// copy from ssl_client_ext.c
#if 0

#include <polarssl/ssl.h>
#include <polarssl/memory.h>

int max_buf_bio_size = SSL_BUFFER_LEN;

#if ENABLE_EAP_SSL_VERIFY_CLIENT
static x509_crt* _cli_crt = NULL;
static pk_context* _clikey_rsa = NULL;
#endif

#if ENABLE_EAP_SSL_VERIFY_SERVER
static x509_crt* _ca_crt = NULL;

static int eap_verify(void *data, x509_crt *crt, int depth, int *flags) 
{

	//char buf[1024];
	((void) data);

	printf("\nVerify requested for (Depth %d):\n", depth);
	//x509_crt_info(buf, sizeof(buf) - 1, "", crt);
	//printf("%s", buf);

	if(((*flags) & BADCERT_EXPIRED) != 0)
		printf("server certificate has expired\n");

	if(((*flags) & BADCERT_REVOKED) != 0)
		printf("  ! server certificate has been revoked\n");

	if(((*flags) & BADCERT_CN_MISMATCH) != 0)
		printf("  ! CN mismatch\n");

	if(((*flags) & BADCERT_NOT_TRUSTED) != 0)
		printf("  ! self-signed or not signed by a trusted CA\n");

	if(((*flags) & BADCRL_NOT_TRUSTED) != 0)
		printf("  ! CRL not trusted\n");

	if(((*flags) & BADCRL_EXPIRED) != 0)
		printf("  ! CRL expired\n");

	if(((*flags) & BADCERT_OTHER) != 0)
		printf("  ! other (unknown) flag\n");

	if((*flags) == 0)
		printf("  Certificate verified without error flags\n");

	return(0);
}
#endif

int eap_cert_init(void)
{
#if ENABLE_EAP_SSL_VERIFY_CLIENT
	if(eap_client_cert != NULL && eap_client_key != NULL){
		_cli_crt = polarssl_malloc(sizeof(x509_crt));
	
		if(_cli_crt)
			x509_crt_init(_cli_crt);
		else
			return -1;

		_clikey_rsa = polarssl_malloc(sizeof(pk_context));
	
		if(_clikey_rsa)
			pk_init(_clikey_rsa);
		else
			return -1;
	}
#endif
#if ENABLE_EAP_SSL_VERIFY_SERVER
	if(eap_ca_cert != NULL){
		_ca_crt = polarssl_malloc(sizeof(x509_crt));
	
		if(_ca_crt)
			x509_crt_init(_ca_crt);
		else
			return -1;
	}
#endif
	return 0;
}

void eap_client_cert_free(void)
{
#if ENABLE_EAP_SSL_VERIFY_CLIENT
	if(eap_client_cert != NULL && eap_client_key != NULL){
		if(_cli_crt) {
			x509_crt_free(_cli_crt);
			polarssl_free(_cli_crt);
			_cli_crt = NULL;
		}

		if(_clikey_rsa) {
			pk_free(_clikey_rsa);
			polarssl_free(_clikey_rsa);
			_clikey_rsa = NULL;
		}
	}
#endif	
}

void eap_server_cert_free(void)
{
#if ENABLE_EAP_SSL_VERIFY_SERVER
	if(eap_ca_cert != NULL){
		if(_ca_crt) {
			x509_crt_free(_ca_crt);
			polarssl_free(_ca_crt);
			_ca_crt = NULL;
		}
	}
#endif
}

int eap_cert_setup(ssl_context *ssl)
{
#if ENABLE_EAP_SSL_VERIFY_CLIENT
	if(eap_client_cert != NULL && eap_client_key != NULL){
		if(x509_crt_parse(_cli_crt, eap_client_cert, strlen(eap_client_cert)) != 0)
			return -1;
	
		if(pk_parse_key(_clikey_rsa, eap_client_key, strlen(eap_client_key), eap_client_key_pwd, strlen(eap_client_key_pwd)) != 0)
			return -1;

		ssl_set_own_cert(ssl, _cli_crt, _clikey_rsa);
	}
#endif
#if ENABLE_EAP_SSL_VERIFY_SERVER
	if(eap_ca_cert != NULL){
		if(x509_crt_parse(_ca_crt, eap_ca_cert, strlen(eap_ca_cert)) != 0)
			return -1;
		ssl_set_ca_chain(ssl, _ca_crt, NULL, NULL);
		ssl_set_authmode(ssl, SSL_VERIFY_REQUIRED);
		ssl_set_verify(ssl, eap_verify, NULL);
	}
#endif
	return 0;
}

#elif CONFIG_USE_MBEDTLS

#include <mbedtls/config.h>
#include <mbedtls/platform.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>
#include <mbedtls/ssl_internal.h>

int max_buf_bio_size = MBEDTLS_SSL_BUFFER_LEN;

struct mbedtls_ssl{
        void *ssl;
        void *conf;   
        void *fd;
};

#if ENABLE_EAP_SSL_VERIFY_CLIENT
static mbedtls_x509_crt* _cli_crt = NULL;
static mbedtls_pk_context* _clikey_rsa = NULL;
#endif

#if ENABLE_EAP_SSL_VERIFY_SERVER
static mbedtls_x509_crt* _ca_crt = NULL;

static int eap_verify(void *data, mbedtls_x509_crt *crt, int depth, int *flags) 
{

	//char buf[1024];
	((void) data);

	printf("\nVerify requested for (Depth %d):\n", depth);
	//mbedtls_x509_crt_info(buf, sizeof(buf) - 1, "", crt);
	//printf("%s", buf);

	if(((*flags) & MBEDTLS_X509_BADCERT_EXPIRED) != 0)
		printf("server certificate has expired\n");

	if(((*flags) & MBEDTLS_X509_BADCERT_REVOKED) != 0)
		printf("  ! server certificate has been revoked\n");

	if(((*flags) & MBEDTLS_X509_BADCERT_CN_MISMATCH) != 0)
		printf("  ! CN mismatch\n");

	if(((*flags) & MBEDTLS_X509_BADCERT_NOT_TRUSTED) != 0)
		printf("  ! self-signed or not signed by a trusted CA\n");

	if(((*flags) & MBEDTLS_X509_BADCRL_NOT_TRUSTED) != 0)
		printf("  ! CRL not trusted\n");

	if(((*flags) & MBEDTLS_X509_BADCRL_EXPIRED) != 0)
		printf("  ! CRL expired\n");

	if(((*flags) & MBEDTLS_X509_BADCERT_OTHER) != 0)
		printf("  ! other (unknown) flag\n");

	if((*flags) == 0)
		printf("  Certificate verified without error flags\n");

	return(0);
}
#endif

int eap_cert_init(void)
{
#if ENABLE_EAP_SSL_VERIFY_CLIENT
	if(eap_client_cert != NULL && eap_client_key != NULL){
		_cli_crt = mbedtls_calloc(1, sizeof(mbedtls_x509_crt));
	
		if(_cli_crt)
			mbedtls_x509_crt_init(_cli_crt);
		else
			return -1;

		_clikey_rsa = mbedtls_calloc(1, sizeof(mbedtls_pk_context));
	
		if(_clikey_rsa)
			mbedtls_pk_init(_clikey_rsa);
		else
			return -1;
	}
#endif
#if ENABLE_EAP_SSL_VERIFY_SERVER
	if(eap_ca_cert != NULL){
		_ca_crt = mbedtls_calloc(1, sizeof(mbedtls_x509_crt));
	
		if(_ca_crt)
			mbedtls_x509_crt_init(_ca_crt);
		else
			return -1;
	}
#endif
	return 0;
}

void eap_client_cert_free(void)
{
#if ENABLE_EAP_SSL_VERIFY_CLIENT
	if(eap_client_cert != NULL && eap_client_key != NULL){
		if(_cli_crt) {
			mbedtls_x509_crt_free(_cli_crt);
			mbedtls_free(_cli_crt);
			_cli_crt = NULL;
		}

		if(_clikey_rsa) {
			mbedtls_pk_free(_clikey_rsa);
			mbedtls_free(_clikey_rsa);
			_clikey_rsa = NULL;
		}
	}
#endif	
}

void eap_server_cert_free(void)
{
#if ENABLE_EAP_SSL_VERIFY_SERVER
	if(eap_ca_cert != NULL){
		if(_ca_crt) {
			mbedtls_x509_crt_free(_ca_crt);
			mbedtls_free(_ca_crt);
			_ca_crt = NULL;
		}
	}
#endif
}

int eap_cert_setup(struct mbedtls_ssl *ssl_context)
{
#if ENABLE_EAP_SSL_VERIFY_CLIENT
	if(eap_client_cert != NULL && eap_client_key != NULL){
		if(mbedtls_x509_crt_parse(_cli_crt, eap_client_cert, strlen(eap_client_cert)+1) != 0)
			return -1;
	
		if(mbedtls_pk_parse_key(_clikey_rsa, eap_client_key, strlen(eap_client_key)+1, eap_client_key_pwd, strlen(eap_client_key_pwd)+1) != 0)
                        return -1;

		mbedtls_ssl_conf_own_cert(ssl_context->conf, _cli_crt, _clikey_rsa);
	}
#endif
#if ENABLE_EAP_SSL_VERIFY_SERVER
	if(eap_ca_cert != NULL){
		if(mbedtls_x509_crt_parse(_ca_crt, eap_ca_cert, strlen(eap_ca_cert)+1) != 0)
			return -1;
		mbedtls_ssl_conf_ca_chain(ssl_context->conf, _ca_crt, NULL);
		mbedtls_ssl_conf_authmode(ssl_context->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
		mbedtls_ssl_conf_verify(ssl_context->conf, eap_verify, NULL);
	}
#endif
	return 0;
}

#endif /*CONFIG_USE_MBEDTLS*/

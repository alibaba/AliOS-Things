802.1X EAP METHOD SUPPLICANT EXAMPLE

Description:
Use 802.1X EAP methods to connect to AP and authenticate with backend radius server.
Current supported methods are EAP-TLS, PEAPv0/EAP-MSCHAPv2, and EAP-TTLS/MSCHAPv2.

Configuration:
Modify the argument of example_eap() in example_entry.c to set which EAP methods you want to use.
Modify the connection config (ssid, identity, password, cert) in example_eap_config() of example_eap.c based on your server's setting.
[FreeRTOSConfig.h]
	#define configTOTAL_HEAP_SIZE          ( ( size_t ) ( 70 * 1024 ) )
[platform_opts.h]
	# define CONFIG_EXAMPLE_EAP	1

	// Turn on/off the specified method
	# define CONFIG_ENABLE_PEAP	1
	# define CONFIG_ENABLE_TLS	1
	# define CONFIG_ENABLE_TTLS	1

	// If you want to verify the certificate of radius server, turn this on
	# define ENABLE_EAP_SSL_VERIFY_SERVER	1

Execution:
An EAP connection thread will be started automatically when booting.

Note:
Please make sure the lib_wps, polarssl, ssl_ram_map are also builded.

If the connection failed, you can try the following directions to make it work:
1. Make sure the config_rsa.h of PolarSSL include the SSL/TLS cipher suite supported by radius server.
2. Set a larger value to SSL_MAX_CONTENT_LEN in config_rsa.h
3. Increase the FreeRTOS heap in FreeRTOSConfig.h, for example you can increase the heap to 80kbytes:
	#define configTOTAL_HEAP_SIZE	( ( size_t ) ( 80 * 1024 ) )
4. Try to change using SW crypto instead of HW crypto.

HIGH-LOAD USE CASE MEMORY USAGE EXAMPLE

Description:
Example to monitor the memory usage for high-load use case.
Includes:
	- 3 TLS sessions
	- 6 UDP sessions including 5 UDP servers on different ports and 1 UDP client

Configuration:
[platform_opts.h]
	#define CONFIG_EXAMPLE_WLAN_FAST_CONNECT	0
	#define CONFIG_EXAMPLE_HIGH_LOAD_MEMORY_USE	1
[lwipopts.h]
	#define MEMP_NUM_NETCONN		20
	#define MEMP_NUM_UDP_PCB		MEMP_NUM_NETCONN
[FreeRTOSConfig.h]
	// Configure this if the heap size is not enough
	#define configTOTAL_HEAP_SIZE		( ( size_t ) ( 80 * 1024 ) )

Execution:
The example thread will be started automatically when booting.
You may need to change the IP/Port used in this example to make it work properly.
To verify the TLS sessions, you can setup an apache server for yourself and make sure the KeepAliveTimeout is larger than 5s.
To verify UDP server session, you can use iperf with following command to send UDP data:
	$ iperf.exe -c 192.168.1.123 -u -p 6001 -t 30 -i 1
To verify UDP client session, you can use iperf with following command to start UDP server:
	$ iperf.exe -s -p 6001 -u -i 1

Note:
Example work flow:
	Start heap monitor thread
	-> Enable Wi-Fi with STA mode
	-> Connect to AP by WPA2-AES
	-> Start 3 TLS sessions
	-> Start 6 UDP sessions

If you want to use ECDHE as TLS cipher suite, you can modify:
[config_rsa.h]
	#define POLARSSL_KEY_EXCHANGE_ECDHE_RSA_ENABLED
	#define POLARSSL_ECDH_C
	#define POLARSSL_ECP_C
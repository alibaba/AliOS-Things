SSL SERVER EXAMPLE

Description:
A simple SSL server which can reply for the https request

Configuration:

Modify SERVER_PORT and response content in example_ssl_server.c based on your SSL server.

[config_rsa.h]
	#define POLARSSL_CERTS_C
	#define POLARSSL_SSL_SRV_C

[platform_opts.h]
	#define CONFIG_EXAMPLE_SSL_SERVER	1

Execution:
Can make automatical Wi-Fi connection when booting by using wlan fast connect example.
A ssl server example thread will be started automatically when booting.


HTTPD EXAMPLE

Description:
Based on HTTPD API, an HTTPS server example with a simple homepage and GET/POST method test pages are provided

Configuration for HTTP: None
Configuration for HTTPS:
[config_rsa.h]
	#define POLARSSL_CERTS_C
	#define POLARSSL_SSL_SRV_C
[platform_opts.h]
	#define CONFIG_EXAMPLE_HTTPD         1
[lwipopts.h]
	#define SO_REUSE                     1

Execution:
Can make automatical Wi-Fi connection when booting by using wlan fast connect example.
A httpd example thread will be started automatically when booting.
HTTPS server is started by this exmaple in default. Can change to HTTP server by modifying starting parameter manually and rebuilding example.
Can test with a Web browser connecting to homepage.
Should link PolarSSL bignum.c to SRAM to speed up SSL handshake if starting HTTPS server.


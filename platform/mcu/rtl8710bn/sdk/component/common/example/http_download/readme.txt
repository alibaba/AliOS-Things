HTTP DOWNLOAD EXAMPLE

Description:
Download file from Web server via http.

Configuration:
Modify SERVER_HOST, SERVER_PORT and RESOURCE in example_http_download.c based on your HTTP server.

[platform_opts.h]
	#define CONFIG_EXAMPLE_HTTP_DOWNLOAD    1

Execution:
Can make automatical Wi-Fi connection when booting by using wlan fast connect example.
A http download example thread will be started automatically when booting.


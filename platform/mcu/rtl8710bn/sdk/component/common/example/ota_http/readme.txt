OTA HTTP UPDATING EXAMPLE

Description:
Download ota.bin from http download server(in tools\DownloadServer(HTTP))

Configuration:
[example_ota_http.c]
	Modify PORT, HOST and RESOURCE based on your HTTP download server.
	eg: SERVER: http://m-apps.oss-cn-shenzhen.aliyuncs.com/051103061600.bin
	set:	#define PORT	80
		#define HOST	"m-apps.oss-cn-shenzhen.aliyuncs.com"
		#define RESOURCE "051103061600.bin"

[platform_opts.h]
	#define CONFIG_EXAMPLE_OTA_HTTP    1
[ota_8195a.h]
	#define HTTP_OTA_UPDATE	

Execution:
Can make automatical Wi-Fi connection when booting by using wlan fast connect example.
A http download example thread will be started automatically when booting.
Using the example with the tool in tools\DownloadServer(HTTP)


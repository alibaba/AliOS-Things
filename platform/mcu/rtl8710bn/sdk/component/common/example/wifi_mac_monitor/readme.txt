WIFI MAC MONITOR EXAMPLE

Description:
Get station mac information in AP mode

Configuration:
[platform_opts.h]
	#define CONFIG_EXAMPLE_WIFI_MAC_MONITOR 1

Execution:
When define CONFIG_EXAMPLE_WIFI_MAC_MONITOR, the callback function is automatically registered as get_station_mac().
It can collect the station mac address and rssi.

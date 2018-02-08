LWIP SNTP SHOWTIME EXAMPLE

Description:
Show system time maintained by time from NTP server and system tick.

Configuration:
Can Modify SNTP_SERVER_ADDRESS and SNTP_UPDATE_DELAY in sntp.c for NTP time update

[platform_opts.h]
	#define CONFIG_EXAMPLE_SNTP_SHOWTIME    1

Execution:
Can make automatical Wi-Fi connection when booting by using wlan fast connect example.
A lwip ntp showtime example thread will be started automatically when booting.

WIFI API SCENARIOS EXAMPLE

Description:
Provide some Wi-Fi API scenarios for common usage.
Includes:
      - Network Scan
      - Authentication
      - Mode switch about 7 cases
      - Sequence scenario about 4 cases

Configuration:
Modify the argument of example_wlan_scenario() in example_entry.c to switch example cases.
[platform_opts.h]
	#define CONFIG_ENABLE_WPS		1
	#define CONFIG_ENABLE_P2P		1	
	#define CONFIG_EXAMPLE_WLAN_FAST_CONNECT	0
	#define CONFIG_EXAMPLE_WLAN_SCENARIO	1

Execution:
The Wi-Fi example thread will be started automatically when booting.

Note:
The currently argument options are:
    "S" for network scan:
      - Scan nearby wlan routers and get info for each routers

    "A" for Authentication:
      - WPS-PBC
      - WPS-PIN static PIN
      - WPS-PIN dynamic PIN
      - open
      - WEP open (64 bit)
      - WEP open (128 bit)
      - WEP shared (64 bit)
      - WEP shared (128 bit)
      - WPA-PSK (TKIP)
      - WPA-PSK (AES)
      - WPA2-PSK (TKIP)
      - WPA2-PSK (AES)

    "M1" for mode switch case 1:
      - Mode switch to infrastructure (AP mode)

    "M2" for mode switch case 2:
      - Mode switch to infrastructure (STA mode)

    "M3" for mode switch case 3:
      - Mode switch to infrastructure (P2P Autonomous GO)

    "M4" for mode switch case 4:
      - Mode switching time between AP and STA

    "M5" for mode switch case 5:
      - Mode switching time between P2P autonomous GO and STA
    
    "M6" for mode switch case 6:
      - Mode switch to infrastructure (AP mode with hidden SSID)
    
    "M7" for mode switch case 7:
      - Mode switching between concurrent mode and STA

    "S1" for sequence scenario case 1:
        Enable Wi-Fi with STA mode
        -> Connect to AP by WPS enrollee static PIN mode (If failed, re-connect one time.)
        -> Enable Wi-Fi Direct GO (It will re-enable WiFi, the original connection to AP would be broken.)

    "S2" for sequence scenario case 2:
        Enable Wi-Fi Direct GO
        -> Disable Wi-Fi Direct GO, and enable Wi-Fi with STA mode (Disable Wi-Fi Direct GO must be done to release P2P resource.)
        -> Connect to AP by WPS enrollee PBC mode (If failed, re-connect one time.)

    "S3" for sequence scenario case 3:
        Enable Wi-Fi with STA mode
        -> Scan network
        -> Connect to AP use STA mode (If failed, re-connect one time.)
        -> Enable Wi-Fi Direct GO (It will re-enable WiFi, the original connection to AP would be broken.)

    "S4" for sequence scenario case 4:
        Enable Wi-Fi with STA mode
        -> Connect to AP by WPS enrollee PBC mode (If failed, re-connect one time.)
        -> Disconnect from AP
        -> Enable Wi-Fi Direct GO
        -> Disable Wi-Fi Direct GO, and enable Wi-Fi with STA mode (Disable Wi-Fi Direct GO must be done to release P2P resource.)
        -> Connect to AP use STA mode (If failed, re-connect one time.)
        -> Disconnect from AP
        -> Disable Wi-Fi
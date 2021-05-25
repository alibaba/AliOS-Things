package com.rtk.Configuration;

public class GlobalConfig {
	public static String APP_VERSION 				= "V1.0a";
	public static String DEVICE_NAME 				= "Repeater";
	
	//RSSI -> position
	public static String KEY_HIGH_RSSI				= "HIGH_RSSI";
	public static String KEY_LOW_RSSI				= "LOW_RSSI";
	public static int DEFAULT_HIGH_RSSI				= 65;	//RSSI 65 = 65 - 100 dBm = -35 dBm
	public static int DEFAULT_LOW_RSSI				= 45;	//RSSI 45 = 45 - 100 dBm = -55 dBm
	
	//filter BT by name
	public static boolean CONFIG_FILTER_BT			= false;
	public static String KEY_FILTER_BT_NAME			= "FILTER_BT_NAME";
	public static String DEFAULT_FILTER_BT_NAME		= "realtek_repeater";
	
	//save previous BT profile
	public static boolean CONFIG_SAVE_BT_PROFILE 	= true;
	public static String KEY_PREVIOUSE_BT_NAME		= "PREVIOUSE_BT_NAME";
	public static String KEY_PREVIOUSE_BT_MAC		= "PREVIOUSE_BT_MAC";
	public static String DEFAULT_PREVIOUSE_BT_NAME	= "";
	public static String DEFAULT_PREVIOUSE_BT_MAC	= "";
	
	
	//save AP password
	public static boolean CONFIG_SAVE_AP_PWD		  = true;
	
	//detect the position
	public static boolean CONFIG_POSITION_DECTECTION  = true;
	
	//detect BT offline and reconnect to BT 
	public static boolean CONFIG_OFFLINE_DECTECTION   = true;
		
	//save configured success AP profile
	public static boolean CONFIG_SAVE_AP_PROFILE	  = true;
}

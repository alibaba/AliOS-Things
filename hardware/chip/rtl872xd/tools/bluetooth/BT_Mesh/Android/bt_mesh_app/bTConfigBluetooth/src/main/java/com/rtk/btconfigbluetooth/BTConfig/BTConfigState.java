package com.rtk.btconfigbluetooth.BTConfig;

public class BTConfigState {
	/*************************BTConfig state*************************/
	/* 1. BTConfig state:  connect to repeater Bluetooth */
	public static final int STATE_BT_CONNECTING					=	0x10;
	public static final int STATE_BT_CONNECT_FAIL				=	0x11;
	public static final int STATE_BT_CONNECT_OK					=	0x12;
	
	/*	2. BTConfig state: get band support capability of repeater	*/
	public static final int STATE_BT_QUERY_WLAN_BAND			=	0x20;
	public static final int STATE_BT_QUERY_WLAN_BAND_END		=	0x21;
	
	/*	3. BTConfig state: scan 2.4G AP	*/
	public static final int STATE_BT_SCAN_WLAN_2G				=	0x25;
	public static final int STATE_BT_RECEIVE_WLAN_2G			=	0x26;
	public static final int STATE_BT_SCAN_WLAN_2G_END			=	0x27;
	public static final int STATE_BT_SCAN_WLAN_2G_HOMEAP		=	0x28;
	
	/*	4. BTConfig state: scan 5G AP	*/
	public static final int STATE_BT_SCAN_WLAN_5G				=	0x2a;
	public static final int STATE_BT_RECEIVE_WLAN_5G			=	0x2b;
	public static final int STATE_BT_SCAN_WLAN_5G_END			=	0x2c;
	public static final int STATE_BT_SCAN_WLAN_5G_HOMEAP		=	0x2d;
	
	/*	5. BTConfig state: get stations	*/
//	public static final int STATE_BT_QUERY_STATION_2G			=	0x30;
//	public static final int STATE_BT_GET_STATION_2G_END			=	0x31;
//	public static final int STATE_BT_QUERY_STATION_5G			=	0x32;
//	public static final int STATE_BT_GET_STATION_5G_END			=	0x33;
	
	/* 6. BTConfig state: connect to remote AP	*/
	public static final int STATE_BT_SEND_WLAN_PROFILE			=	0x40;
	public static final int STATE_BT_SEND_WLAN_PROFILE_END		=	0x41;
	
	/* 7. BTConfig state: check repeater status	*/
	public static final int STATE_BT_QUERY_REPEATER_STATUS		=	0x45;
	public static final int STATE_BT_QUERY_REPEATER_STATUS_END	=	0x46;
	
	/* 8. BTConfig state: repeater is offLine	*/
	public static final int STATE_BT_REPEATER_OFFLINE			=	0x50;

	/* 9. send system command/receive response **/
//	public static final int STATE_BT_REQUEST_CLI				=	0x55;	
//	public static final int STATE_BT_REPLY_CLI					=	0x56;	
//	public static final int STATE_BT_REPLY_CLI_END				=	0x57;	
//	public static final int STATE_BT_PARSE_REPLY_CLI_ERROR		=	0x58;
	
	public static final int STATE_AP_SCAN_WLAN_END				=	0x71;
	
	public static final int STATE_BT_DISCONNECT					=	0x81;
}

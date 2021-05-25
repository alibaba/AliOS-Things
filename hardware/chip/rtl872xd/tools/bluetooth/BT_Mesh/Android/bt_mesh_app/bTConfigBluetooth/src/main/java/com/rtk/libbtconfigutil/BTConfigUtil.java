package com.rtk.libbtconfigutil;

public class BTConfigUtil {
	static
	{
		System.loadLibrary("BTConfigUtil");
	}
  
	/*1.construct command to repeater */
	public native byte[] construct_get_wlan_band_cmd();
	public native byte[] construct_site_survery_2G_cmd();
	public native byte[] construct_site_survery_5G_cmd();
	public native byte[] construct_AP_profile_cmd(byte[] profile, int len);
	public native byte[] construct_check_repeater_status_cmd();

	/*2. parse reply from repeater */
	public native int parse_wlan_band_reply(byte[] msg, int len);
	public native int parse_AP_profile_ACK_reply(byte[] msg, int len);
	public native int parse_repeater_status_reply(byte[] msg, int len);
	// RFCOMM
	public native int parse_AP_results_2G_reply(byte[] msg, int len);
	public native int parse_AP_results_5G_reply(byte[] msg, int len);
	// BLE
	public native int parse_AP_results_2G_reply_GATT(int check, byte[] check_mac, byte[] msg, int len);
	public native int parse_AP_results_5G_reply_GATT(int check, byte[] check_mac, byte[] msg, int len);
	
	
	/*3. get results after parse */
	public native byte get_band_support_2G_result();
	public native byte get_band_support_5G_result();
	public native byte get_product_type();
	public native APClass[] get_AP_scan_2G_results();
	public native APClass[] get_AP_scan_5G_results();
	public native APClass[] get_repeater_status_results();
	
}

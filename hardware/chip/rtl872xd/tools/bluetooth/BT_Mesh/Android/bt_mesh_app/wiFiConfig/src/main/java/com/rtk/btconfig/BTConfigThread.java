package com.rtk.btconfig;


import com.rtk.btconfigbluetooth.BTConfig.BTConfig;

import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.util.Log;

public class BTConfigThread extends HandlerThread{
	private String TAG = "BTConfigThread";
	private Handler hBTConfigHandler;
	public BTConfig hBTConfig;
	
	public static String KEY_BTDEVICE_MAC;
	public static String KEY_BTCONFIG_AP;
	
	// BTConfig CMD
	public final static int CMD_START_SCAN_BT				=	0x01;
	public final static int CMD_CONNECT_TO_REPEATER			=	0x05;
	
	public final static int CMD_START_INPUTSTREAM_RECEIVE	=	0x10;
	
	public final static int CMD_QUERY_WLAN_BAND				=	0x15;
	public final static int CMD_START_SCAN_WLAN_2G			=	0x16;
	public final static int CMD_START_SCAN_WLAN_5G			=	0x17;
	public final static int CMD_CONNECT_EXTEND_AP			=	0x18;
	public final static int CMD_QUERY_REPEATER_STATUS		=	0x19;
	
//	public final static int CMD_SEND_SYSCMD					=	0x20;
	
	public final static int CMD_CLOSE_BTSOCKET				=	0xff;

	public BTConfigThread(String name) {
		super(name);
	}

	public Handler getHandler(){
		if(hBTConfigHandler != null){
			return hBTConfigHandler;
		}else{
			hBTConfigHandler = new Handler(Looper.myLooper()){
				public void handleMessage(android.os.Message msg) {
					switch(msg.what){
					/* 	start scan bt device	*/
					case CMD_START_SCAN_BT:
						Log.w(TAG,"BTCommand.CMD_START_SCAN_BT");
						
						hBTConfig.startBTScan();
						break;
					/* 	connect to repeater	*/
					case CMD_CONNECT_TO_REPEATER:
						Log.w(TAG,"BTCommand.CMD_CONNECT_TO_REPEATER");
						String mBTDeviceMac = msg.getData().getString(KEY_BTDEVICE_MAC);
						
						hBTConfig.startConnect(mBTDeviceMac);
						break;
					/* 	start to receive  message from repeater */
					case CMD_START_INPUTSTREAM_RECEIVE:
						Log.w(TAG,"BTCommand.CMD_START_INPUTSTREAM_RECEIVE");

						hBTConfig.startReceive();

						break;
					/* query band support capability */
					case CMD_QUERY_WLAN_BAND:
						Log.w(TAG,"BTCommand.CMD_QUERY_WLAN_BAND");
						
						hBTConfig.queryWlanBand();
						break;
					/* scan 2.4G AP */
					case CMD_START_SCAN_WLAN_2G:
						Log.w(TAG,"BTCommand.CMD_START_SCAN_WLAN_2G");

						if(BTConfig.flag){
							hBTConfig.startReceive();
						}									
						hBTConfig.doSiteSurvey_2G();
				
						break;
					/* scan 5G AP */
					case CMD_START_SCAN_WLAN_5G:
						Log.w(TAG,"BTCommand.CMD_START_SCAN_WLAN_5G");
						
						if(BTConfig.flag){
							hBTConfig.startReceive();
						}
						hBTConfig.doSiteSurvey_5G();
						
						break;
					/* send Extended AP profile for connected to repeater */
					case CMD_CONNECT_EXTEND_AP:
						Log.w(TAG,"BTCommand.CMD_CONNECT_EXTEND_AP");

						byte[] extendAPInfo = msg.getData().getByteArray(KEY_BTCONFIG_AP);
						hBTConfig.sendAPProfile(extendAPInfo);

						if(BTConfig.flag){
							hBTConfig.startReceive();
						}
						
						break;
					/* query repeater connection status */
					case CMD_QUERY_REPEATER_STATUS:
						Log.w(TAG,"BTCommand.CMD_QUERY_REPEATER_STATUS");
						
						hBTConfig.queryRepeaterStatus();
						
						if(BTConfig.flag){
							hBTConfig.startReceive();
						}
						break;
					/* close BluetootSocket established with repeater */
					case CMD_CLOSE_BTSOCKET:
						Log.w(TAG,"BTCommand.CMD_CLOSE_BTSOCKET");
						
						hBTConfig.closeBTSocket();
						break;

					default:
						break;
					}
				}
			};
			
			return hBTConfigHandler;
		}
	}
}

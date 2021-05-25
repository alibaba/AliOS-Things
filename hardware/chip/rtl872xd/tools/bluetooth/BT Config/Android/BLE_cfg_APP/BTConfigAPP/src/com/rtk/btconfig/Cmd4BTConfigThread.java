package com.rtk.btconfig;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;

public class Cmd4BTConfigThread {

	/**	send common cmd to btConfig thread	**/
	public static void sendCommonCmd(int cmdWhat, Handler handler){
		handler.sendEmptyMessage(cmdWhat);
	} 
	
	/**	send repeater connect cmd and its mac to btConfig thread **/
	public static void sendConnectBTCmd(String btDeviceMac, Handler handler){
		Message msg = new Message();
		Bundle data = new Bundle();
		data.putString(BTConfigThread.KEY_BTDEVICE_MAC, btDeviceMac);
		
		msg.what = BTConfigThread.CMD_CONNECT_TO_REPEATER;
		msg.setData(data);
		handler.sendMessage(msg);
	}  
	 
	/**	send remote AP "connect" cmd and its info to btConfig thread **/
	public static void sendAPProfileCmd(byte[] btConfigInfo, Handler handler){
		Message msg = new Message();
		Bundle data = new Bundle();
		data.putByteArray(BTConfigThread.KEY_BTCONFIG_AP, btConfigInfo);
		
		msg.what = BTConfigThread.CMD_CONNECT_EXTEND_AP;
		msg.setData(data);
		handler.sendMessage(msg);
	} 
	
}

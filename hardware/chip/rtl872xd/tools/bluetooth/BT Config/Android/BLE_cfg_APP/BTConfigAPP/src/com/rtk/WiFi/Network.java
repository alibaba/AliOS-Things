package com.rtk.WiFi;

import java.util.List;

import android.content.Context;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;

public class Network {
	
	private static final String TAG = "NetworkOps";
	private WifiManager SCWifiMngr = null;
	private WifiInfo SCWifiInfo;
	private WifiConfiguration wifiConfig 	= null;
	private static String macAddr;
	private Context context;
	
	public void WifiInit(Context context){
		this.context = context;
		SCWifiMngr = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
		SCWifiInfo = SCWifiMngr.getConnectionInfo();
		macAddr = (SCWifiInfo == null) ? null : SCWifiInfo.getMacAddress();
	}
	
	public void WifiOpen(){
		if(!SCWifiMngr.isWifiEnabled()){
			SCWifiMngr.setWifiEnabled(true);
		}
	}
	
	public void WifiStartScan(){
		SCWifiMngr.startScan();
	}
	
	public List<ScanResult> WifiGetScanResults(){
		return SCWifiMngr.getScanResults();
	}
}


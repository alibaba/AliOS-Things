package com.rtk.btconfigbluetooth.ScanResults;

import java.io.Serializable;

/** 
 ** class for devices list, including Bluetooth Device/ WiFi AP/ Extended AP
 **/
@SuppressWarnings("serial")
public  class ScanObj implements Serializable {
	private String mSsid;
	private String mMac;
	private int mRssi;
	private byte mBand;
	private int mTTL;
	private byte mEncrpytType;
	private byte mConnectStatus;
	private byte mConfigureStatus;
	public static boolean CompareFlag = true; //true:compare via mac  false:compare via ssid
	
	// Bluetooth device 
	public ScanObj(String ssid, String mac, int rssi){
		this.mSsid 		= ssid;
		this.mMac 		= mac;
		this.mRssi		= rssi;
	}
	
	// WiFi AP
	public ScanObj(String ssid, String mac, int rssi, byte encrpytType, int TTL, byte band){
		this.mSsid 	= ssid;
		this.mMac 	= mac;
		this.mRssi 	= rssi;
		this.mEncrpytType = encrpytType;
		this.mTTL = TTL;
		this.mBand = band;
	}
	
	// Extended AP
	public ScanObj(String ssid, String mac, int rssi, byte encrpytType, byte connectStatus, byte configureStatus){
		this.mSsid 	= ssid;
		this.mMac 	= mac;
		this.mRssi 	= rssi;
		this.mEncrpytType = encrpytType;
		this.mConnectStatus  = connectStatus;
		this.mConfigureStatus = configureStatus;
	}

	public String getSSID(){
		return mSsid;
	}
	
	public String getMac(){
		return mMac;
	}
	
	public void setMac(String mac){
		mMac = mac;
		return;
	}
	
	public int getRssi(){
		return mRssi;
	}
	
	public void setRssi(int rssi){
		mRssi = rssi;
		return;
	}
	
	public byte getEncrpytType(){
		return mEncrpytType;
	}
	
	public byte getConnectStatus(){
		return mConnectStatus;
	}
	
	public byte getConfigureStatus(){
		return mConfigureStatus;
	}
	
	public byte getBand(){
		return mBand;
	}
	
	public void setBand(byte band){
		mBand = band;
		return;
	}
	
	public int getTTL(){
		return mTTL;
	}
	
	public void decreaseTTL(){
		mTTL = mTTL - 1;
		return;
	}
	
	public void resetTTL(int initTTL){
		mTTL = initTTL;
		return;
	}
	
	 @Override
	 public boolean equals(Object object){
		 boolean sameSame = false;

		 if(CompareFlag){
		     if (object != null && object instanceof ScanObj){
		    	 sameSame = this.mMac.equals(((ScanObj) object).mMac);
		     }
		 }
		 else{
			 if (object != null && object instanceof ScanObj){
		    	 sameSame = this.mSsid.equals(((ScanObj) object).mSsid);
		     }			 
		 }
	     return sameSame;
	 }
	 
	 @Override
	 public int hashCode() {
		 if(CompareFlag){
			 return mMac.hashCode();
		 }else{
			 return mSsid.hashCode();
		 }
	 }
}


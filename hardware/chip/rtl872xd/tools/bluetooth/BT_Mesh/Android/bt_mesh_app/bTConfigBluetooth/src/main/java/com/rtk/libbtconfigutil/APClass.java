package com.rtk.libbtconfigutil;

public class APClass {
	private byte mBand;
	private String mSsid;
	private String mMac;
	private byte mRssi;
	private byte mEncrpytType;
	private byte mConnectStatus;
	private byte mConfigureStatus;
	
	public byte getBand(){
		return mBand;
	}
	
	public String getSSID(){
		return mSsid;
	}
	
	public String getMac(){
		return mMac;
	}
	
	public byte getRssi(){
		return mRssi;
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
}

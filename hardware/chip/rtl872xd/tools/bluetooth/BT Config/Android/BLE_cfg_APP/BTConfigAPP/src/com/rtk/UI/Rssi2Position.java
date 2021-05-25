package com.rtk.UI;

import com.rtk.btconfig.MainActivity;
import com.rtk.Configuration.GlobalConfig;

//import android.util.Log;

/**** 
 * 
 * parse  RSSI to position (too far/ fine /too close) 
 * 
 * **/
public class Rssi2Position {
	private String TAG = "ParseRssiToPosition";
	/* position info */
	private int mShowPositionInterval = 3; //average the values per 3 time 
	private int mReceiveConnectStatusCount;
	private byte mRssiTmp1;
	private byte mRssiTmp2;
	private byte mRssiTmp3;
	private byte mAverageRssi;
	
	private int mLastPositionIndex 		= -1;
	private int mCurrentPositionIndex 	= -1;
	private int mFinalShowPositionIndex = -1;
	
	 /** get position index by RSSI (0: too far; 1: fine; 2: too close) **/
	 public int getRssiRangeIndex(byte rssi, int highRssi, int lowRssi){
		 if(rssi > highRssi){
			 return 0; // too far
		 }else if(rssi <= highRssi && rssi >= lowRssi){
			 return 1; // fine
		 }else{
			 return 2; // too close
		 }
	 }
	 
	 /** parse position index	**/
	 public int getPostionRangeIndex(int currentRssi){
		 int highRssi = MainActivity.mSharePref.getInt(GlobalConfig.KEY_HIGH_RSSI, GlobalConfig.DEFAULT_HIGH_RSSI);
		 int lowRssi  = MainActivity.mSharePref.getInt(GlobalConfig.KEY_LOW_RSSI, GlobalConfig.DEFAULT_LOW_RSSI);
		 
		 mCurrentPositionIndex = -1;
		 mFinalShowPositionIndex = -1;
		 
		 mReceiveConnectStatusCount++;
		 /*1. first time, calculate initial average RSSI by sampling instant RSSI 3 times */
		 if(mReceiveConnectStatusCount <= mShowPositionInterval){
			 if(mReceiveConnectStatusCount % mShowPositionInterval == 1){
				 mRssiTmp1 = (byte)(currentRssi+100);
			 }else if(mReceiveConnectStatusCount % mShowPositionInterval == 2){
				 mRssiTmp2 = (byte)(currentRssi+100);
			 }else{
				 mRssiTmp3 = (byte)(currentRssi+100);
				 mAverageRssi = (byte)((mRssiTmp1 + mRssiTmp2+ mRssiTmp3)/3);
//				 Log.d(TAG, "mAverageRssi:" + String.valueOf(mAverageRssi-100));
				 
				 mCurrentPositionIndex = getRssiRangeIndex(mAverageRssi, highRssi, lowRssi);
				 mLastPositionIndex = mCurrentPositionIndex;
			 }
			 mFinalShowPositionIndex = mCurrentPositionIndex;
			 return mFinalShowPositionIndex;
		 }
		 /*2. when average RSSI already existed, re-calculate new average RSSI based on previous average RSSI */
		 else{
			 mRssiTmp1 = mRssiTmp2;
			 mRssiTmp2 = mRssiTmp3;
			 mRssiTmp3 = (byte)(currentRssi+100);
			 mAverageRssi = (byte)((mRssiTmp1 + mRssiTmp2+ mRssiTmp3)/3);
			 
			 mCurrentPositionIndex = getRssiRangeIndex(mAverageRssi, highRssi, lowRssi);
			 mFinalShowPositionIndex = mCurrentPositionIndex;
			
			 // If second time range is different to first time, then reference third values
			 if(mFinalShowPositionIndex != -1 && mLastPositionIndex != -1
					 && mFinalShowPositionIndex != mLastPositionIndex){ 
				 mFinalShowPositionIndex = -1;
				 mReceiveConnectStatusCount = 0;
				 mLastPositionIndex = -1;
			 }else{
				 mFinalShowPositionIndex = mCurrentPositionIndex;
				 mLastPositionIndex = mCurrentPositionIndex;
			 }
			 return mFinalShowPositionIndex;
		 }
	 }
	 
	 public void resetReceiveConnectStatusCount(){
		 mReceiveConnectStatusCount = 0;
	 }
	 
}

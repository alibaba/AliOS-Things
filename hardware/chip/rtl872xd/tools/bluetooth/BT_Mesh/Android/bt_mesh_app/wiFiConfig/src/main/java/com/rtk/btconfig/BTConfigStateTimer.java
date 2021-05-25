package com.rtk.btconfig;

import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

import android.util.Log;

import com.rtk.btconfigbluetooth.BTConfig.BTConfig;
import com.rtk.btconfigbluetooth.BTConfig.BTConfigState;
import com.rtk.btconfigbluetooth.ScanResults.ScanObj;

import com.rtk.Configuration.GlobalConfig;

public class BTConfigStateTimer {
private String TAG = "CheckStatusTimer";
	
	private TimerTask mCheckStateTimerTask;
	private Timer mCheckStateTimer;
	
	/* check interval */
	private int mTimerCheckInterval = 1000; 
		
	/* wait for response of wlan band capability */
	private int mTimeOutForResponseWlanBand = 1000; // 1s
	private int mWaitCountsForResponseWlanBand;
	private int mMaxWaitCountsForResponseWlanBand = mTimeOutForResponseWlanBand/mTimerCheckInterval;
	
	/* wait for results of scanning 2.4G AP */
	private int mTimeOutForScan2G     = 20000; //20s
	private int mWaitCountsForScanWlan2G;
	private int mMaxWaitCountsForScan2G = mTimeOutForScan2G/mTimerCheckInterval;
	
	/* wait for results of scanning 5G AP  */
	private int mTimeOutForScan5G     = 20000; //20s
	private int mWaitCountsForScanWlan5G;
	private int mMaxWaitCountsForScan5G = mTimeOutForScan5G/mTimerCheckInterval;
	
	/* wait for ACK of receiving command for connecting to remote AP  */
	private int mTimeOutForAckAPConnect = 1000; // 1s
	private int mWaitCountsForAckAPConnect;
	private int mMaxWaitCountsForAckAPConnect = mTimeOutForAckAPConnect/mTimerCheckInterval;
	
	/* check if repeater have no response  when choose position adjustment */
	private int mTimeoutForResponse	= 5000; // 5s
	private int mWaitForRepeaterResponse;
	private int mMaxWaitCountsForResponse = mTimeoutForResponse/mTimerCheckInterval;
	
	/* flag for repeater has no response */
	public boolean mIsNoResponse = false;
	
	/* detect the repeater reOnline */
	public boolean mRepeaterRedetect = false;
	
	public int product_type = 0;
	
	public int wlan2GEnabled = 0;
	public int wlan5GEnabled = 0;

	public void startCheckStatusTimer(final BTConfig mBTConfig){
		if(mCheckStateTimerTask != null && mCheckStateTimer != null){
			return;
		}
		else{
			if(mCheckStateTimerTask == null){
				mCheckStateTimerTask = new TimerTask() {
					@Override
					public void run() {

						if(MainActivity.mMainAct.mClickBackToBTUI)
							return;

						int mBTConfigState = mBTConfig.getBTConfigState();
						//Log.i(TAG, "mBTConfigState :" + mBTConfigState);

						// connecting to Bluetooth
						if(mBTConfigState == BTConfigState.STATE_BT_CONNECTING){
							return;

						}
						// connect to Bluetooth fail
						else if(mBTConfigState == BTConfigState.STATE_BT_CONNECT_FAIL){
							if(mBTConfig.closeBTSocket()){
								Cmd4BTConfigThread.sendConnectBTCmd(MainActivity.mSavedBTMAC,
										MainActivity.mMainAct.mBTConfigHandler);
							}
							return;

						}
						// connect to Bluetooth OK
						else if(mBTConfigState == BTConfigState.STATE_BT_CONNECT_OK){
							mIsNoResponse = false;
							resetWaitCount();

							if(!mRepeaterRedetect){
								MainActivity.mMainAct.mUIHandler.sendEmptyMessage(UIMsgHandler.MSG_CONNECT_BT_OK);
							}
							else
							{
								mRepeaterRedetect = false;
								MainActivity.mMainAct.mUIHandler.sendEmptyMessage(UIMsgHandler.MSG_RECONNECT_BT_OK);
							}
							return;

						}
						// query wlan band capability
						else if(mBTConfigState == BTConfigState.STATE_BT_QUERY_WLAN_BAND){

							mWaitCountsForScanWlan2G = 0;
							mWaitCountsForScanWlan5G = 0;

							mWaitCountsForResponseWlanBand++;
							if(mWaitCountsForResponseWlanBand > mMaxWaitCountsForResponseWlanBand){
								Log.e(TAG,"query band timeout");
								mWaitCountsForResponseWlanBand = 0;
								MainActivity.mWaitForBandResult = true;

								Cmd4BTConfigThread.sendCommonCmd(BTConfigThread.CMD_QUERY_WLAN_BAND, MainActivity.mMainAct.mBTConfigHandler);
							}
							return;

						}
						// get wlan band capability response
						else if(mBTConfigState == BTConfigState.STATE_BT_QUERY_WLAN_BAND_END){

							mWaitCountsForResponseWlanBand = 0;
							MainActivity.mWaitForBandResult = false;

							wlan2GEnabled = mBTConfig.getWlanBand_2G();
							wlan5GEnabled = mBTConfig.getWlanBand_5G();
							product_type = mBTConfig.getProductType();

							if(wlan2GEnabled != 1 && wlan5GEnabled != 1){
								MainActivity.mMainAct.mUIHandler.sendEmptyMessage(UIMsgHandler.MSG_NO_BAND_SUPPORT);
								return;
							 }
							else if(wlan2GEnabled == 1){
								mWaitCountsForScanWlan2G = 0;
								Cmd4BTConfigThread.sendCommonCmd(BTConfigThread.CMD_START_SCAN_WLAN_2G, MainActivity.mMainAct.mBTConfigHandler);
							}
							else if(wlan5GEnabled == 1){
								mWaitCountsForScanWlan5G = 0;
								Cmd4BTConfigThread.sendCommonCmd(BTConfigThread.CMD_START_SCAN_WLAN_5G, MainActivity.mMainAct.mBTConfigHandler);
							}

							return;

						}
						// scan wlan AP 2G
						else if(mBTConfigState == BTConfigState.STATE_BT_SCAN_WLAN_2G){
							mWaitCountsForScanWlan2G++;

							if( mWaitCountsForScanWlan2G > mMaxWaitCountsForScan2G ){
								Log.e(TAG,"scan 2G timeout");
								mWaitCountsForScanWlan2G = 0;

								if(mBTConfig.getWlanBand_5G() == 1){

									Cmd4BTConfigThread.sendCommonCmd(BTConfigThread.CMD_START_SCAN_WLAN_5G, MainActivity.mMainAct.mBTConfigHandler);
								}
								else{
									MainActivity.mIsDoingScanWlan = false;
									MainActivity.mMainAct.mUIHandler.sendEmptyMessage(UIMsgHandler.MSG_SHOW_SCAN_TIMEOUT);
								}
							}
							return;

						}
						// receiving wlan AP 2G results
						else if(mBTConfigState == BTConfigState.STATE_BT_RECEIVE_WLAN_2G){
							mWaitCountsForScanWlan2G = 0;
							return;

						}
						// STATE_BT_SCAN_WLAN_2G_HOMEAP
						else if(mBTConfigState == BTConfigState.STATE_BT_SCAN_WLAN_2G_HOMEAP){
							MainActivity.mMainAct.mUIHandler.sendEmptyMessage(UIMsgHandler.MSG_SHOW_HOMEAP_DIALOG);
							mBTConfig.setBTConfigState(BTConfigState.STATE_BT_SCAN_WLAN_2G_END);
							return;

						}
						// STATE_BT_SCAN_WLAN_5G_HOMEAP
						else if(mBTConfigState == BTConfigState.STATE_BT_SCAN_WLAN_5G_HOMEAP){
							MainActivity.mMainAct.mUIHandler.sendEmptyMessage(UIMsgHandler.MSG_SHOW_HOMEAP_DIALOG);
							mBTConfig.setBTConfigState(BTConfigState.STATE_BT_SCAN_WLAN_5G_END);
							return;

						}
						//finish receiving wlan AP 2G results
						else if(mBTConfigState == BTConfigState.STATE_BT_SCAN_WLAN_2G_END){

							mWaitCountsForScanWlan2G = 0;

							if(!mBTConfig.getWlanScanResults(0).isEmpty()){
								MainActivity.mMainAct.mUIHandler.sendEmptyMessage(UIMsgHandler.MSG_UPDATE_WLAN_2G);
							}

							if(mBTConfig.getWlanBand_5G() != 1){

								MainActivity.mMainAct.mUIHandler.sendEmptyMessage(UIMsgHandler.MSG_DISMISS_SCAN_DIALOG);

								Cmd4BTConfigThread.sendCommonCmd(BTConfigThread.CMD_START_SCAN_WLAN_2G, MainActivity.mMainAct.mBTConfigHandler);

							}else{
								MainActivity.mMainAct.mUIHandler.sendEmptyMessage(UIMsgHandler.MSG_DISMISS_SCAN_DIALOG);

								Cmd4BTConfigThread.sendCommonCmd(BTConfigThread.CMD_START_SCAN_WLAN_5G, MainActivity.mMainAct.mBTConfigHandler);
							}
							return;

						}
						//scan wlan AP 5G
						else if(mBTConfigState == BTConfigState.STATE_BT_SCAN_WLAN_5G){
							mWaitCountsForScanWlan5G++;
							if( mWaitCountsForScanWlan5G > mMaxWaitCountsForScan5G){
								Log.e(TAG,"scan 5G timeout");
								mWaitCountsForScanWlan5G = 0;

								if(mBTConfig.getWlanBand_2G() == 1){
									Log.e(TAG,"mBTConfig.getWlanBand_2G() == 1");
									Cmd4BTConfigThread.sendCommonCmd(BTConfigThread.CMD_START_SCAN_WLAN_2G, MainActivity.mMainAct.mBTConfigHandler);
								}
								else{
									MainActivity.mIsDoingScanWlan = false;
									MainActivity.mMainAct.mUIHandler.sendEmptyMessage(UIMsgHandler.MSG_SHOW_SCAN_TIMEOUT);
								}

							}

							return;
						}
						//receiving wlan AP 5G results
						else if(mBTConfigState == BTConfigState.STATE_BT_RECEIVE_WLAN_5G){
							mWaitCountsForScanWlan5G = 0;
							return;

						}
						//finish receiving wlan AP 5G results
						else if(mBTConfigState == BTConfigState.STATE_BT_SCAN_WLAN_5G_END){
							mWaitCountsForScanWlan5G = 0;
							if(!mBTConfig.getWlanScanResults(1).isEmpty()){
								MainActivity.mMainAct.mUIHandler.sendEmptyMessage(UIMsgHandler.MSG_UPDATE_WLAN_5G);
							}

							if(mBTConfig.getWlanBand_2G() != 1){

								MainActivity.mMainAct.mUIHandler.sendEmptyMessage(UIMsgHandler.MSG_DISMISS_SCAN_DIALOG);

								Cmd4BTConfigThread.sendCommonCmd(BTConfigThread.CMD_START_SCAN_WLAN_5G, MainActivity.mMainAct.mBTConfigHandler);

							}else{
								MainActivity.mMainAct.mUIHandler.sendEmptyMessage(UIMsgHandler.MSG_DISMISS_SCAN_DIALOG);
								Cmd4BTConfigThread.sendCommonCmd(BTConfigThread.CMD_START_SCAN_WLAN_2G, MainActivity.mMainAct.mBTConfigHandler);
							}

							return;

						}
						//send remote AP profile
						else if(mBTConfigState == BTConfigState.STATE_BT_SEND_WLAN_PROFILE){
							mWaitCountsForAckAPConnect++;
							if(mWaitCountsForAckAPConnect >= mMaxWaitCountsForAckAPConnect){
								Log.e(TAG,"send btConfig timeout");
								mWaitCountsForAckAPConnect = -2;
								if(MainActivity.mMainAct.mRemoteAPBuf == null)
									return;
								else{
									 Cmd4BTConfigThread.sendAPProfileCmd(MainActivity.mMainAct.mRemoteAPBuf, MainActivity.mMainAct.mBTConfigHandler);
								}
							}

						}
						//receive ACK for connecting to remote AP
						else if(mBTConfigState == BTConfigState.STATE_BT_SEND_WLAN_PROFILE_END){
							mWaitCountsForAckAPConnect = 0;
							MainActivity.mWaitForConnectACK = false;
							MainActivity.mWaitForConnectACK_2 = true;

							Cmd4BTConfigThread.sendCommonCmd( BTConfigThread.CMD_QUERY_REPEATER_STATUS,MainActivity.mMainAct.mBTConfigHandler);

						}
						//check repeater status
						else if(mBTConfigState == BTConfigState.STATE_BT_QUERY_REPEATER_STATUS){
							if(!mIsNoResponse){
								Cmd4BTConfigThread.sendCommonCmd( BTConfigThread.CMD_QUERY_REPEATER_STATUS,MainActivity.mMainAct.mBTConfigHandler);
							}


							//if(GlobalConfig.CONFIG_OFFLINE_DECTECTION){
							if(!GlobalConfig.CONFIG_OFFLINE_DECTECTION){
								if(mIsNoResponse){
									/* show wait repeater onLine dialog: choose adjust and the position adjustment dialog is dismiss */
									if(MainActivity.mChoiceForAdjust == MainActivity.POSITION_ADJUSTMENT && MainActivity.mNeedShowWaitOnlineDialog)
										MainActivity.mMainAct.mUIHandler.sendEmptyMessage(UIMsgHandler.MSG_SHOW_WAIT_ONLINE);
									return;
								}

								/* when choose adjust position, check if the repeater is no response, when detect the repeater no response, just update the extend AP UI */
								if(MainActivity.mChoiceForAdjust == MainActivity.POSITION_ADJUSTMENT){
									mWaitForRepeaterResponse++;
									if(mWaitForRepeaterResponse > mMaxWaitCountsForResponse){ // if no response for checking status over 5000ms
										Log.d(TAG,"1. detect BTConfig.STATE_BT_REPEATER_NO_RESPONSE");

										mWaitForRepeaterResponse = 0;
										mIsNoResponse = true;

										/* set repeater to disconnect status*/
										MainActivity.mMainAct.mUIHandler.sendEmptyMessage(UIMsgHandler.MSG_NO_RESPONSE);

										/* show wait repeater on-line dialog */
										if(MainActivity.mNeedShowWaitOnlineDialog)
											MainActivity.mMainAct.mUIHandler.sendEmptyMessage(UIMsgHandler.MSG_SHOW_WAIT_ONLINE);
										return;
									}
								}
							}

						}
						//reply for check repeater status
						else if(mBTConfigState == BTConfigState.STATE_BT_QUERY_REPEATER_STATUS_END){
							mWaitForRepeaterResponse = 0;
							mIsNoResponse = false;

							MainActivity.mMainAct.mUIHandler.sendEmptyMessage(UIMsgHandler.MSG_UPDATE_EXTENDED_AP);

							Cmd4BTConfigThread.sendCommonCmd( BTConfigThread.CMD_QUERY_REPEATER_STATUS, MainActivity.mMainAct.mBTConfigHandler);

						}
						//repeater is offline
						else if(mBTConfigState == BTConfigState.STATE_BT_REPEATER_OFFLINE){

							MainActivity.mWaitForBandResult = false;
							MainActivity.mWaitForConnectACK = false;

							if(MainActivity.mMainAct.mClickBackToBTUI)
								return;

							if(GlobalConfig.CONFIG_OFFLINE_DECTECTION){

								if(MainActivity.mMainAct.mActiveCloseSocket){
									Log.d(TAG,"active close");
									return;
								}

								Cmd4BTConfigThread.sendCommonCmd( BTConfigThread.CMD_CLOSE_BTSOCKET,
										MainActivity.mMainAct.mBTConfigHandler);

								MainActivity.mMainAct.mUIHandler.sendEmptyMessage(UIMsgHandler.MSG_NO_RESPONSE);

								/* position adjustment dialog is showing */
								if(MainActivity.mUIState == UIState.STATE_UI_SHOW_CHOICE_FOR_ADJUSTMENT)
									return;

								/* show wait repeater onLine dialog */
								if(MainActivity.mChoiceForAdjust == MainActivity.POSITION_ADJUSTMENT && MainActivity.mNeedShowWaitOnlineDialog)
									MainActivity.mMainAct.mUIHandler.sendEmptyMessage(UIMsgHandler.MSG_SHOW_WAIT_ONLINE);

								if(mRepeaterRedetect){ // have discovered the repeater again
									Log.d(TAG,"reOnline-----connect cmd ");
									Cmd4BTConfigThread.sendConnectBTCmd(MainActivity.mSavedBTMAC,
											MainActivity.mMainAct.mBTConfigHandler);
									return;
								}else{ // haven't discovery the repeater, continue scanning

									if(mBTConfig.flag){
										MainActivity.mMainAct.mBTConfig.getBTBle().doBTScan(false);
										checkRepeaterReOnline_gatt(mBTConfig);
									}else{
										MainActivity.mMainAct.mBTConfig.getBTRfComm().doBTScan(false);
										checkRepeaterReOnline(mBTConfig);
									}


								}
							}else{
								MainActivity.mMainAct.mUIHandler.sendEmptyMessage(UIMsgHandler.MSG_BT_DISCONNECT);
							}

						}

						else if(mBTConfigState == BTConfigState.STATE_AP_SCAN_WLAN_END){
							mWaitCountsForScanWlan2G++;
							if( mWaitCountsForScanWlan2G > 5 ){
								MainActivity.mMainAct.mUIHandler.sendEmptyMessage(UIMsgHandler.MSG_UPDATE_APLIST);
								mWaitCountsForScanWlan2G = 0;
							}

							return;
						}


						else if(mBTConfigState == BTConfigState.STATE_BT_DISCONNECT){
							MainActivity.mMainAct.mUIHandler.sendEmptyMessage(UIMsgHandler.MSG_SHOW_HOME_BUTTON);

						}

					}
				};

				if(mCheckStateTimer == null){
					mCheckStateTimer = new Timer();
					mCheckStateTimer.schedule(mCheckStateTimerTask, 1000, mTimerCheckInterval);
				}

			}
		}
	}//startCheckStatusTimer

	public void stopCheckStatusTimer(){
		if(mCheckStateTimerTask != null){
			mCheckStateTimerTask.cancel();
			mCheckStateTimerTask = null;
		}

		if(mCheckStateTimer != null){
			mCheckStateTimer.cancel();
			mCheckStateTimer = null;
		}
	}//stopCheckStatusTimer
	
	private void checkRepeaterReOnline(BTConfig mBTConfig){
		List<ScanObj> btScanList = BTScanReceiver.getBTScanResults();
		mRepeaterRedetect = false;
		for(int i = 0; i < btScanList.size(); i++){
			String scanBTMac = (String)btScanList.get(i).getMac();
			if(scanBTMac.equals(MainActivity.mSavedBTMAC)){
				MainActivity.mMainAct.mBTConfig.getBTRfComm().cancelBTScan();
				mRepeaterRedetect = true;
				break;
			}
		} // for match
	}
	
	private void checkRepeaterReOnline_gatt(BTConfig mBTConfig){
		List<ScanObj> btScanList = mBTConfig.getBleScanResults();
		mRepeaterRedetect = false;
		for(int i = 0; i < btScanList.size(); i++){
			String scanBTMac = btScanList.get(i).getMac();
			if(scanBTMac.equals(MainActivity.mSavedBTMAC)){
				MainActivity.mMainAct.mBTConfig.getBTBle().cancelBTScan();
				mRepeaterRedetect = true;
				break;
			}
		} // for match
	}
	
	private void resetWaitCount(){
		mWaitCountsForResponseWlanBand	= 0;
		mWaitCountsForScanWlan2G 		= 0;
		mWaitCountsForScanWlan5G		= 0;
		mWaitCountsForAckAPConnect		= 0;
		mWaitForRepeaterResponse		= 0;
	}
	
}

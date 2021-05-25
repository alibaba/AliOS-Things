package com.rtk.btconfigbluetooth.BTConfig;

import java.util.ArrayList;
import java.util.List;

import com.rtk.btconfigbluetooth.BTRfComm.BTRfComm;
import com.rtk.btconfigbluetooth.BTBle.BTBle;
import com.rtk.btconfigbluetooth.ScanResults.ScanObj;
import com.rtk.libbtconfigutil.APClass;
import com.rtk.libbtconfigutil.BTConfigUtil;

import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.os.Handler;
import android.util.Log;

public class BTConfig {
	public final static boolean flag = true; //ble:true rfcomm:false
	
	public static boolean onSend = false;
	
	public final static byte MSG_UPDATE_BTLIST	 			= 0x01;
	private String TAG = "BTConfig";
	private Context mContext;
	static BTConfig mBTConfig;
	private static Handler mUIHandler;
	
	private int mBTConfigState = -1;
	
	public static final int mTTL_count = 5;
	
	public static int mHomeAP_encrypt = 0;
	public static int mHomeAP_band = 0;
	
	public static int check_HomeAP = 1;
	public static byte[] Check_HomeAP_BSSID;	
	public static boolean wifi_list_ready = false;
	public static int bt_state = -1;
	
	private BTRfComm mBTRfComm;
	private static BTBle mBTBle;
	private Runnable mScanBTRunable;
	
	private String mBTDeviceMac;
	private BTConnectThread mBTConnectThread;
	
	private BTReceiveThread mReceiveThread;
	
	/* scan AP results */
	private List<ScanObj> mWlanAPList_2G = new ArrayList<ScanObj>();
	private List<ScanObj> mWlanAPList_5G = new ArrayList<ScanObj>();
	
	/* extended AP */
	private List<ScanObj> mExtendAPList = new ArrayList<ScanObj>();
	
	public static BTConfigUtil BTConfigLib = new BTConfigUtil();

	public BTConfig(Handler uihandler, Context context){
		mBTConfig = this;
		mUIHandler = uihandler;
		mContext = context;		
		if(flag){
			initBTBle();
		}else{
			initBTRfComm();
		}	
	}
	
	private void initBTBle(){
		if(mBTBle == null)
			mBTBle = new BTBle(mContext, this);
	}
	
	/** init BTRfComm and btAdapter **/
	private void initBTRfComm(){
		if(mBTRfComm == null)
			mBTRfComm = new BTRfComm();
	}
	
	
	public BTBle getBTBle(){
		return mBTBle;
	}
	
	/**	return BTRfComm instance  **/
	public BTRfComm getBTRfComm(){
		return mBTRfComm;
	}
	
	
	/**	return BTConfig state  **/
	public int getBTConfigState(){
		return mBTConfigState;
	}
	public static void update_btlist(){
		if(mUIHandler != null)
			mUIHandler.sendEmptyMessage(MSG_UPDATE_BTLIST);
	}
	
	public static List<ScanObj> getBleScanResults(){
		if(mBTBle != null)
			return mBTBle.getBleScanResults();
		return null;
	}
	
	public void setBTConfigState(int state) {
		mBTConfigState = state;
	}
	
	// runnable: scan Bluetooth device runnable
	private void initScanBTRunnable(){
		if(mScanBTRunable != null){
			return;
		}
		else{
    		mScanBTRunable = new Runnable() {
				@Override
				public void run() {
					if(flag){
						mBTBle.resetBTScanResults();
						mBTBle.doBTScan(true);
					}else{
						mBTRfComm.doBTScan(true);
					}
					
				}	
    		};
    	} 
	}
	
	
	// start scan bt device
	public void startBTScan(){
		Log.e(TAG,"startBTScan");
		if(mUIHandler == null){
		
			if(flag){
				mBTBle.resetBTScanResults();
				mBTBle.doBTScan(true);
			}else{
			
				mBTRfComm.doBTScan(true);
			}
		}else{
			initScanBTRunnable();
			mUIHandler.post(mScanBTRunable);
		}
	}
	
	
	// start the connection thread to repeater
	public void startConnect(String btDeviceMac){
		mBTConfigState = BTConfigState.STATE_BT_CONNECTING;
		mBTDeviceMac = btDeviceMac;
		
		if(flag){
			mBTBle.cancelBTScan();
		}else{			
			mBTRfComm.cancelBTScan();
		}
		
		if(mBTConnectThread == null){
			mBTConnectThread = new BTConnectThread();
			mBTConnectThread.start();
		}else{
			mBTConnectThread.onThreadResume();
		}
	}
	
	// cancel current connection process to repeater
	public void pauseConnect(){
		if(mBTConnectThread != null)
			mBTConnectThread.onThreadPause();
	}
	
	
	// close the BluetoothSocket
	public boolean closeBTSocket(){
	
		if(flag){
			return mBTBle.closeBTSocket();
		}else{			
			return mBTRfComm.closeBTSocket();
		}
		
	}
	
	// disconnect the Bluetooth
	public boolean disconnectBTSocket(){
		if(flag){
			return mBTBle.disconnectBTSocket();
		}
		else{
			return true;
		}			
	}
	
	// return Bluetooth MAC address of repeater
	public String getBTDeviceMac(){
		return mBTDeviceMac;
	}
	
	
	//  start the receiving thread
	public void startReceive(){
		if(mBTConfigState < BTConfigState.STATE_BT_CONNECT_OK){
			return;
		}else{
			if(mReceiveThread == null){
				mReceiveThread = new BTReceiveThread();
				mReceiveThread.start();
			}else{
				mReceiveThread.awake();
			}
		}
	}	

		
	//	check whether has connected to repeater successfully
	private boolean checkBTConnectState()
	{
		if(mBTConfigState < BTConfigState.STATE_BT_CONNECT_OK)
			return false;
		return true;
	}
	
	/**
	 * cmd to repeater
	 */
	// query band capability
	public void queryWlanBand(){
		if(checkBTConnectState()==false)
			return;
		
		mBTConfigState = BTConfigState.STATE_BT_QUERY_WLAN_BAND;
		
		if(flag){
			mBTBle.sendBTMessage(BTConfigLib.construct_get_wlan_band_cmd());
		}else{
			mBTRfComm.sendBTMessage(BTConfigLib.construct_get_wlan_band_cmd());
		}	
	}
	
	// scan WiFi AP: 2.4G
	public void doSiteSurvey_2G(){
		if(checkBTConnectState()==false)
			return;
		Log.w(TAG, "doSiteSurvey_2G");
		
		if(flag){
			int ret = mBTBle.sendBTMessage(BTConfigLib.construct_site_survery_2G_cmd());
			if(ret != -1){
				mBTConfigState = BTConfigState.STATE_BT_SCAN_WLAN_2G;
			}
		}else{
			mBTConfigState = BTConfigState.STATE_BT_SCAN_WLAN_2G;
			mBTRfComm.sendBTMessage(BTConfigLib.construct_site_survery_2G_cmd());
		}
	}
	
	
	// scan WiFi AP: 5G 
	public void doSiteSurvey_5G(){
		if(checkBTConnectState()==false)
			return;
		Log.w(TAG, "doSiteSurvey_5G");
		
		if(flag){
			int ret = mBTBle.sendBTMessage(BTConfigLib.construct_site_survery_5G_cmd());
			if(ret != -1){
				mBTConfigState = BTConfigState.STATE_BT_SCAN_WLAN_5G;
			}
		}else{
			mBTConfigState = BTConfigState.STATE_BT_SCAN_WLAN_5G;
			mBTRfComm.sendBTMessage(BTConfigLib.construct_site_survery_5G_cmd());
		}
		
		
	}
	
	// send remote AP profile
	public void sendAPProfile(byte[] APProfile){

		if(checkBTConnectState()==false)
			return;

		byte[] sendBuff = BTConfigLib.construct_AP_profile_cmd(APProfile,APProfile.length);
		if(sendBuff != null){
			
			mBTConfigState = BTConfigState.STATE_BT_SEND_WLAN_PROFILE;
			
			if(flag){
				int retry_cnt = 3; //re-write 
				int ret;
				while(retry_cnt-- > 0){
					if((ret = mBTBle.sendBTMessage(sendBuff)) == 1) //write success
						break;
					try{
						Thread.sleep(100); 
					}catch (InterruptedException e){
						e.printStackTrace();
					}
				}
				
			}else{
				mBTRfComm.sendBTMessage(sendBuff);
			}		
		}
		else{			
			Log.i(TAG,"sendBuff == null");
		}
	}
	
	// check repeater connection status
	public void queryRepeaterStatus(){
		if(checkBTConnectState()==false)
			return;
			
		mBTConfigState = BTConfigState.STATE_BT_QUERY_REPEATER_STATUS;
		
		if(flag){
			mBTBle.sendBTMessage(BTConfigLib.construct_check_repeater_status_cmd());
		}else{
			mBTRfComm.sendBTMessage(BTConfigLib.construct_check_repeater_status_cmd());
		}		
	}
	
	/**
	 * get results after parsing response
	 */
	// get 2.4G band support capability
	public int getWlanBand_2G(){
		byte ret = BTConfigLib.get_band_support_2G_result();
		Log.i(TAG,"BTConfigLib.so get_band_support_2G = "+ret);
		return ret;
	}
	
	// get 5G band support capability
	public int getWlanBand_5G(){
		byte ret = BTConfigLib.get_band_support_5G_result();
		Log.i(TAG,"BTConfigLib.so get_band_support_5G = "+ret);
		return ret;
	}
	
	// get product type
	public int getProductType(){
		byte ret = BTConfigLib.get_product_type();
		//Log.i(TAG,"BTConfigLib.so get_product_type = "+ret);
		return ret;
	}
	
	// return 2.4G/5G scan results in List<RTK_APClass> format
	 public List<ScanObj> getWlanScanResults(int wlan2GOr5G){
		 mWlanAPList_2G.clear();
		 mWlanAPList_5G.clear();
		 
		 if(wlan2GOr5G == 0){
			 Log.e(TAG,"2G");
			
			 APClass APs[] = BTConfigLib.get_AP_scan_2G_results();
			 if(APs == null)
				 return mWlanAPList_2G;
			 
			 for(int i = 0; i < APs.length; i++){
				 APClass tmpAP = APs[i];
				 mWlanAPList_2G.add(new ScanObj(tmpAP.getSSID(), tmpAP.getMac(), tmpAP.getRssi()-100, tmpAP.getEncrpytType(), mTTL_count, (byte) 0));
			 }
			 
			 return mWlanAPList_2G;
		 }else if(wlan2GOr5G == 1){
			 Log.e(TAG,"5G");
			 
			 APClass APs[] = BTConfigLib.get_AP_scan_5G_results();
			 if(APs == null)
				 return mWlanAPList_5G;
			 
			 for(int i = 0; i < APs.length; i++){
				 APClass tmpAP = APs[i];
				 mWlanAPList_5G.add(new ScanObj(tmpAP.getSSID(), tmpAP.getMac(), tmpAP.getRssi()-100, tmpAP.getEncrpytType(), mTTL_count, (byte) 1));
			 }
			 
			 return mWlanAPList_5G;
		 }else
			 return null;
	 }
	
	//	return the extended AP information in List<RTK_APClass> format
	public List<ScanObj> getExtendAPObjs(){
		APClass[] extendedAP = BTConfigLib.get_repeater_status_results();
		
		//Log.d("MainActivity","===========================================");
		//Log.d("MainActivity","SSID  : "+extendedAP[0].getSSID());
		//Log.d("MainActivity","MAC   : "+extendedAP[0].getMac());
		//Log.d("MainActivity","con st: "+extendedAP[0].getConnectStatus());
		//Log.d("MainActivity","RSSI  : "+(extendedAP[0].getRssi() -100));
		//Log.d("MainActivity","cfg st: "+extendedAP[0].getConfigureStatus());
		
		ScanObj tmpExtendedAP = new ScanObj(extendedAP[0].getSSID(),extendedAP[0].getMac(), extendedAP[0].getRssi() -100,
				extendedAP[0].getEncrpytType(), extendedAP[0].getConnectStatus(), extendedAP[0].getConfigureStatus()); 
		mExtendAPList.clear();
		mExtendAPList.add(tmpExtendedAP);
		return mExtendAPList;
	}
	
}

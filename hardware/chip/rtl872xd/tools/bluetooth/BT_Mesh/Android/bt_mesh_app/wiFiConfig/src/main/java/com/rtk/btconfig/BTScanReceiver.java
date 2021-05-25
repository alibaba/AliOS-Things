package com.rtk.btconfig;

import java.util.ArrayList;
import java.util.List;

import com.rtk.btconfigbluetooth.ScanResults.ScanObj;

import android.annotation.SuppressLint;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class BTScanReceiver extends BroadcastReceiver{
	
	private final String BT_PRODUCTION_1 = "realtek_rpt";
	private final String BT_PRODUCTION_2 = "Ameba";
	
	private String TAG = "BTScanReceiver";
	private static List<ScanObj> mBTScanResults 	= new ArrayList<ScanObj>();
	
	@SuppressLint("NewApi")
	@Override
	public void onReceive(Context context, Intent intent) {

		String action = intent.getAction();
		if(BluetoothAdapter.ACTION_DISCOVERY_STARTED.equals(action)){
			Log.w(TAG,"ACTION_DISCOVERY_STARTED");
			resetBTScanResults();
			return;

		}else if(BluetoothAdapter.ACTION_DISCOVERY_FINISHED.equals(action)){
			Log.w(TAG,"ACTION_DISCOVERY_FINISHED");
			return;

		}
		else if (BluetoothDevice.ACTION_FOUND.equals(action)) {
			try {
				boolean isOurProduction = false;
				BluetoothDevice mBTScanDevice = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
				short mBTDeviceRSSI_short = intent.getExtras().getShort(BluetoothDevice.EXTRA_RSSI);
				byte mBTDeviceRSSI_byte = (byte)(mBTDeviceRSSI_short);
				if(mBTDeviceRSSI_byte < -100)
					mBTDeviceRSSI_byte = -100;
				else if(mBTDeviceRSSI_byte > 0){
					mBTDeviceRSSI_byte = (byte)(mBTDeviceRSSI_byte -100);
				}

				//filter our production
				String deviceName = mBTScanDevice.getName();
				Log.d(TAG,"================ "+deviceName+" ===================");

				if(deviceName.length()>0){
					if(deviceName.indexOf(BT_PRODUCTION_1)>=0)isOurProduction=true;
					if(deviceName.indexOf(BT_PRODUCTION_2)>=0)isOurProduction=true;
				}

				if(deviceName.indexOf(BT_PRODUCTION_2)>=0)        // for Ameba
                {
                        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.JELLY_BEAN_MR2) {
                                int deviceType = 0;
                                deviceType = mBTScanDevice.getType();
                                if (deviceType != BluetoothDevice.DEVICE_TYPE_CLASSIC) {        // only show classic BT entries, no BLE
                                        isOurProduction=false;
                                }
                        }
                }


				if(isOurProduction){
					if(mBTScanDevice.getBondState() == BluetoothDevice.BOND_NONE){//unPaired list
						if(mBTScanDevice.getName() != null){
							updateBTScanResults(mBTScanDevice.getName(), mBTScanDevice.getAddress(), mBTDeviceRSSI_byte);
							MainActivity.mMainAct.mUIHandler.sendEmptyMessage(UIMsgHandler.MSG_UPDATE_BTLIST);
						}
					}else if(mBTScanDevice.getBondState() == BluetoothDevice.BOND_BONDED){//paired list
						if(mBTScanDevice.getName() != null){
							updateBTScanResults(mBTScanDevice.getName(), mBTScanDevice.getAddress(), mBTDeviceRSSI_byte);
							MainActivity.mMainAct.mUIHandler.sendEmptyMessage(UIMsgHandler.MSG_UPDATE_BTLIST);
						}
					}
				}

			} catch (Exception e) {
				e.printStackTrace();
			}
		} //ACTION_FOUND

	}
	
	/** clear Bluetooth scan results**/
	private void resetBTScanResults(){
		if( !mBTScanResults.isEmpty() )
			mBTScanResults.clear();
	}
	
	/** judge If Bluetooth has scanned the btDevice already, and update the BTScanResults ArrayList	**/
	private void updateBTScanResults(String btSSID, String btMAC, byte btRSSI){
		boolean hasContainBTDevice = false;
		if(!mBTScanResults.isEmpty()){
			for(int i = 0; i < mBTScanResults.size(); i++){
				String tmpBTSsid = (String)mBTScanResults.get(i).getSSID();
				String tmpBTMac  = (String)mBTScanResults.get(i).getMac();
				if(btSSID.equals(tmpBTSsid) && btMAC.equals(tmpBTMac)){
					hasContainBTDevice = true;
					break;
				}
			}
		}

		if(!hasContainBTDevice){
			mBTScanResults.add( new ScanObj(btSSID, btMAC, btRSSI) );
		}
	}
	
	
	/**	get scan Bluetooth results	**/
	public static List<ScanObj> getBTScanResults(){
		return mBTScanResults;
	}
	
}

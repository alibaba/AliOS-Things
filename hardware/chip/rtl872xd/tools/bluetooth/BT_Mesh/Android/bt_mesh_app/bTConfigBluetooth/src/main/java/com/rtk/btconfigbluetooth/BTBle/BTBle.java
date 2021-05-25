package com.rtk.btconfigbluetooth.BTBle;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.UUID;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;


import android.annotation.TargetApi;
import android.app.AlertDialog;
import android.app.Application;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.BluetoothSocket;
import android.bluetooth.le.BluetoothLeScanner;

import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanResult;
import android.content.Context;
import android.content.DialogInterface;
import android.content.pm.PackageManager;
import android.os.Build;
import android.util.Log;


import com.rtk.btconfigbluetooth.ScanResults.ScanObj;
import com.rtk.btconfigbluetooth.BTConfig.BTConfig;
import com.rtk.btconfigbluetooth.BTConfig.BTConfigState;
import com.rtk.btconfigbluetooth.BTConfig.BTReceiveThread;


public class BTBle  {
	private String TAG = "BTBle";
	private int mBTBleState = -1;
	private BluetoothAdapter mBTAdapter;
	private Context mContext;
	private BTConfig mBTconfig;
	private static final int PERMISSION_REQUEST_COARSE_LOCATION = 1;

	public final static String ACTION_GATT_CONNECTED =
			"com.example.bluetooth.le.ACTION_GATT_CONNECTED";
	public final static String ACTION_GATT_DISCONNECTED =
			"com.example.bluetooth.le.ACTION_GATT_DISCONNECTED";
	public final static String ACTION_GATT_SERVICES_DISCOVERED =
			"com.example.bluetooth.le.ACTION_GATT_SERVICES_DISCOVERED";
	public final static String ACTION_DATA_AVAILABLE =
			"com.example.bluetooth.le.ACTION_DATA_AVAILABLE";
	public final static String EXTRA_DATA =
			"com.example.bluetooth.le.EXTRA_DATA";

	private BluetoothLeScanner mBTAdapterBluetoothLeScanner;
	
	
	/* Bluetooth connect state */
	public static final int STATE_BT_CONNECT_ONGOING 		= 0x01;
	public static final int STATE_BT_CONNECT_OK 	 		= 0x02;
	public static final int STATE_BT_CONNECT_FAIL 	 		= 0x03;
	
	/* read/write the InputStream/OutputStream */
	public static final int STATE_BT_STREAM_NULL 	= -2;
	public static final int STATE_BT_STREAM_FAIL 	= -1; 
	public static final int STATE_BT_STREAM_OK 		= 1;

	private final String BT_PRODUCTION_1 = "realtek_rpt";
	private final String BT_PRODUCTION_2 = "Ameba";

	UUID  BTCONF_SERVICE_UUID = UUID.fromString("0000ff01-0000-1000-8000-00805f9b34fb");
	UUID  BTCONF_BLECONFIG_UUID = UUID.fromString("00002a0d-0000-1000-8000-00805f9b34fb");

	private BluetoothGattCharacteristic mBLEConfigCharacteristic = null;

	private static List<ScanObj> mBTScanResults = new ArrayList<ScanObj>();
	private BluetoothGatt bluetoothGatt;

	public static byte[] mReceiveBuf;

	public BTBle(Context context, BTConfig btconfig){
		final BluetoothManager bluetoothManager = (BluetoothManager) context.getSystemService( Context.BLUETOOTH_SERVICE);
		mBTAdapter = bluetoothManager.getAdapter();
		mBTAdapterBluetoothLeScanner = mBTAdapter.getBluetoothLeScanner();
		mContext = context;
		mBTconfig = btconfig;
	}

	public static List<ScanObj> getBleScanResults(){
		return mBTScanResults;
	}
	
	/** get BluetoothAdapter status	**/
	public int getBTAdapterStatus(){
		if(mBTAdapter != null){ //not support BluetoothAdapter
			if (!mBTAdapter.isEnabled()) {
				return 0;
			}else{
				return 1;
			}
		}else
			return -1;
	}

	public void resetBTScanResults() {
		if( !mBTScanResults.isEmpty() )
			mBTScanResults.clear();
	}

	
	/**	do Bluetooth scan **/
	public void doBTScan(boolean newScan){
		if(mBTAdapter == null)
			return;

		mBTAdapterBluetoothLeScanner = mBTAdapter.getBluetoothLeScanner();

		if(newScan == true)
		{
			if(mBTAdapter.isDiscovering()){
				mBTAdapterBluetoothLeScanner.stopScan(BTBleScanCallback);
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
		}
		else
		{
			if(mBTAdapter.isDiscovering())
				return;
		}
		
		Log.w(TAG,"doBTScan");
		
				
		// disconnect GATT
		if (bluetoothGatt != null){
			bluetoothGatt.disconnect();
			Log.w(TAG,"bluetoothGatt.disconnect");
		}

		mBTAdapterBluetoothLeScanner.startScan(BTBleScanCallback);

	}

	public static String bytesToHexString(byte[] src) {
		StringBuilder stringBuilder = new StringBuilder("");
		if (src == null || src.length <= 0) {
			return null;
		}
		for (int i = 0; i < src.length; i++) {
			int v = src[i] & 0xFF;
			String hv = Integer.toHexString(v);
			if (hv.length() < 2) {
				stringBuilder.append(0);
			}
			stringBuilder.append(hv);
		}
		return stringBuilder.toString();
	}

	private ScanCallback BTBleScanCallback = new ScanCallback() {
		@Override
		public void onScanResult(int callbackType, ScanResult result) {
			try {
				boolean isOurProduction = false;
				BluetoothDevice mBTScanDevice = result.getDevice();
				short mBTDeviceRSSI_short =(short) result.getRssi();
				byte mBTDeviceRSSI_byte = (byte)(mBTDeviceRSSI_short);
				if(mBTDeviceRSSI_byte < -100)
					mBTDeviceRSSI_byte = -100;
				else if(mBTDeviceRSSI_byte > 0){
					mBTDeviceRSSI_byte = (byte)(mBTDeviceRSSI_byte -100);
				}

				//filter our production
				String deviceName = mBTScanDevice.getName();
				//Log.w(TAG, "onScanResult: name: " + mBTScanDevice.getName() +", address: " + mBTScanDevice.getAddress() +", rssi: " + result.getRssi());

				if(deviceName != null){
					if(deviceName.indexOf(BT_PRODUCTION_1)>=0) {
						isOurProduction = true;
					}
					if(deviceName.indexOf(BT_PRODUCTION_2)>=0) {
						isOurProduction = true;
					}

					if(deviceName.indexOf(BT_PRODUCTION_2)>=0)        // for Ameba
					{
						if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.JELLY_BEAN_MR2) {
							int deviceType = 0;
							deviceType = mBTScanDevice.getType();
							if (deviceType != BluetoothDevice.DEVICE_TYPE_LE) {        // only show classic BT entries, no BLE
								isOurProduction=false;
							}
						}
					}

					if(isOurProduction){
						//Log.e(TAG,"isOurProduction!");

						if(mBTScanDevice.getBondState() == BluetoothDevice.BOND_NONE){//unPaired list
			
							if(mBTScanDevice.getName() != null){
								updateBTScanResults(mBTScanDevice.getName(), mBTScanDevice.getAddress(), mBTDeviceRSSI_byte);
								BTConfig.update_btlist();
							}
						}else if(mBTScanDevice.getBondState() == BluetoothDevice.BOND_BONDED){//paired list
				
							if(mBTScanDevice.getName() != null){
								updateBTScanResults(mBTScanDevice.getName(), mBTScanDevice.getAddress(), mBTDeviceRSSI_byte);
								BTConfig.update_btlist();
							}
						}
					}
				}
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		
		
		
		@Override
		public void onBatchScanResults(List<ScanResult> results) {
			super.onBatchScanResults(results);
		}

		@Override
		public void onScanFailed(int errorCode) {
			super.onScanFailed(errorCode);
		}
	};

	// Device connect call back
	private final BluetoothGattCallback btleGattCallback = new BluetoothGattCallback() {

		
		public void onMtuChanged(BluetoothGatt gatt, int mtu, int status){
		}

		@Override
		public void onCharacteristicChanged(BluetoothGatt gatt, final BluetoothGattCharacteristic characteristic) {
			// this will get called anytime you perform a read or write characteristic operation
		}

		@Override
		public void onConnectionStateChange(final BluetoothGatt gatt, final int status, final int newState) {
			// this will get called when a device connects or disconnects
			//Log.w(TAG,"onConnectionStateChange " + newState);

			switch (newState) {
				case BluetoothProfile.STATE_DISCONNECTED: //device disconnected
					Log.e(TAG,"device disconnected");
					Log.w(TAG,"gatt.close.");
					gatt.close();

					mBTconfig.bt_state = mBTconfig.getBTConfigState();
	
					mBTconfig.setBTConfigState(BTConfigState.STATE_BT_DISCONNECT);
					
					
					break;
				case BluetoothProfile.STATE_CONNECTED: //device connected
					Log.w(TAG,"device connected");
					Log.w(TAG,"Start to discover services.");
					// Discovers services offered by a remote device as well as their characteristics and descriptors.
					gatt.discoverServices();

					break;
				default:
					Log.e(TAG,"device encounterned an unknown state");

					break;
			}
		}

		@Override
		public void onServicesDiscovered(final BluetoothGatt gatt, final int status) {
		// this will get called after the client initiates a BluetoothGatt.discoverServices() call
			
			if (status == BluetoothGatt.GATT_SUCCESS) {
				
				if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP){             
					gatt.requestMtu(185);   
	            }

				for (BluetoothGattService service : gatt.getServices()) {										
					if (service.getUuid().equals(BTCONF_SERVICE_UUID)){						
			            for (BluetoothGattCharacteristic characteristic : service.getCharacteristics()) {			            	
			            	if (characteristic != null){			            		
			            		if(characteristic.getUuid().equals(BTCONF_BLECONFIG_UUID)) {
			            			mBLEConfigCharacteristic = characteristic;			            			
			            			mBTconfig.setBTConfigState(BTConfigState.STATE_BT_CONNECT_OK);
			            		}
			            	}
			            	else{			            		
			            		mBTconfig.setBTConfigState(BTConfigState.STATE_BT_CONNECT_FAIL);
			            	}
			            }
					}
					else{
						mBTconfig.setBTConfigState(BTConfigState.STATE_BT_CONNECT_FAIL);
					}
		        }
			}
			else{
				mBLEConfigCharacteristic = null;			
			}
		}

		@Override
		// Result of a characteristic read operation
		public void onCharacteristicRead(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {

			if (status == BluetoothGatt.GATT_SUCCESS) {
				
				byte[] characteristicValueBytes = characteristic.getValue();
				if (characteristicValueBytes != null){
					mReceiveBuf = new byte[characteristicValueBytes.length];
					System.arraycopy(characteristicValueBytes, 0, mReceiveBuf, 0, characteristicValueBytes.length);
				}
			}
			else{				
				Log.w(TAG,"onCharacteristicRead fail: " + status);
			}

		}

		@Override
		public  void  onCharacteristicWrite(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
			if (status == BluetoothGatt.GATT_SUCCESS) {			
			}
			else{
				Log.w(TAG,"onCharacteristicWrite fail: " + status);						
			}
			BTConfig.onSend = false;
		}
	};

	
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

	/**	cancel Bluetooth scan **/
	public void cancelBTScan(){
		if(mBTAdapter != null){
			if(mBTAdapterBluetoothLeScanner != null)
			{
				mBTAdapterBluetoothLeScanner.stopScan(BTBleScanCallback);
			}
		}else
			return;
	}


	/** btBleConnectMethod **/
	private boolean btBleConnectMethod(BluetoothDevice btDevice)
	{

		closeBTSocket();

		try {

			bluetoothGatt = btDevice.connectGatt(mContext, false, btleGattCallback);
			if(bluetoothGatt.getDevice().equals(btDevice)) {
				return true;
			}else {
				return false;
			}
		} catch (Exception e2) {
			e2.printStackTrace();

			return false;
		}

	}

	/**	btConnectvia BLE **/
	public boolean doBTBleConnect(String btMac){
		if(btMac == null){
			mBTBleState = STATE_BT_CONNECT_FAIL;
			return false;
		}
		mBTBleState = STATE_BT_CONNECT_ONGOING;

		BluetoothDevice btDevice = mBTAdapter.getRemoteDevice(btMac);
		
		if(btDevice == null){
			mBTBleState = STATE_BT_CONNECT_FAIL;
			return false;
		}

		boolean connectState = btBleConnectMethod(btDevice);
		
		if(connectState){			
			return true;
			
		}else{
			mBTBleState = STATE_BT_CONNECT_FAIL;
			return false;
		}
	}
	
	
	/**	send Message through OutputStream of BluetoothSocket	**/
	public int sendBTMessage(byte[] sendBuf){

		if(mBLEConfigCharacteristic == null){
			return STATE_BT_STREAM_FAIL;
		}
		
		if(mBLEConfigCharacteristic.getUuid().equals(BTCONF_BLECONFIG_UUID)){

			BTConfig.onSend = true;
			
			try{
				Thread.sleep(200);
			}catch (InterruptedException e){
				e.printStackTrace();
			}

			mBLEConfigCharacteristic.setValue(sendBuf);
			if(bluetoothGatt.writeCharacteristic(mBLEConfigCharacteristic)){

				return STATE_BT_STREAM_OK;
			}
			else {

				return STATE_BT_STREAM_FAIL;
			}

		}
		else{
			return STATE_BT_STREAM_NULL;
			
		}

	}
	
	/**	get response through InputStream of  BluetoothSocket	**/
	public int getBTResponse(){

		if(mBLEConfigCharacteristic == null){
			return STATE_BT_STREAM_FAIL;
		}
		
		if(mBLEConfigCharacteristic.getUuid().equals(BTCONF_BLECONFIG_UUID)){
	
			try{
				Thread.sleep(150); 
			}catch (InterruptedException e){
				e.printStackTrace();
			}						
			bluetoothGatt.readCharacteristic(mBLEConfigCharacteristic);
		}
	
		if(mReceiveBuf == null){
			return STATE_BT_STREAM_FAIL;
		}
		return mReceiveBuf.length;			
	}

	
	/** close BluetoothSocket  **/
	public boolean closeBTSocket(){

		if (bluetoothGatt != null) {		
			bluetoothGatt.close();
			bluetoothGatt = null;
					
		}
		return true;
	}
	
	public boolean disconnectBTSocket(){

		if (bluetoothGatt != null){
			bluetoothGatt.disconnect();
		}

		return true;
	}
		
}

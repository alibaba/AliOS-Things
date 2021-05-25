package com.rtk.btconfigbluetooth.BTRfComm;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.util.Log;

public class BTRfComm {
	private String TAG = "BTRfComm";
	private int mBTRfcommState = -1;
	private BluetoothAdapter mBTAdapter;
	
	private BluetoothSocket mBTSocket;
	private OutputStream mBTSendStream;
	private InputStream mBTReceiveStream;
	
	/* Bluetooth connect state */
	public static final int STATE_BT_CONNECT_ONGOING 		= 0x01;
	public static final int STATE_BT_CONNECT_OK 	 		= 0x02;
	public static final int STATE_BT_CONNECT_FAIL 	 		= 0x03;
	
	/* read/write the InputStream/OutputStream */
	public static final int STATE_BT_STREAM_NULL 	= -2;
	public static final int STATE_BT_STREAM_FAIL 	= -1; //һ���� < 0 !
	public static final int STATE_BT_STREAM_OK 		= 1;
	
	public BTRfComm(){
		mBTAdapter = BluetoothAdapter.getDefaultAdapter();
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
	
	/**	do Bluetooth scan **/
	public void doBTScan(boolean newScan){
		if(mBTAdapter == null)
			return;
		
		if(newScan==true)
		{
			if(mBTAdapter.isDiscovering()){
				mBTAdapter.cancelDiscovery();
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
		
		Log.d(TAG,"doBTScan");
		mBTAdapter.startDiscovery();
	}
	
	/**	cancel Bluetooth scan **/
	public void cancelBTScan(){
		if(mBTAdapter != null){
			if(mBTAdapter.isDiscovering()){
				Log.d(TAG,"cancelBTScan");
				mBTAdapter.cancelDiscovery();
			}
		}else
			return;
	}
	
	
	/** btConnectRfcommMethod1 **/
	private boolean btRfcommConnectMethod1(BluetoothDevice btDevice)
    {
		String BT_SPP_UUID_STR 	= 	"00001101-0000-1000-8000-00805F9B34FB";
		UUID BT_SPP_UUID 		= 	UUID.fromString(BT_SPP_UUID_STR) ;
		boolean isCloseBTSocketOK = closeBTSocket();
		if(!isCloseBTSocketOK){
			Log.e(TAG,"closeBTSocket fail");
			return false;
		}else{
			Log.d(TAG,"btConnectRfcommMethod1");
			try {
	    		mBTSocket = btDevice.createRfcommSocketToServiceRecord(BT_SPP_UUID);
	    		mBTSocket.connect();
	    		Log.e(TAG,"BT Connect OK");
				return true;
			} catch (Exception e1) {
				e1.printStackTrace();
				Log.e(TAG,"Connect fail originally");

				return btRfcommConnectMethod2(btDevice);
			} // catch
		}// else
    }
	
    
	/** btConnectRfcommMethod2: reflect method	**/
	private boolean btRfcommConnectMethod2(BluetoothDevice btDevice)
    {
		boolean isCloseBTSocketOK = closeBTSocket();
		if(!isCloseBTSocketOK){
			Log.e(TAG,"closeBTSocket fail");
			return false;
		}else{
			Log.d(TAG,"btConnectRfcommMethod2");
			try {
				mBTSocket = (BluetoothSocket)btDevice.getClass().getMethod("createRfcommSocket", new Class[] {int.class}).invoke(btDevice,1);
				mBTSocket.connect();
				return true;
			} catch (Exception e2) {
				e2.printStackTrace();
				try {
					if(mBTSocket != null){
						mBTSocket.close();
						mBTSocket = null;
					}
				} catch (IOException e) {
					e.printStackTrace();
					return false;
				}
				return false;
			}//catch
		}//else
    }
    
	
	/**	btConnectvia RFCOMM **/
	public boolean doBTRfCommConnect(String btMac){
		if(btMac == null){
			mBTRfcommState = STATE_BT_CONNECT_FAIL;
			return false;
		}
		mBTRfcommState = STATE_BT_CONNECT_ONGOING;
		
		BluetoothDevice btDevice = mBTAdapter.getRemoteDevice(btMac);
		
		boolean connectState = btRfcommConnectMethod1(btDevice);
		if(connectState){
			mBTRfcommState = STATE_BT_CONNECT_OK;
			try {
				mBTSendStream = mBTSocket.getOutputStream();
				mBTReceiveStream = mBTSocket.getInputStream();
			} catch (IOException e) {
				e.printStackTrace();
				mBTSendStream 	  = null;
				mBTReceiveStream = null;
			}
			return true;
		}else{
			mBTRfcommState = STATE_BT_CONNECT_FAIL;
			return false;
		}
	}
	
	
	/**	send Message through OutputStream of BluetoothSocket	**/
	public int sendBTMessage(byte[] sendBuf){
		if(mBTSendStream == null){
			Log.e(TAG,"mBTSendStream == null");
			return STATE_BT_STREAM_NULL; 
		}else{
			try {
				mBTSendStream.write(sendBuf);
				return STATE_BT_STREAM_OK;
			} catch (IOException e) {
				e.printStackTrace();
				return STATE_BT_STREAM_FAIL;
			}
		}
		
	}
	
	/**	get response through InputStream of  BluetoothSocket	**/
	public int getBTResponse(byte[] receiveBuf){
		if(mBTReceiveStream == null){
			Log.e(TAG,"mBTReceiveStream == null");
			return STATE_BT_STREAM_NULL;
		}else{
			try {
				int len = mBTReceiveStream.read(receiveBuf, 0, receiveBuf.length);
				return len;
			} catch (IOException e) {
				e.printStackTrace();
				//closeBTSocket();
				return STATE_BT_STREAM_FAIL;
			}
		}//else
	}
	
	
	/**	return RfComm state	**/
//	public int getBTRfCommState(){
//		return mBTRfcommState;
//	}
	
	/** close BluetoothSocket  **/
	public boolean closeBTSocket(){
		
		try {
			if(mBTSendStream != null){
				mBTSendStream.close();
				mBTSendStream = null;
			}
			if(mBTReceiveStream != null){
				mBTReceiveStream.close();
				mBTReceiveStream = null;
			}
			
			if(mBTSocket != null){
				mBTSocket.close();
				mBTSocket = null;
			}
			return true;
		} catch (IOException e) {
			e.printStackTrace();
			return false;
		}
	}
	
	
}

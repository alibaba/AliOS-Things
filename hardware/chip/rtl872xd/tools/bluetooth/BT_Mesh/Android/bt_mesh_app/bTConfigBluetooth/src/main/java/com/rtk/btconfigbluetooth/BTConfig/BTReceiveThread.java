package com.rtk.btconfigbluetooth.BTConfig;

import com.rtk.btconfigbluetooth.BTRfComm.BTRfComm;
import com.rtk.btconfigbluetooth.BTBle.BTBle;

import android.util.Log;

/**	The thread for receiving data from repeater and parsing the data **/
public class BTReceiveThread extends Thread{
	private String TAG = "BTReceiveThread";
	private boolean mIsPause = false;
	private int onSend_counter = 0;
	
	/** awake the thread **/
	public synchronized void awake(){
		this.notify();
	}
	
	/** make the thread wait **/
	public void onThreadWait(){		
		try {  
            synchronized (this) {  
                this.wait();  
            }  
        } catch (Exception e) {  
            e.printStackTrace();  
        }  
	}

	/**  receive message from repeater **/
	@Override
	public void run() {
		while(!mIsPause && !isInterrupted()){

			if(BTConfig.flag){
				
				if(BTConfig.mBTConfig.onSend){ //write action 

					try{
						Thread.sleep(150); 
						onSend_counter ++;
					}catch (InterruptedException e){
						e.printStackTrace();
					}	
					
					if(onSend_counter > 30){ //3s
						BTConfig.mBTConfig.onSend = false;
						onSend_counter = 0;
					}
					
					continue;
				}
			
				int len = BTConfig.mBTConfig.getBTBle().getBTResponse();

				
				if(len == BTBle.STATE_BT_STREAM_NULL){

					BTConfig.mBTConfig.setBTConfigState(BTConfigState.STATE_BT_REPEATER_OFFLINE);

					onThreadWait();
				
				}else{
					if(len == BTBle.STATE_BT_STREAM_FAIL){
						continue;
					}
					else{
						// parse the data received from repeater
						parseReceiveData(BTBle.mReceiveBuf, BTConfig.mBTConfig.getBTConfigState());		
					}
				}
			
			}else{
			
				byte[] temp = new byte[10*1024];

				int len = BTConfig.mBTConfig.getBTRfComm().getBTResponse(temp);
			
				if(len == BTRfComm.STATE_BT_STREAM_FAIL){
					BTConfig.mBTConfig.setBTConfigState(BTConfigState.STATE_BT_REPEATER_OFFLINE);
				
					onThreadWait();
				}else{
					byte[] temp_buf = new byte[len];
					System.arraycopy(temp, 0, temp_buf, 0, len);
				
				// parse the data received from repeater
				parseReceiveData(temp_buf, BTConfig.mBTConfig.getBTConfigState()); 
			
			}
		
			}
		}
	}

	/** parse the data received from wifi's client **/
	private void parseReceiveData(byte[] buf,  int btConfigState){

		if(btConfigState == BTConfigState.STATE_BT_QUERY_WLAN_BAND){
	
			int ret = BTConfig.BTConfigLib.parse_wlan_band_reply(buf, buf.length);
			if(ret == 1){				
				BTConfig.mBTConfig.setBTConfigState(BTConfigState.STATE_BT_QUERY_WLAN_BAND_END);
				
				if(BTConfig.flag){
					onThreadWait();
				}
			}
			
		}
		else if(btConfigState == BTConfigState.STATE_BT_SCAN_WLAN_2G){
			
			if(BTConfig.flag){

				int ret = BTConfig.BTConfigLib.parse_AP_results_2G_reply_GATT(BTConfig.check_HomeAP, BTConfig.Check_HomeAP_BSSID, buf, buf.length);
				if(ret == 1){
					BTConfig.wifi_list_ready = true;
					BTConfig.mBTConfig.setBTConfigState(BTConfigState.STATE_BT_SCAN_WLAN_2G_END);					
					onThreadWait();
				}else if(ret == 0){
					BTConfig.mBTConfig.setBTConfigState(BTConfigState.STATE_BT_RECEIVE_WLAN_2G);
				}else{
					BTConfig.mHomeAP_band = 0;
					if(ret == 2){
						BTConfig.mHomeAP_encrypt = 0;
						BTConfig.mBTConfig.setBTConfigState(BTConfigState.STATE_BT_SCAN_WLAN_2G_HOMEAP);
						onThreadWait();
					}
					else if(ret == 3){
						BTConfig.mHomeAP_encrypt = 1;
						BTConfig.mBTConfig.setBTConfigState(BTConfigState.STATE_BT_SCAN_WLAN_2G_HOMEAP);
						onThreadWait();
					}
					else if(ret == 4){
						BTConfig.mHomeAP_encrypt = 2;
						BTConfig.mBTConfig.setBTConfigState(BTConfigState.STATE_BT_SCAN_WLAN_2G_HOMEAP);
						onThreadWait();
					}
					
				}
				
				
			}else{
				int ret = BTConfig.BTConfigLib.parse_AP_results_2G_reply(buf, buf.length);
				if(ret == 1){
					BTConfig.wifi_list_ready = true;
					BTConfig.mBTConfig.setBTConfigState(BTConfigState.STATE_BT_SCAN_WLAN_2G_END);
				}else if(ret == 0){
					BTConfig.mBTConfig.setBTConfigState(BTConfigState.STATE_BT_RECEIVE_WLAN_2G);
				}
				
			}		
			
		}else if(btConfigState == BTConfigState.STATE_BT_RECEIVE_WLAN_2G){
						
			if(BTConfig.flag){

				int ret = BTConfig.BTConfigLib.parse_AP_results_2G_reply_GATT(BTConfig.check_HomeAP, BTConfig.Check_HomeAP_BSSID, buf, buf.length);				
				if(ret == 1){
					BTConfig.wifi_list_ready = true;
					BTConfig.mBTConfig.setBTConfigState(BTConfigState.STATE_BT_SCAN_WLAN_2G_END);
					onThreadWait();
				}else if(ret == 0){
					BTConfig.mBTConfig.setBTConfigState(BTConfigState.STATE_BT_RECEIVE_WLAN_2G);
				}else{
					BTConfig.mHomeAP_band = 0;
					if(ret == 2){
						BTConfig.mHomeAP_encrypt = 0;
						BTConfig.mBTConfig.setBTConfigState(BTConfigState.STATE_BT_SCAN_WLAN_2G_HOMEAP);
						onThreadWait();
					}
					else if(ret == 3){
						BTConfig.mHomeAP_encrypt = 1;
						BTConfig.mBTConfig.setBTConfigState(BTConfigState.STATE_BT_SCAN_WLAN_2G_HOMEAP);
						onThreadWait();
					}
					else if(ret == 4){
						BTConfig.mHomeAP_encrypt = 2;
						BTConfig.mBTConfig.setBTConfigState(BTConfigState.STATE_BT_SCAN_WLAN_2G_HOMEAP);
						onThreadWait();
					}
					
				}
				
			}else{
				int ret = BTConfig.BTConfigLib.parse_AP_results_2G_reply(buf, buf.length);
				if(ret == 1){
					BTConfig.wifi_list_ready = true;
					BTConfig.mBTConfig.setBTConfigState(BTConfigState.STATE_BT_SCAN_WLAN_2G_END);
				}else if(ret == 0){
					BTConfig.mBTConfig.setBTConfigState(BTConfigState.STATE_BT_RECEIVE_WLAN_2G);
				}
				
			}
			
		}
		else if(btConfigState == BTConfigState.STATE_BT_SCAN_WLAN_5G){
			
			if(BTConfig.flag){
				int ret = BTConfig.BTConfigLib.parse_AP_results_5G_reply_GATT(BTConfig.check_HomeAP, BTConfig.Check_HomeAP_BSSID, buf, buf.length);

				if(ret == 1){
					BTConfig.wifi_list_ready = true;
					BTConfig.mBTConfig.setBTConfigState(BTConfigState.STATE_BT_SCAN_WLAN_5G_END);
					onThreadWait();
				}else if(ret == 0){
					BTConfig.mBTConfig.setBTConfigState(BTConfigState.STATE_BT_RECEIVE_WLAN_5G);
				}else{
					BTConfig.mHomeAP_band = 1;
					if(ret == 2){
						BTConfig.mHomeAP_encrypt = 0;
						BTConfig.mBTConfig.setBTConfigState(BTConfigState.STATE_BT_SCAN_WLAN_5G_HOMEAP);
						onThreadWait();
					}
					else if(ret == 3){
						BTConfig.mHomeAP_encrypt = 1;
						BTConfig.mBTConfig.setBTConfigState(BTConfigState.STATE_BT_SCAN_WLAN_5G_HOMEAP);
						onThreadWait();
					}
					else if(ret == 4){
						BTConfig.mHomeAP_encrypt = 2;
						BTConfig.mBTConfig.setBTConfigState(BTConfigState.STATE_BT_SCAN_WLAN_5G_HOMEAP);
						onThreadWait();
					}
				}
			}else{
				int ret = BTConfig.BTConfigLib.parse_AP_results_5G_reply(buf, buf.length);
				if(ret == 1){
					BTConfig.wifi_list_ready = true;
					BTConfig.mBTConfig.setBTConfigState(BTConfigState.STATE_BT_SCAN_WLAN_5G_END);
				}else if(ret == 0){
					BTConfig.mBTConfig.setBTConfigState(BTConfigState.STATE_BT_RECEIVE_WLAN_5G);
				}				
			}	
		}
		else if(btConfigState == BTConfigState.STATE_BT_RECEIVE_WLAN_5G){
			
			if(BTConfig.flag){
				int ret = BTConfig.BTConfigLib.parse_AP_results_5G_reply_GATT(BTConfig.check_HomeAP, BTConfig.Check_HomeAP_BSSID, buf, buf.length);

				if(ret == 1){
					BTConfig.wifi_list_ready = true;
					BTConfig.mBTConfig.setBTConfigState(BTConfigState.STATE_BT_SCAN_WLAN_5G_END);
					onThreadWait();
				}else if(ret == 0){
					BTConfig.mBTConfig.setBTConfigState(BTConfigState.STATE_BT_RECEIVE_WLAN_5G);
				}else{
					BTConfig.mHomeAP_band = 1;
					if(ret == 2){
						BTConfig.mHomeAP_encrypt = 0;
						BTConfig.mBTConfig.setBTConfigState(BTConfigState.STATE_BT_SCAN_WLAN_5G_HOMEAP);
						onThreadWait();
					}
					else if(ret == 3){
						BTConfig.mHomeAP_encrypt = 1;
						BTConfig.mBTConfig.setBTConfigState(BTConfigState.STATE_BT_SCAN_WLAN_5G_HOMEAP);
						onThreadWait();
					}
					else if(ret == 4){
						BTConfig.mHomeAP_encrypt = 2;
						BTConfig.mBTConfig.setBTConfigState(BTConfigState.STATE_BT_SCAN_WLAN_5G_HOMEAP);
						onThreadWait();
					}
					
				}	
			
			}else{
				int ret = BTConfig.BTConfigLib.parse_AP_results_5G_reply(buf, buf.length);
				if(ret == 1){
					BTConfig.wifi_list_ready = true;
					BTConfig.mBTConfig.setBTConfigState(BTConfigState.STATE_BT_SCAN_WLAN_5G_END);
				}else if(ret == 0){
					BTConfig.mBTConfig.setBTConfigState(BTConfigState.STATE_BT_RECEIVE_WLAN_5G);
				}									
			}
			
		}
		else if(btConfigState == BTConfigState.STATE_BT_SEND_WLAN_PROFILE){
			int ret = BTConfig.BTConfigLib.parse_AP_profile_ACK_reply(buf, buf.length);
			if(ret == 1){
				BTConfig.mBTConfig.setBTConfigState(BTConfigState.STATE_BT_SEND_WLAN_PROFILE_END);
				
				if(BTConfig.flag){
					onThreadWait();
				}
			}
			
		}
		else if(btConfigState == BTConfigState.STATE_BT_QUERY_REPEATER_STATUS){
			int ret = BTConfig.BTConfigLib.parse_repeater_status_reply(buf, buf.length);
			if(ret == 1){
				BTConfig.mBTConfig.setBTConfigState(BTConfigState.STATE_BT_QUERY_REPEATER_STATUS_END);
				
				if(BTConfig.flag){
					onThreadWait();
				}
			}
		}
		else if(btConfigState == -1 || btConfigState == BTConfigState.STATE_BT_DISCONNECT){
			if(BTConfig.flag){onThreadWait();}
		}
		else{
			return;
		}
			
	 }//parseReceiveData
	
}//BTReceiveThread

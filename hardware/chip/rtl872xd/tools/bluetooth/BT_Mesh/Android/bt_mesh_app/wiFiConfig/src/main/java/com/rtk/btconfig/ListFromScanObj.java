package com.rtk.btconfig;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import com.rtk.btconfigbluetooth.ScanResults.ScanObj;

import android.net.wifi.WifiManager;
import android.util.Log;

public class ListFromScanObj {
	public String TAG = "ListFromScanObj";
	
	/** signal strength	**/
	private final byte WIFI_GRADE1 						= 	0x01;
	private final byte WIFI_GRADE2 						= 	0x02;
	private final byte WIFI_GRADE3 						= 	0x03;
	private final byte WIFI_GRADE4 						= 	0x04;
	private final byte[] RSSI_STRENGTH_ARRAY 			= 	{WIFI_GRADE1, WIFI_GRADE2, WIFI_GRADE3, WIFI_GRADE4};
	/** get strength according rssi **/
	private byte parseBTStrength(int rssi){
		byte strength = RSSI_STRENGTH_ARRAY[WifiManager.calculateSignalLevel(rssi,RSSI_STRENGTH_ARRAY.length)];
		return strength;
	}
	
	/** * parse RSSI to signal strength  * */
	public byte parseWifiStrength(int rssi){
		byte wifiStrength = -1;
		
		int highRssi = -35;
		int lowRssi  = -65; 
		
		int high_low_interval = highRssi - lowRssi;
		int middle1 = lowRssi + high_low_interval/6; // 1/6
		int middle2 = middle1 + high_low_interval/3; // 1/2
//		int middle3 = middle2 + high_low_interval/6;
		
		if(rssi < lowRssi){
			wifiStrength = WIFI_GRADE1;
		}else if(rssi >= lowRssi && rssi < middle1){
			wifiStrength = WIFI_GRADE2;
		}else if(rssi >= middle1 && rssi < middle2){
			wifiStrength = WIFI_GRADE3;
		}else if(rssi >= middle2 && rssi < highRssi){
			wifiStrength = WIFI_GRADE4;
		}else{
			wifiStrength = WIFI_GRADE4;
		}
		
		return wifiStrength;
	}
	
	/**	get btScanResults, and parse the results to arraylist, and set it to UI	**/
	public List<HashMap<String,Object>> getArrayList(List<ScanObj> scanResults, int mListType){
		List<HashMap<String,Object>> itemList = new  ArrayList<HashMap<String,Object>>();
		for(ScanObj scanItem: scanResults) {	//2.4G	
			 HashMap<String,Object> map_item = new HashMap<String,Object>();
			 map_item.put("list_item_ssid",  scanItem.getSSID());
			 map_item.put("list_item_mac",  scanItem.getMac());
			 
			 byte strength;
			/* repeater status */ 
			 if(mListType == ListBaseAdapter.LIST_EXTENDED_AP){ 
				 byte connectStatus = scanItem.getConnectStatus();
				 if(scanItem.getMac().equals("00:00:00:00:00:00"))
					 map_item.put("list_item_mac",  "");
				 
				 if(connectStatus != 4)
				 {
					 map_item.put("list_item_rssi",   "");
					 map_item.put("list_item_strength", R.drawable.strength_grade0);
					 map_item.put("list_item_status", R.drawable.disconnect);
				 }else{
					 map_item.put("list_item_rssi",   scanItem.getRssi()+"dBm");
					 strength = parseWifiStrength( scanItem.getRssi() );
					 map_item.put("list_item_strength", getStrengthDrawable(strength));
					 map_item.put("list_item_status", R.drawable.disconnect);
				 }
				
				 switch(scanItem.getEncrpytType()){
				 case 0:
					 map_item.put("list_item_encrypt", R.drawable.transparent);
					 break;
				 case 1:
				 case 2:
					 map_item.put("list_item_encrypt", android.R.drawable.ic_secure);
					 break;
				 default:
					 map_item.put("list_item_encrypt", R.drawable.unkown);
				 }
				 
			 }
			/* Bluetooth list/wifi AP list */
			 else{ 
				 map_item.put("list_item_rssi",   scanItem.getRssi() +"dBm");
				 if(mListType == ListBaseAdapter.LIST_SCAN_BT){ // bt list
					 strength = parseBTStrength( scanItem.getRssi() );
				 }else{ // wlan list
					 strength = parseWifiStrength( scanItem.getRssi() );
					 
					 switch(scanItem.getEncrpytType()){
					 case 0:
						 map_item.put("list_item_encrypt", R.drawable.transparent);
						 break;
					 case 1:
					 case 2:
						 map_item.put("list_item_encrypt", android.R.drawable.ic_secure);
						 break;
					 default:
						 map_item.put("list_item_encrypt", R.drawable.unkown);
					 }
 
				 }
				 map_item.put("list_item_strength", getStrengthDrawable(strength));
				 
			 }
			 itemList.add(map_item);
		}
		 return itemList;
	 }
	 
	/** get Drawable due to strengthGrade **/
	 public int getStrengthDrawable(byte strength){
		 switch(strength){
		 case WIFI_GRADE1:
			 return R.drawable.strength_grade1;
		 case WIFI_GRADE2:
			 return R.drawable.strength_grade2;
		 case WIFI_GRADE3:
			 return R.drawable.strength_grade3;
		 case WIFI_GRADE4:
			 return R.drawable.strength_grade4;	 
		 default:
			 return R.drawable.strength_grade0;
		 }
	 }
		
		
}

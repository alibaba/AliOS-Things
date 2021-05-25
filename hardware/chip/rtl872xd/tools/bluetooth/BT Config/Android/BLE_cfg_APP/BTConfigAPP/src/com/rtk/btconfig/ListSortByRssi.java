package com.rtk.btconfig;

import java.util.Comparator;

import android.util.Log;

import com.rtk.btconfigbluetooth.ScanResults.ScanObj;

public class ListSortByRssi implements Comparator<ScanObj>{

	@Override
	public int compare(ScanObj obj0, ScanObj obj1) {
		// Auto-generated method stub
		int rssi0 = obj0.getRssi();
		int rssi1 = obj1.getRssi();
		
		if(MainActivity.AP_Profile_queue.size() > 0 ){
			for(int i = MainActivity.AP_Profile_queue.size() - 1; i >= 0; i--){
				if(MainActivity.AP_Profile_queue.toArray()[i].equals(obj0.getMac())){
					return -1;
				}
				else if(MainActivity.AP_Profile_queue.toArray()[i].equals(obj1.getMac())){
					return 1;
				}
			}
		}			

		if(rssi0 > rssi1){
			return -1;
		}else if(rssi0 == rssi1){
			return 0;
		}
		else{
			return 1;
		}
			
	}
	
}

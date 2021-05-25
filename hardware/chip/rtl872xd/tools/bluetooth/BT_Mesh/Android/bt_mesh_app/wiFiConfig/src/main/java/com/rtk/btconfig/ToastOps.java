package com.rtk.btconfig;

import android.annotation.SuppressLint;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.rtk.Configuration.GlobalConfig;

public class ToastOps {

	/** set toast view **/
    @SuppressLint("InflateParams") 
    private static View getToastView(String text){
    	LayoutInflater inflater = LayoutInflater.from(MainActivity.mMainAct);
    	View view = inflater.inflate(R.layout.toast_view, null);
    	TextView toast_text = (TextView)view.findViewById(R.id.toastWarningText);
    	toast_text.setText(text);
    	return view;
    }
    
    /** get toast**/
	public static Toast getToast(String text){
    	Toast toast = new Toast(MainActivity.mMainAct);
    	toast.setGravity(Gravity.CENTER_HORIZONTAL|Gravity.CENTER_VERTICAL, 0, 0);
    	toast.setDuration(Toast.LENGTH_LONG);
    	toast.setView(getToastView(text));
    	return toast;
    }
	
	public static void createToast(){
		if(MainActivity.mToastBTWaitOnLine == null)
			MainActivity.mToastBTWaitOnLine = ToastOps.getToast(GlobalConfig.DEVICE_NAME +" is offline, please wait...");
      	if(MainActivity.mToastBTNoResponse == null)
      		MainActivity.mToastBTNoResponse = ToastOps.getToast(GlobalConfig.DEVICE_NAME +" has no response, please wait...");
      	if(MainActivity.mToastBTDisconnected == null)
      		MainActivity.mToastBTDisconnected = ToastOps.getToast(GlobalConfig.DEVICE_NAME +" is disconnected...");
	}
	
	public static void cancelToast(){
		MainActivity.mToastBTNoResponse.cancel();
		MainActivity.mToastBTWaitOnLine.cancel();
		MainActivity.mToastBTDisconnected.cancel();
	}
	
}

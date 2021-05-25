package com.rtk.btconfig;

import java.util.HashMap;
import java.util.List;

import com.rtk.Configuration.GlobalConfig;


import android.content.Context;
import android.graphics.Color;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;


/***  
* Bluetooth/Wifi AP scan listView
*   
* @param listView  
*/  

public class ListBaseAdapter extends BaseAdapter {
	public final static int LIST_SCAN_BT				=	0x01; // bt
	public final static int LIST_SCAN_WLAN_AP			=	0x02; // wlan scan
	public final static int LIST_EXTENDED_AP			=	0x03; // extended AP
	
    private LayoutInflater mInflater;
    private List<HashMap<String, Object>> listDevs;
    private int listClass;
    
    public ListBaseAdapter(Context context, List<HashMap<String, Object>> getListDevs,int listClass) {
        mInflater = LayoutInflater.from(context);
        listDevs = getListDevs;
        this.listClass = listClass;
    }

    public int getCount() {
        return listDevs.size();
    }

    public Object getItem(int arg0) {
        return listDevs.get(arg0);
    }

    public long getItemId(int arg0) {
        return arg0;
    }
    
    public View getView(int position, View convertView, ViewGroup parent) {
    	if(listClass == LIST_SCAN_BT){
    		ViewHolder holder = null;
            if (convertView == null) {
                holder = new ViewHolder();
                convertView = mInflater.inflate(R.layout.bt_scan_list_item, parent, false);
                holder.ssidText = (TextView)convertView.findViewById(R.id.btSSID);
                holder.macText = (TextView)convertView.findViewById(R.id.btMAC);
                holder.rssiText = (TextView)convertView.findViewById(R.id.btRSSI);
                holder.profileText = (TextView)convertView.findViewById(R.id.bt_saved_profile);
                holder.strengthImage = (ImageView)convertView.findViewById(R.id.btStrength);
                convertView.setTag(holder);
            } else {
                holder = (ViewHolder)convertView.getTag();
            }

            String list_item_upper = (String)listDevs.get(position).get("list_item_ssid");
            String list_item_below = (String)listDevs.get(position).get("list_item_mac");
            String list_item_rssi =  (String)listDevs.get(position).get("list_item_rssi");
            int list_item_strength = (Integer)listDevs.get(position).get("list_item_strength");
            holder.ssidText.setText(list_item_upper);
            holder.macText.setText(list_item_below);
            holder.rssiText.setText(list_item_rssi);
            holder.profileText.setVisibility(View.INVISIBLE);
            holder.strengthImage.setBackgroundResource(list_item_strength);
            
            
            //check saved profile
            String preBTName = MainActivity.mSharePref.getString(GlobalConfig.KEY_PREVIOUSE_BT_NAME, GlobalConfig.DEFAULT_PREVIOUSE_BT_NAME);
			String preBTMac = MainActivity.mSharePref.getString(GlobalConfig.KEY_PREVIOUSE_BT_MAC, GlobalConfig.DEFAULT_PREVIOUSE_BT_MAC);
            if(list_item_upper.length()>0 && list_item_below.length()>0 && preBTName.length()>0 && preBTMac.length()>0){
            	//Log.i("MainActivity","scan :"+list_item_upper+"/"+list_item_below);
            	//Log.i("MainActivity","saved:"+preBTName+"/"+preBTMac);
            	
            	if(preBTName.equals(list_item_upper) && preBTMac.equals(list_item_below)){
            		holder.profileText.setVisibility(View.VISIBLE);
            	}
            }
            
            
    	}
    	
    	else if(listClass == LIST_SCAN_WLAN_AP){
    		WIFIViewHolder holder = null;
            if (convertView == null) {
                holder = new WIFIViewHolder();
                convertView = mInflater.inflate(R.layout.wlan_scan_list_item, parent, false);
                holder.ssidText = (TextView)convertView.findViewById(R.id.wifi_list_item_upper);
                holder.macText = (TextView)convertView.findViewById(R.id.wifi_list_item_below);
                holder.rssiText = (TextView)convertView.findViewById(R.id.wifi_list_item_rssi);
                holder.strengthImage = (ImageView)convertView.findViewById(R.id.wifi_list_item_strength);
                holder.encryptImage = (ImageView)convertView.findViewById(R.id.wifi_list_item_encrypt);
                convertView.setTag(holder);
            } else {
                holder = (WIFIViewHolder)convertView.getTag();
            }

            String list_item_upper = (String)listDevs.get(position).get("list_item_ssid");
            String list_item_below = (String)listDevs.get(position).get("list_item_mac");
            String list_item_rssi = (String)listDevs.get(position).get("list_item_rssi");
            int list_item_strength = (Integer)listDevs.get(position).get("list_item_strength");
            int list_item_encrypt = (Integer)listDevs.get(position).get("list_item_encrypt");
            holder.ssidText.setText(list_item_upper);
            holder.macText.setText(list_item_below);
            holder.rssiText.setText(list_item_rssi);
            holder.strengthImage.setBackgroundResource(list_item_strength);
            holder.encryptImage.setBackgroundResource(list_item_encrypt);
                        

			 if(MainActivity.AP_Profile_queue.size() > 0 ){
					for(int i = MainActivity.AP_Profile_queue.size() - 1; i >= 0; i--){
						if(MainActivity.AP_Profile_queue.toArray()[i].equals(list_item_below)){
							holder.ssidText.setTextColor(Color.rgb(0,0,162));
						}								
					}
			 }		
            
    	}else if(listClass == LIST_EXTENDED_AP){
    		WIFIConnectViewHolder holder = null;
            if (convertView == null) {
                holder = new WIFIConnectViewHolder();
                convertView = mInflater.inflate(R.layout.wlan_scan_list_item, parent, false);
                holder.ssidText = (TextView)convertView.findViewById(R.id.wifi_list_item_upper);
                holder.macText = (TextView)convertView.findViewById(R.id.wifi_list_item_below);
                holder.rssiText = (TextView)convertView.findViewById(R.id.wifi_list_item_rssi);
                holder.strengthImage = (ImageView)convertView.findViewById(R.id.wifi_list_item_strength);
                holder.encryptImage = (ImageView)convertView.findViewById(R.id.wifi_list_item_encrypt);
                holder.statusImage = (ImageView)convertView.findViewById(R.id.wifi_list_item_status);
                convertView.setTag(holder);
            } else {
                holder = (WIFIConnectViewHolder)convertView.getTag();
            }

            String list_item_upper = (String)listDevs.get(position).get("list_item_ssid");
            String list_item_below = (String)listDevs.get(position).get("list_item_mac");
            String list_item_rssi = (String)listDevs.get(position).get("list_item_rssi");
            int list_item_strength = (Integer)listDevs.get(position).get("list_item_strength");
            int list_item_encrypt = (Integer)listDevs.get(position).get("list_item_encrypt");
            int list_item_status = (Integer)listDevs.get(position).get("list_item_status");
            holder.ssidText.setText(list_item_upper);
            holder.macText.setText(list_item_below);
            holder.rssiText.setText(list_item_rssi);
            holder.strengthImage.setBackgroundResource(list_item_strength);
            holder.encryptImage.setBackgroundResource(list_item_encrypt);
            holder.statusImage.setBackgroundResource(list_item_status);
    	}
    	return convertView;
    }
    
    public final class ViewHolder {
        public TextView ssidText;
        public TextView macText;
        public TextView rssiText;
        public TextView profileText;
        public ImageView strengthImage;
    }
    
    public final class WIFIViewHolder {
        public TextView ssidText;
        public TextView macText;
        public TextView rssiText;
        public ImageView strengthImage;
        public ImageView encryptImage;
    }
    
    
    public final class WIFIConnectViewHolder {
        public TextView ssidText;
        public TextView macText;
        public TextView rssiText;
        public ImageView strengthImage;
        public ImageView encryptImage;
        public ImageView statusImage;
    }
    
}

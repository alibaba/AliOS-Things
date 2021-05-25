package com.rtk.btconfig;

import android.view.View;
import android.view.ViewGroup;
import android.widget.ListAdapter;
import android.widget.ListView;

public class ListViewHeight {
    /***  
     *  set the height of ListView dynamically, not allow the the scroll bar of listview itself to scroll 
     * @param listView  
     */    
    public void setListViewHeightBasedOnChildren(ListView listView) {    
        ListAdapter listAdapter = listView.getAdapter();    
        if (listAdapter == null) {    
            return;    
        }    
        int totalHeight = 0;    
        for (int i = 0; i < listAdapter.getCount(); i++) {    
            View listItem = listAdapter.getView(i, null, listView);    
            listItem.measure(0, 0);  //<span style="font-family: Helvetica, Tahoma, Arial, sans-serif; font-size: 14px; line-height: 25px; text-align: left; ">�ڻ�û�й���View ֮ǰ�޷�ȡ��View�Ķȿ� </span><span style="font-family: Helvetica, Tahoma, Arial, sans-serif; font-size: 14px; line-height: 25px; text-align: left; ">�ڴ�֮ǰ���Ǳ���ѡ measure һ��. </span><br style="font-family: Helvetica, Tahoma, Arial, sans-serif; font-size: 14px; line-height: 25px; text-align: left; ">  
            totalHeight += listItem.getMeasuredHeight();    
        }    
        ViewGroup.LayoutParams params = listView.getLayoutParams();    
        params.height = totalHeight    
                + (listView.getDividerHeight() * (listAdapter.getCount() - 1));    
        listView.setLayoutParams(params);    
    }
}

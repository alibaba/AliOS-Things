package com.rtk.btconfig;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;
import java.util.Timer;
import java.util.TimerTask;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.bluetooth.BluetoothAdapter;
import android.content.Context;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.Rect;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.wifi.ScanResult;
import android.net.wifi.SupplicantState;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.SystemClock;
import android.text.InputType;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnTouchListener;
import android.view.MotionEvent;
import android.view.ViewGroup.LayoutParams;
import android.view.Window;
import android.view.View.OnClickListener;
import android.view.inputmethod.InputMethodManager;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.rtk.Configuration.GlobalConfig;
import com.rtk.WiFi.Network;

import com.rtk.btconfig.UIMsgHandler;
import com.rtk.btconfigbluetooth.BTConfig.BTConfig;
import com.rtk.btconfigbluetooth.BTConfig.BTConfigState;
import com.rtk.btconfigbluetooth.ScanResults.ScanObj;
import com.rtk.libbtconfigutil.APClass;

import com.google.zxing.BarcodeFormat;
import com.google.zxing.EncodeHintType;
import com.google.zxing.WriterException;
import com.google.zxing.common.BitMatrix;
import com.google.zxing.qrcode.QRCodeWriter;
import com.google.zxing.qrcode.decoder.ErrorCorrectionLevel;


public class MainActivity extends BaseActivity
{

	final Context context = this;

	public static MainActivity mMainAct; 
	private static final int PERMISSION_REQUEST_COARSE_LOCATION = 1;
	/*********** define ******************/
	protected static final String TAG = "MainActivity";
	
	/*choice for adjust position */
	public static int POSITION_CANCEL	  		=  0;
	public static int POSITION_ADJUSTMENT 		=  1;
	public static int POSITION_DEFAULT	  		= -1;
	public static int mChoiceForAdjust	  		= POSITION_DEFAULT;
	public static int mPositionIndex 	  		= -1; //0: too close, 1: fine, 2: too far
	private boolean isCheckPostiion 			= false;
	
	public static String SharePrefFile = "data";
	
	/*********** Main component ******************/

	/* Button */
	private static ImageButton  mImgBtn_version;
	private static ImageButton 	mImgBtn_searchDevice;
	private static ImageButton 	mImgBtn_QRcode;

	private static ImageButton 	mImgBtn_info_setting;
	private static ImageButton 	mImgBtn_return;
	
	/* ListView */
	private static ListView		mBTScanListView;
	private ListView mWlanScanListView_2G;
	private ListView mWlanScanListView_5G;
	
	/*custom dialog component*/
	private Button mRssiSetting;
	private Button mClearBtn;
	
	private EditText mHighRssiEditText;
	private EditText mLowRssiEditText;
	
	LinearLayout parentLayout_ap_list_view;
	LinearLayout layout_linear;
	
	LinearLayout parentView;
		
	/*********** Main Data ******************/
	public static SharedPreferences mSharePref;
	
	/* List */
	private List<ScanObj> mBTScanObjs = new ArrayList<ScanObj>();
	private List<ScanObj> mWlanScanObjs_2G = new ArrayList<ScanObj>();
	private List<ScanObj> mWlanScanObjs_5G = new ArrayList<ScanObj>();
	public  List<ScanObj> mExtendAPScanObjs = new ArrayList<ScanObj>();
	private List<HashMap<String, Object>>  mWlanArrayList_2G;
	private List<HashMap<String, Object>>  mWlanArrayList_5G;
	private List<HashMap<String, Object>>  mExtendAPArrayList;
	private List<HashMap<String, Object>>  mBTArrayList;
	
	/* BaseAdapter */
	private BaseAdapter mBTListAdapter;
	private BaseAdapter mWlanAdapter_2G; 
	private BaseAdapter mWlanAdapter_5G; 
	private BaseAdapter mExtendAPAdapter; 
	
	/* toast */
	public static Toast mToastBTNoResponse;
	public static Toast mToastBTWaitOnLine;
	public static Toast mToastBTDisconnected;
	
	/* ProgressDialog */
	private ProgressDialog pd = null;
	
	/* Dialog */
	private AlertDialog 		deviceList_alert 		= null;
	private AlertDialog.Builder deviceList_builder 		= null;
	public AlertDialog 			alertDialog_pwd 		= null;
	public AlertDialog 			alertDialog_custom_dialog	= null;
	public AlertDialog.Builder	custom_dialog_2 		= null;
	
	/*********** variable ******************/
	/* remote AP profile */
	public byte[] mRemoteAPBuf;	
	public String mRemoteAPSSID;
	public String mRemoteAPMAC;
	
	private boolean mBTConnectedFlag 		    = false;/* connect to BT device */
	private boolean mAPConnectedFlag 		    = false;/* connect to remote AP */
	public static boolean mNeedShowAPConnectedDialog  = true; /* need show connect to AP OK dialog*/
	
	public boolean mBTConnectState = true;
	public boolean return_bnt_state = true;

	/*parse RSSI to position */
	public static com.rtk.UI.Rssi2Position Rssi2Position = new com.rtk.UI.Rssi2Position();
	
	public UIMsgHandler mUIHandler;
	public BTConfigStateTimer mCheckBTConfigStateTimer;
	public static int mUIState = -1;
	
	public BTConfig mBTConfig; 
	public Handler mBTConfigHandler;
	public BTConfigThread mBTConfigThread;
	
	/* close socket active */
	public boolean mClickBackToBTUI 			= false;
	public boolean mActiveCloseSocket 			= false;
	
	/* previous connected BT */
	public static String mSavedBTName;   
	public static String mSavedBTMAC;

	public static Handler handler_pd;
	
	private Timer wait_timer = null;
	
	/*  set ListView height dynamically */
	private static ListViewHeight mListViewHeigth = new ListViewHeight();
	/* parse scanobjs to ListView on UI */
	private static ListFromScanObj mParseScanObj = new ListFromScanObj();
	
	public static boolean mWaitForBandResult 	= false; 	/* wait for band support result*/
	public static boolean mWaitForConnectACK 	= false; 	/* wait for ACK of connecting to AP*/
	public static boolean mWaitForConnectACK_2 	= false; 	/* wait for ACK of connecting to AP*/
	public static boolean mIsDoingScanWlan   	= false; 	/* show scan wlan dialog */
	public static boolean mNeedShowWaitOnlineDialog = true; /* need show wait repeater onLine dialog*/
	public static boolean mIsDoingAdjustment 	= false; 	/* show position adjustment dialog */
	public static boolean mIsDoingConnectAP  	= false; 	/* show connect to AP dialog */
	public static boolean show_wifi_list 		= false;	/* show connect to AP tip msg */
	public static boolean show_wifi_list_2 		= false;	/* show connect to AP tip msg */
	public static boolean show_fail_msg 		= false;
	public static boolean isHomeAP_check 		= true;		/* when to show fast connect to home AP dialog */
	public static boolean isHomeAP_exist 		= false;	/* when to show scanning wifi list */
	
	public static boolean isDualBandWifiReady 	= false;	/* when to show 2.4G+5G wifi list */
	
	public String targetAP_password = "";
	public String targetAP_password_prefs = "";

	
	public TextView ap_info_name;
	public ImageView ap_info_rssi;
	public ImageView image_qrcode;
	
	private Button mConfirm_Config_Button;
	private Button mtry_newap_button;	
	
	private Rect rect;
	
	public static Queue<String> AP_Profile_queue = new LinkedList<String>();
	private int g_SingleChoiceID = -1;
	private boolean WifiConnected = false;
	private List<ScanResult> mScanResults;
	private ScanResult g_ScanResult;
	private static final int APNumber = 100;
	private List<ScanObj> mWlanAPList_2G = new ArrayList<ScanObj>();

	private static Network mNetwork = new Network();

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

//		setContentView(R.layout.activity_main);
//		requestWindowFeature(Window.FEATURE_NO_TITLE);
		mNetwork.WifiInit(this);

		initData();
		initBTConfig();
		initComponent();
		initComponentAction();

		checkBTAdapterStatus();
	}

	protected void onStart()
	{
		super.onStart();
		
	}
	
	@Override
	public void onPause() {
		// Auto-generated method stub
		super.onPause();
		Log.d(TAG,"onPause");
	}
	
	@Override
	protected void onStop() 
	{
		super.onStop();
		Log.d(TAG,"onStop");
		mBTConnectedFlag = false;
		mAPConnectedFlag = false;

	}
	
	@Override
	protected void onDestroy() 
	{
		super.onDestroy();
		Log.d(TAG,"onDestroy");
		android.os.Process.killProcess(android.os.Process.myPid());
	}

	/** exit button **/
    @Override
	public void onBackPressed() {

    	AlertDialog.Builder builder = new AlertDialog.Builder(mMainAct);
		builder.setTitle("Warning");
		builder.setMessage("Do you exit WiFiConfig?");
		builder.setPositiveButton("Yes", new DialogInterface.OnClickListener(){

			@Override
			public void onClick(DialogInterface dialog, int which) {
				onDestroy();
			}

		});
		builder.setNegativeButton("No",new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog,int id) {
				dialog.cancel();
			}
		});
		builder.setCancelable(false);
		builder.create().show();


	}

	private void initData() {

		mMainAct = this;
		mUIHandler = new UIMsgHandler();

		mSharePref = getSharedPreferences(SharePrefFile, 0);

		mBTConnectedFlag = false;
		mAPConnectedFlag = false;
	}

	private void initBTConfig()
	{
		if(mBTConfig == null)
			mBTConfig = new BTConfig(mUIHandler,this);

		if(mBTConfigThread == null){
			mBTConfigThread = new BTConfigThread("BTConfig");
			mBTConfigThread.hBTConfig = mBTConfig;
			mBTConfigThread.start();
        }

		if(mBTConfigHandler == null)
        	mBTConfigHandler = mBTConfigThread.getHandler();

		if(mCheckBTConfigStateTimer == null){
        	mCheckBTConfigStateTimer = new BTConfigStateTimer();
            mCheckBTConfigStateTimer.startCheckStatusTimer(mBTConfig);
        }
	}

	private void initComponent()
	{
		View wlanScanLayout = View.inflate(mMainAct, R.layout.wlan_scan_listview, null);


		mImgBtn_version			= (ImageButton) findViewById(R.id.btn_version);
		mImgBtn_searchDevice 	= (ImageButton) findViewById(R.id.btn_gosetting);
		mImgBtn_QRcode 			= (ImageButton) findViewById(R.id.btn_goQRcode);
		mImgBtn_info_setting 	= (ImageButton) findViewById(R.id.btn_info_setting);
		mImgBtn_return 			= (ImageButton) findViewById(R.id.btn_return);
		mImgBtn_return.setVisibility(View.GONE);

		layout_linear = new LinearLayout(mMainAct);
		parentLayout_ap_list_view = (LinearLayout) findViewById(R.id.layer1_ap_list_view);
		layout_linear.setLayoutParams(new LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT));

		mWlanScanListView_2G = (ListView) wlanScanLayout.findViewById(R.id.wlanScanListID_2G);	//2G list
		mWlanScanListView_5G = (ListView) wlanScanLayout.findViewById(R.id.wlanScanListID_5G);	//5G list
		parentLayout_ap_list_view.addView(wlanScanLayout, layout_linear.getLayoutParams());

	}

	private void initComponentAction()
	{

		mImgBtn_version.setOnClickListener(new OnClickListener(){
			@Override
			public void onClick(View v) {
				checkVersion_action();
			}
		});


		mImgBtn_searchDevice.setOnTouchListener(new OnTouchListener(){

			@Override
	        public boolean onTouch(View v, MotionEvent event) {

	            switch(event.getAction()) {
	                case MotionEvent.ACTION_DOWN:

	                	mImgBtn_searchDevice.setImageResource(R.drawable.btn_adddevice_click);
	                	// Construct a rect of the view's bounds
	                	rect = new Rect(v.getLeft(), v.getTop(), v.getRight(), v.getBottom());

	                    return true;

	                case MotionEvent.ACTION_UP:

	                	mImgBtn_searchDevice.setImageResource(R.drawable.btn_adddevice);

	                	if(rect.contains(v.getLeft() + (int) event.getX(), v.getTop() + (int) event.getY())){

	                		mBTConnectState 		 = true;
	                		isHomeAP_check 			 = true;
	                		isHomeAP_exist 			 = false;
	                		BTConfig.wifi_list_ready = false;
	                		isDualBandWifiReady		 = false;

	                		WifiManager wifiManager = (WifiManager) MainActivity.this.getSystemService(Context.WIFI_SERVICE);
	                		WifiInfo wifiInfo = wifiManager.getConnectionInfo();
	                		String current_bssid = wifiInfo.getBSSID();



	                		if(current_bssid != null && !current_bssid.equals("00:00:00:00:00:00")){
	                			BTConfig.check_HomeAP 	= 1;
	                			BTConfig.Check_HomeAP_BSSID = MacToByteArray(current_bssid);
	                		}
	                		else{
	                			BTConfig.check_HomeAP 	= 0;
	                			BTConfig.Check_HomeAP_BSSID = new byte[6];
	                		}

		    				searchBTDevice_action();
	                    }

	                    return true;
	            }
	            return false;
	        }
		});

		mImgBtn_QRcode.setOnTouchListener(new Button.OnTouchListener() {

			@Override
			public boolean onTouch(View v, MotionEvent event) {

				switch(event.getAction()) {
                	case MotionEvent.ACTION_DOWN:
                		mImgBtn_QRcode.setAlpha(0.5f);

                		// Construct a rect of the view's bounds
                		rect = new Rect(v.getLeft(), v.getTop(), v.getRight(), v.getBottom());

                		return true;
                	case MotionEvent.ACTION_UP:
                		mImgBtn_QRcode.setAlpha(1f);
                		if(rect.contains(v.getLeft() + (int) event.getX(), v.getTop() + (int) event.getY())){
                			Log.e(TAG,"start QRcode pairing");
                			searchAPDevice_action();
                		}

                		return true;

				}
				return false;
			}

		});



		mImgBtn_info_setting.setOnClickListener(new OnClickListener(){
			@Override
			public void onClick(View v) {
				Log.d(TAG,"Info Setting");
				info_setting_action();
			}
		});

		mImgBtn_return.setOnClickListener(new OnClickListener(){
			@Override
			public void onClick(View v) {
				Log.d(TAG,"Return page");

				if(!return_bnt_state){
					setContentView(R.layout.activity_main);
					initComponent();
					initComponentAction();
					searchAPDevice_action();
					return_bnt_state = true;

				}
				else{

					setContentView(R.layout.activity_main);
					initComponent();
					initComponentAction();

					mWlanScanListView_2G.setVisibility(View.INVISIBLE);
					mWlanScanListView_5G.setVisibility(View.INVISIBLE);

					mBTConfig.setBTConfigState(-1);
	            	resetTargetData();
	            	show_home_button();

	            	if(BTConfig.flag){
	            		mBTConfig.disconnectBTSocket();
					}
					else{
						disconnectBT();
					}
				}

			}
		});

		handler_pd = new Handler() {
			@Override
			public void handleMessage(Message msg) {
				// Log.d(TAG,"handleMessage msg.what: " + String.valueOf(msg.what));
				switch (msg.what) {
				case 0:
					if(pd!=null){
						if(pd.isShowing())
							pd.dismiss();
					}

					break;
				default:
					break;
				}
			}
		};
	}
	public BTConfig getmBTConfig(){return mBTConfig;};
	public static Context getAppContext() {
		return MainActivity.getAppContext();
	}

	public void checkVersion_action()
	{
		String appVersion = "";

		PackageManager manager = this.getPackageManager();
		PackageInfo info = null;
		try {
			info = manager.getPackageInfo(this.getPackageName(), 0);
		} catch (NameNotFoundException e) {
			e.printStackTrace();
		}
		appVersion = info.versionName;

		if (appVersion.length() == 0)
			appVersion = "2.0.0";

		AlertDialog.Builder builder = new AlertDialog.Builder(mMainAct);
		builder.setTitle("About WiFiConfig");
		builder.setMessage("Version : "+appVersion);
		builder.setPositiveButton("OK", new DialogInterface.OnClickListener(){

			@Override
			public void onClick(DialogInterface dialog, int which) {
				dialog.dismiss();
			}

		});
		builder.create().show();
	}

	/** check Bluetooth enable status */
    public void checkBTAdapterStatus(){

    	int btAdapterStatus = -1;

		if(BTConfig.flag){
			btAdapterStatus = mBTConfig.getBTBle().getBTAdapterStatus();
		}else{
			btAdapterStatus = mBTConfig.getBTRfComm().getBTAdapterStatus();
		}

    	if(btAdapterStatus == 1){
    		//Cmd4BTConfigThread.sendCommonCmd(BTConfigThread.CMD_START_SCAN_BT, mBTConfigHandler);
    	}else if(btAdapterStatus == 0){
    		//mDialogTurnOnBT.show();

    	}else if(btAdapterStatus == -1){
    		ToastOps.getToast("Sorry, your device does not support Bluetooth").show();
    		return;
    	}
    }

	private void searchBTDevice_action()
	{
		final BluetoothAdapter mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
		if (mBluetoothAdapter.isEnabled()) {

			mBTConnectedFlag = false;
			mAPConnectedFlag = false;
			resetTargetData();

			if(!BTConfig.flag){
				disconnectBT();
			}

			scan_BT_peripheral();

		}else{
			AlertDialog.Builder builder = new AlertDialog.Builder(mMainAct);
			builder.setTitle("Warning");
			builder.setMessage("Bluetooth Disabled!");
			builder.setPositiveButton("Turn on Bluetooth", new DialogInterface.OnClickListener(){

				@Override
				public void onClick(DialogInterface dialog, int which) {
					mBluetoothAdapter.enable();
					dialog.dismiss();

					mBTConnectedFlag = false;
					mAPConnectedFlag = false;
					resetTargetData();

					if(!BTConfig.flag){
						disconnectBT();
					}
				}

			});
			builder.setNegativeButton("Cancel",new DialogInterface.OnClickListener() {
				public void onClick(DialogInterface dialog,int id) {
					dialog.cancel();
				}
			});
			builder.create().show();
		}

	}

	private void searchAPDevice_action()
	{

		resetTargetData();

		int i = 0, j = 0;
		int itmeNum = 0;
		g_SingleChoiceID = -1;

		WifiManager wifi = (WifiManager) getSystemService(Context.WIFI_SERVICE);

		// check wifi is disable
		if (!wifi.isWifiEnabled()) {
			// OpenWifiPopUp();
			mNetwork.WifiOpen();

			do {
				SystemClock.sleep(1000);
				wifi = (WifiManager) getSystemService(Context.WIFI_SERVICE);
				// Log.d(TAG, "Turn on Wi-Fi wait");
			} while (!wifi.isWifiEnabled()); // check again if it is disable

			mNetwork.WifiStartScan();

			SystemClock.sleep(2500);
		}


		MainActivity.mMainAct.hide_home_button();
		MainActivity.show_wifi_list = true;

		GetAllWifiList();

		mBTConfig.setBTConfigState(BTConfigState.STATE_AP_SCAN_WLAN_END);

	}


	void GetAllWifiList()
	{

		mNetwork.WifiStartScan();

		ConnectivityManager connManager = (ConnectivityManager) getSystemService(Context.CONNECTIVITY_SERVICE);
		NetworkInfo mWifi = connManager.getNetworkInfo(ConnectivityManager.TYPE_WIFI);

		String tmp = "";
		String connected_ssid = "";
		String connected_bssid = "";
		String connected_ip = "";
		WifiConnected = false;

		WifiManager wifiManager = (WifiManager) MainActivity.this.getSystemService(Context.WIFI_SERVICE);
		WifiInfo wifiInfo = wifiManager.getConnectionInfo();

		connected_ssid = wifiInfo.getSSID();

		if (WifiInfo.getDetailedStateOf(wifiInfo.getSupplicantState()) == NetworkInfo.DetailedState.CONNECTED
				|| mWifi.isConnected()
				|| wifiInfo.getSupplicantState() == SupplicantState.COMPLETED) {

			connected_ssid = wifiInfo.getSSID();
			connected_bssid = wifiInfo.getBSSID();
			if (connected_ssid.indexOf("\"") == 0)
				tmp = connected_ssid.substring(1, connected_ssid.length() - 1);
			else
				tmp = connected_ssid;
			int myIp = wifiInfo.getIpAddress();
			int intMyIp3 = myIp / 0x1000000;
			int intMyIp3mod = myIp % 0x1000000;

			int intMyIp2 = intMyIp3mod / 0x10000;
			int intMyIp2mod = intMyIp3mod % 0x10000;

			int intMyIp1 = intMyIp2mod / 0x100;
			int intMyIp0 = intMyIp2mod % 0x100;

			connected_ip = String.valueOf(intMyIp0) + "."
					+ String.valueOf(intMyIp1) + "." + String.valueOf(intMyIp2)
					+ "." + String.valueOf(intMyIp3);

			WifiConnected = true;
		}

		mScanResults = mNetwork.WifiGetScanResults();

		if (mScanResults != null) {
			mWlanAPList_2G.clear();

			int i = 0;

			for (int iScan = 0; iScan < mScanResults.size() && i < APNumber; iScan++) {

				g_ScanResult = mScanResults.get(iScan);

				byte mEncrpytType;
				if(g_ScanResult.capabilities.contains("WEP")){
					mEncrpytType = 2;
				}else if(g_ScanResult.capabilities.contains("PSK") || g_ScanResult.capabilities.contains("EAP")){
					mEncrpytType = 1;
				}
				else{
					mEncrpytType = 0;
				}

				mWlanAPList_2G.add(new ScanObj(g_ScanResult.SSID, g_ScanResult.BSSID, g_ScanResult.level, mEncrpytType, BTConfig.mTTL_count, (byte) 0));
			}

			updateWlan2GScanUI_AP(mWlanAPList_2G);
		}

	}



	public void resetTargetData() {

		mWlanScanObjs_2G.clear();
		mWlanArrayList_2G = mParseScanObj.getArrayList(mWlanScanObjs_2G,ListBaseAdapter.LIST_SCAN_WLAN_AP);
		mWlanAdapter_2G = new ListBaseAdapter(mMainAct, mWlanArrayList_2G, ListBaseAdapter.LIST_SCAN_WLAN_AP);
		mWlanScanListView_2G.setAdapter(mWlanAdapter_2G);
		mWlanScanListView_2G.setVisibility(View.VISIBLE);
		mListViewHeigth.setListViewHeightBasedOnChildren(mWlanScanListView_2G);
		mWlanScanObjs_5G.clear();
		mWlanArrayList_5G = mParseScanObj.getArrayList(mWlanScanObjs_5G,ListBaseAdapter.LIST_SCAN_WLAN_AP);
		mWlanAdapter_5G = new ListBaseAdapter(mMainAct, mWlanArrayList_5G, ListBaseAdapter.LIST_SCAN_WLAN_AP);
		mWlanScanListView_5G.setAdapter(mWlanAdapter_5G);
		mWlanScanListView_5G.setVisibility(View.VISIBLE);
		mListViewHeigth.setListViewHeightBasedOnChildren(mWlanScanListView_5G);

		mBTScanObjs.clear();

		AP_Profile_queue.clear();
		if(mSharePref.getString("AP1", "") != ""){
			AP_Profile_queue.add(mSharePref.getString("AP1", ""));
		}
		if(mSharePref.getString("AP2", "") != ""){
			AP_Profile_queue.add(mSharePref.getString("AP2", ""));
		}
		if(mSharePref.getString("AP3", "") != ""){
			AP_Profile_queue.add(mSharePref.getString("AP3", ""));
		}

		restParams();
	}

	/** reset parameters to original **/
   	public void restParams(){

   		if(mCheckBTConfigStateTimer != null){
			mCheckBTConfigStateTimer.mRepeaterRedetect = false;
   		}

		mActiveCloseSocket = true;

		mChoiceForAdjust 	= POSITION_DEFAULT;

		mIsDoingAdjustment 	= false;
		mIsDoingConnectAP  	= false;
		mIsDoingScanWlan   	= false;


		mNeedShowAPConnectedDialog		= true;

		mWaitForConnectACK  = false;
		mWaitForBandResult  = false;

		targetAP_password = "";
		targetAP_password_prefs = "";
   	}

   	private void info_setting_action()
	{

   		mImgBtn_info_setting.setVisibility(View.GONE);
   		mImgBtn_return.setVisibility(View.VISIBLE);

   		parentView = (LinearLayout) findViewById(R.id.mainlayout);
    	View show_settingview = LayoutInflater.from(this).inflate(R.layout.setting_page, parentView, false);
    	parentView.addView(show_settingview);

    	mClearBtn 			= (Button) findViewById(R.id.clear_connection_data);
        mRssiSetting 		= (Button) findViewById(R.id.rssi_setting);

        mClearBtn.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {

            	AlertDialog.Builder builder = new AlertDialog.Builder(mMainAct);
        		builder.setTitle("Warning");
        		builder.setMessage("Are you sure to clear all cached information?");

        		builder.setPositiveButton("OK", new DialogInterface.OnClickListener(){
        			@Override
        			public void onClick(DialogInterface dialog, int which) {

        				if(mSharePref!=null){
                    		mSharePref.edit().clear().commit();
                    	}
        				dialog.dismiss();
        			}
        		});

        		builder.setNegativeButton("Cancel", new DialogInterface.OnClickListener(){
        			@Override
        			public void onClick(DialogInterface dialog, int which) {
        				dialog.dismiss();
        			}
        		});

        		builder.create().show();
            }
        });

        mRssiSetting.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {

            	AlertDialog.Builder builder = new AlertDialog.Builder(mMainAct);
        		builder.setTitle("RSSI Configuration");

        		LinearLayout layout = new LinearLayout(context);
        		layout.setOrientation(LinearLayout.VERTICAL);

        		final int highRssi = mSharePref.getInt(GlobalConfig.KEY_HIGH_RSSI, GlobalConfig.DEFAULT_HIGH_RSSI);
           		final int lowRssi  = mSharePref.getInt(GlobalConfig.KEY_LOW_RSSI, GlobalConfig.DEFAULT_LOW_RSSI);

        		int rssi_dbm_high = highRssi - 100;
        		int rssi_dbm_low  = lowRssi - 100;


        		final EditText mHighRssiEditText = new EditText(context);
        		final EditText mLowRssiEditText = new EditText(context);
        		layout.addView(mHighRssiEditText);
        		layout.addView(mLowRssiEditText);

        		mHighRssiEditText.setHint( "default RSSI(high)  " + String.valueOf(rssi_dbm_high) + " dBm");
                mLowRssiEditText.setHint(  "default RSSI(low)   " + String.valueOf(rssi_dbm_low) + " dBm" );

        		builder.setView(layout);

        		builder.setPositiveButton("OK", new DialogInterface.OnClickListener(){
        			@Override
        			public void onClick(DialogInterface dialog, int which) {

        				if(!mHighRssiEditText.getText().toString().matches("") && !mLowRssiEditText.getText().toString().matches("")){

        					int h = Integer.valueOf(mHighRssiEditText.getText().toString());
                        	int l = Integer.valueOf(mLowRssiEditText.getText().toString());

                        	if(h>l && h<0 && l<0 && h>=-100 && l>=-100){

                        		int h_rssi_input = h + 100;
                        		int l_rssi_input = l  + 100;

                            	SharedPreferences.Editor editor = mSharePref.edit();
                       		 	editor.putInt(GlobalConfig.KEY_HIGH_RSSI, h_rssi_input);
                       		 	editor.putInt(GlobalConfig.KEY_LOW_RSSI, l_rssi_input);
                       		 	editor.commit();

                        	}else{
                        		Toast.makeText(	MainActivity.this,"invaild RSSI value",Toast.LENGTH_LONG).show();
                        	}
        				}

                    	dialog.dismiss();
        			}
        		});


        		builder.setNegativeButton("Cancel", new DialogInterface.OnClickListener(){
        			@Override
        			public void onClick(DialogInterface dialog, int which) {
        				dialog.dismiss();
        			}
        		});

        		builder.create().show();
            }
        });

	}

	public void scan_BT_peripheral()
	{
		if(BTConfig.flag){
			if (!getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE)) {
				Toast.makeText(this, "BLE Not Supported", Toast.LENGTH_SHORT).show();
				finish();
			}
		}

    	startWaiting("Scan BT Device","Waiting...",30,"Scan Timeout");

		updateBTScanUI(null);
		Cmd4BTConfigThread.sendCommonCmd(BTConfigThread.CMD_START_SCAN_BT, mBTConfigHandler);

	}

	private void disconnectBT()
	{
		mBTConfig.closeBTSocket();
		mActiveCloseSocket = true;
	}

	public void savePreBT(){
		SharedPreferences.Editor editor = mSharePref.edit();
		editor.putString(GlobalConfig.KEY_PREVIOUSE_BT_NAME, mSavedBTName);
	   	editor.putString(GlobalConfig.KEY_PREVIOUSE_BT_MAC, mSavedBTMAC);
	   	editor.commit();
	}

	public void updateBTScanUI(List<ScanObj> btScanList){
		mBTScanObjs.clear();

		if(btScanList != null){
			for(ScanObj mScanObj: btScanList){
				//filter Bluetooth by name
				if(GlobalConfig.CONFIG_FILTER_BT){
					String filterName = mSharePref.getString(GlobalConfig.KEY_FILTER_BT_NAME,GlobalConfig.DEFAULT_FILTER_BT_NAME);
					if(mScanObj.getSSID().contains(filterName)){
						mBTScanObjs.add(mScanObj);
					}

				}else{
					mBTScanObjs.add(mScanObj);
				}

			}

			sortBTByRSSI();

			if(mBTConnectState){
				mBTConnectState = false;
				mSavedBTName = mBTScanObjs.get(0).getSSID();
	        	mSavedBTMAC = mBTScanObjs.get(0).getMac();
	        	Cmd4BTConfigThread.sendConnectBTCmd(mSavedBTMAC, mBTConfigHandler);
	        	startWaiting("Connect to "+mSavedBTName,"waiting...",30,"Connection Timeout");
			}
		}

	}

	public void hide_home_button(){

		mImgBtn_info_setting.setVisibility(View.GONE);
		mImgBtn_searchDevice.setVisibility(View.GONE);
		mImgBtn_QRcode.setVisibility(View.GONE);

		mImgBtn_return.setVisibility(View.VISIBLE);
	}

	public void show_home_button(){
		mImgBtn_info_setting.setVisibility(View.VISIBLE);
		mImgBtn_searchDevice.setVisibility(View.VISIBLE);
		mImgBtn_QRcode.setVisibility(View.VISIBLE);

		mImgBtn_return.setVisibility(View.GONE);
	}

	public void hide_WiFiList(){
		mWlanScanListView_2G.setVisibility(View.INVISIBLE);
		mWlanScanListView_5G.setVisibility(View.INVISIBLE);
	}

	public void show_WiFiList(){
		mWlanScanListView_2G.setVisibility(View.VISIBLE);
		mWlanScanListView_5G.setVisibility(View.VISIBLE);
	}


	/** sort Bluetooth by RSSI **/
	private void sortBTByRSSI( ){
		if(mBTScanObjs.isEmpty())
			return;

		Collections.sort(mBTScanObjs, new ListSortByRssi()); // sort by rssi
	}

	private void detectPreBT(){
		if(mBTScanObjs.isEmpty())
			return;

		//saved previous Bluetooth profile
		if(GlobalConfig.CONFIG_SAVE_BT_PROFILE){

			String preBTName = mSharePref.getString(GlobalConfig.KEY_PREVIOUSE_BT_NAME, GlobalConfig.DEFAULT_PREVIOUSE_BT_NAME);
			String preBTMac = mSharePref.getString(GlobalConfig.KEY_PREVIOUSE_BT_MAC, GlobalConfig.DEFAULT_PREVIOUSE_BT_MAC);

		}
	}

	public void BTconnectOK()
	{
		mBTConnectedFlag = true;
	}

	public void showBTdisconnection()
	{
		if(mBTConnectedFlag){
			mBTConnectedFlag = false;
			AlertDialog.Builder builder = new AlertDialog.Builder(mMainAct);
			builder.setTitle("Warning");
			builder.setMessage("Please check if BT device is exist.");
			builder.setPositiveButton("OK", new DialogInterface.OnClickListener(){

				@Override
				public void onClick(DialogInterface dialog, int which) {
					dialog.dismiss();
				}

			});
			builder.create().show();

			mBTConnectedFlag = false;
			mAPConnectedFlag = false;
			resetTargetData();
			disconnectBT();
		}

	}

	public void showHomeAP_Dialog()
	{

		WifiManager wifiManager = (WifiManager) MainActivity.this.getSystemService(Context.WIFI_SERVICE);
		WifiInfo wifiInfo = wifiManager.getConnectionInfo();
		final String connected_bssid = wifiInfo.getBSSID();
		String connected_ssid = wifiInfo.getSSID();

		if (connected_ssid.indexOf("\"") == 0){
			connected_ssid = connected_ssid.substring(1,connected_ssid.length() - 1);
		}

		final String connected_ssid_final = connected_ssid;

		hide_WiFiList();

		AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(context);
		final EditText edittext = new EditText(context);
		//check security type
		if(BTConfig.mHomeAP_encrypt != 0){ // security
			String savedPwd = "";
			if(GlobalConfig.CONFIG_SAVE_AP_PWD){
				savedPwd = mSharePref.getString(connected_bssid, "");
			}
			edittext.setInputType(InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_PASSWORD);
			edittext.setText(savedPwd);
			alertDialogBuilder.setTitle("Enter the password for AP \"" + connected_ssid_final + "\"");
			alertDialogBuilder.setView(edittext);
		}else{	//open
			alertDialogBuilder.setTitle("The target AP \"" + connected_ssid_final + "\" has no security. Suggest choosing an secured AP.");
		}

		alertDialogBuilder.setCancelable(false);
		alertDialogBuilder.setPositiveButton("Continue",new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog,int id) {

				//hide keyboard
				if (edittext != null) {
				    InputMethodManager imm = (InputMethodManager)getSystemService(Context.INPUT_METHOD_SERVICE);
				    imm.hideSoftInputFromWindow(edittext.getWindowToken(), 0);
				}

				isHomeAP_exist = false;

				boolean isAPProfileRight = false;
           	 	String passwordStr = null;
           	 	byte sendEncrpyt = 1;

				if (BTConfig.mHomeAP_encrypt == 0) {	/* AP is open */
					sendEncrpyt = 0;
					isAPProfileRight = true;
				}else{					/* AP is not open */
					if(BTConfig.mHomeAP_encrypt == 1){
						sendEncrpyt = 1;
					}else if(BTConfig.mHomeAP_encrypt == 2){
						sendEncrpyt = 2;
					}
					targetAP_password = edittext.getText().toString();
					passwordStr = targetAP_password;
					// Check the legality of the input password
					int checkPwdFlag = APPasswordCheck.checkWifiPassWord(passwordStr.getBytes(), (byte)BTConfig.mHomeAP_encrypt);

					if(checkPwdFlag == 1){/* password in right format */
               			isAPProfileRight = true;
               	 	}else{/* password in wrong format */
               	 		isAPProfileRight = false;

               	 	AlertDialog.Builder builder = new AlertDialog.Builder(mMainAct);
	        		builder.setTitle("Warning");
	        		builder.setMessage("The password is invalid!");

	        		builder.setPositiveButton("OK", new DialogInterface.OnClickListener(){

	        			@Override
	        			public void onClick(DialogInterface dialog, int which) {
	        				dialog.dismiss();
	        				showHomeAP_Dialog();
	        			}

	        		});
	        		builder.create().show();

               	 	}// wrong password format
				}

				if(isAPProfileRight){

	           		 mIsDoingConnectAP = false;
	           		 mAPConnectedFlag = false;
	           		 mChoiceForAdjust = POSITION_DEFAULT;

	           		 mUIState = UIState.STATE_UI_SHOW_CONNECTING_AP;

	           		 isCheckPostiion = true;
	           		 setAPDisconnectOnUI(connected_ssid_final, sendEncrpyt);
	           		 startWaiting_ConnectAP("Connecting to " + connected_ssid_final, "waiting...", 30, "Connection to AP Timeout");

	           		 mRemoteAPSSID = connected_ssid_final;
	           		 mRemoteAPMAC = connected_bssid;
	           		 targetAP_password_prefs = passwordStr;

	           		 mNeedShowAPConnectedDialog = true;
	           		 mWaitForConnectACK 		= true;
	           		 show_fail_msg 				= false;

	           		 Log.w(TAG, "send profile: "+ connected_ssid_final.toString()+"/"+passwordStr);

	           		 mRemoteAPBuf = constructAPProfile((byte)BTConfig.mHomeAP_band, sendEncrpyt, connected_ssid_final, connected_bssid, passwordStr);
	           		 Cmd4BTConfigThread.sendAPProfileCmd(mRemoteAPBuf, mBTConfigHandler);
	           	}

			}
		});


		alertDialogBuilder.setNegativeButton("Select AP",new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog,int id) {
				dialog.cancel();

				if(!BTConfig.wifi_list_ready){
					startWaiting("Scan WiFi AP", "Waiting...", 30,"Scan Timeout");
					show_wifi_list = true;
	            	isHomeAP_exist = false;
	            	if(mCheckBTConfigStateTimer.wlan2GEnabled == 0 && mCheckBTConfigStateTimer.wlan5GEnabled == 1){
	            		mBTConfig.setBTConfigState(BTConfigState.STATE_BT_RECEIVE_WLAN_5G);
		            	mWlanScanListView_5G.setVisibility(View.VISIBLE);
	            	}
	            	else{
	            		mBTConfig.setBTConfigState(BTConfigState.STATE_BT_RECEIVE_WLAN_2G);
		            	mWlanScanListView_2G.setVisibility(View.VISIBLE);
	            	}

				}
            	else{
            		if(mCheckBTConfigStateTimer.wlan2GEnabled == 0 && mCheckBTConfigStateTimer.wlan5GEnabled == 1){
            			mBTConfig.setBTConfigState(BTConfigState.STATE_BT_SCAN_WLAN_5G_END);
	            		mWlanScanListView_5G.setVisibility(View.VISIBLE);
	            	}
	            	else{
	            		mBTConfig.setBTConfigState(BTConfigState.STATE_BT_SCAN_WLAN_2G_END);
	            		mWlanScanListView_2G.setVisibility(View.VISIBLE);
	            	}
            		show_wifi_list = true;
                	isHomeAP_exist = false;

            	}
			}
		});

		alertDialog_pwd = alertDialogBuilder.create();
		alertDialog_pwd.show();

	}

	/** query wlan band capability * **/
	 public void queryWlanBand(){
		 setUIToWlanScanUI(); // refresh the scan results on UI
		 mWaitForBandResult = true;

		 Cmd4BTConfigThread.sendCommonCmd(BTConfigThread.CMD_QUERY_WLAN_BAND, mBTConfigHandler);
	 }

	 /** init wlan scan UI  * ***/
    private void setUIToWlanScanUI()
    {

		mWlanScanListView_2G.setOnItemClickListener(new AdapterView.OnItemClickListener(){

			@Override
			public void onItemClick(AdapterView<?> parent, View view, int position, long id) {

				if(mCheckBTConfigStateTimer.mIsNoResponse)
				{
					mToastBTNoResponse.show();
					return;
				}else if(mBTConfig.getBTConfigState() == BTConfigState.STATE_BT_REPEATER_OFFLINE
						|| mCheckBTConfigStateTimer.mRepeaterRedetect){

					if(GlobalConfig.CONFIG_OFFLINE_DECTECTION)
						mToastBTWaitOnLine.show();
					else
						mToastBTDisconnected.show();

					return;
				}
				mUIState = UIState.STATE_UI_SHOW_CHOICE_FOR_AP;
				mIsDoingConnectAP = true;

				showAPConnectDialog((byte)0, position, 0);
			}

		});

    	mWlanScanListView_5G.setOnItemClickListener(new AdapterView.OnItemClickListener(){

			@Override
			public void onItemClick(AdapterView<?> parent, View view, int position, long id) {

				if(mCheckBTConfigStateTimer.mIsNoResponse)
				{
					mToastBTNoResponse.show();
					return;
				}else if(mBTConfig.getBTConfigState() == BTConfigState.STATE_BT_REPEATER_OFFLINE
						|| mCheckBTConfigStateTimer.mRepeaterRedetect){

					if(GlobalConfig.CONFIG_OFFLINE_DECTECTION)
						mToastBTWaitOnLine.show();
					else
						mToastBTDisconnected.show();

					return;
				}
				mUIState = UIState.STATE_UI_SHOW_CHOICE_FOR_AP;
				mIsDoingConnectAP = true;

				showAPConnectDialog((byte)1, position, 0);
			}

    	});
    }


    void APConfig_info(){

    	if(show_fail_msg){
    		return;
    	}

    	final String target_SSID 		= (String)mExtendAPScanObjs.get(0).getSSID();
    	final String target_MAC 		= (String)mExtendAPScanObjs.get(0).getMac();

    	View view = View.inflate(this, R.layout.ap_info, null);

    	custom_dialog_2 = new AlertDialog.Builder(context);
    	custom_dialog_2.setTitle("Configure Success");
    	custom_dialog_2.setCancelable(false);
    	custom_dialog_2.setView(view);

    	ap_info_name 				= (TextView)view.findViewById(R.id.test_name);
    	ap_info_rssi 				= (ImageView)view.findViewById(R.id.test_rssi);
    	mConfirm_Config_Button 		= (Button)view.findViewById(R.id.confirm_config_button);
    	mtry_newap_button 			= (Button)view.findViewById(R.id.try_newap_button);

    	ap_info_name.setText(target_SSID);
    	ap_info_rssi.setImageResource(0);



    	alertDialog_custom_dialog = custom_dialog_2.create();
    	alertDialog_custom_dialog.show();

    	mConfirm_Config_Button.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {

            	// CONFIG_SAVE_AP_PROFILE
            	if(GlobalConfig.CONFIG_SAVE_AP_PROFILE && (int)mExtendAPScanObjs.get(0).getEncrpytType() != 0){

            		AP_Profile_queue.add(target_MAC);
            		if(AP_Profile_queue.size()>3){
            			AP_Profile_queue.remove();
            		}

    				SharedPreferences.Editor editor = mSharePref.edit();

    				if(AP_Profile_queue.peek()!=null){
    					editor.putString("AP1", AP_Profile_queue.poll());
    				}
    				if(AP_Profile_queue.peek()!=null){
    					editor.putString("AP2", AP_Profile_queue.poll());
    				}
    				if(AP_Profile_queue.peek()!=null){
    					editor.putString("AP3", AP_Profile_queue.poll());
    				}

          			editor.commit();
    			}

            	// CONFIG_SAVE_AP_PWD
            	if(GlobalConfig.CONFIG_SAVE_AP_PWD && (int)mExtendAPScanObjs.get(0).getEncrpytType() != 0){

          			 SharedPreferences.Editor editor = mSharePref.edit();
          			 editor.putString(target_MAC, targetAP_password_prefs);
          			 editor.commit();
          		}

            	alertDialog_custom_dialog.dismiss();

            	mBTConfig.setBTConfigState(-1);
            	resetTargetData();
            	show_home_button();

            	if(BTConfig.flag){
            		mBTConfig.disconnectBTSocket();
				}
				else{
					disconnectBT();
				}
            }
        });

    	mtry_newap_button.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
            	alertDialog_custom_dialog.dismiss();

            	if(mCheckBTConfigStateTimer.wlan2GEnabled == 0 && mCheckBTConfigStateTimer.wlan5GEnabled == 1){
            		mBTConfig.setBTConfigState(BTConfigState.STATE_BT_SCAN_WLAN_5G_END);
                	mWlanScanListView_5G.setVisibility(View.VISIBLE);
            	}
            	else{
            		mBTConfig.setBTConfigState(BTConfigState.STATE_BT_SCAN_WLAN_2G_END);
                	mWlanScanListView_2G.setVisibility(View.VISIBLE);
            	}

            	show_wifi_list = true;
            	isHomeAP_exist = false;


            	if(!BTConfig.wifi_list_ready){
            		mBTConfig.setBTConfigState(BTConfigState.STATE_BT_QUERY_WLAN_BAND_END);
					startWaiting("Scan WiFi AP", "Waiting...", 30,"Scan Timeout");
				}

            }
        });
    }

    private void sortAPByRSSI(){
		if(mWlanScanObjs_2G.isEmpty())
			return;
		Collections.sort(mWlanScanObjs_2G, new ListSortByRssi()); // sort by rssi
	}

    private void sortAPByRSSI_5G(){
		if(mWlanScanObjs_5G.isEmpty())
			return;
		Collections.sort(mWlanScanObjs_5G, new ListSortByRssi()); // sort by rssi
	}

    protected void updateWlan2GScanUI_AP(List<ScanObj> tmpScanResults){

		ScanObj.CompareFlag = true;

		for(int i = 0; i < tmpScanResults.size(); i++){

			if(!tmpScanResults.get(i).getMac().equals("00:00:00:00:00:00")){
				if(!mWlanScanObjs_2G.contains(tmpScanResults.get(i))){
					mWlanScanObjs_2G.add(tmpScanResults.get(i));
				}
				else{
					// update TTL and RSSI for existed AP
					mWlanScanObjs_2G.get(mWlanScanObjs_2G.indexOf(tmpScanResults.get(i))).resetTTL(BTConfig.mTTL_count);
					mWlanScanObjs_2G.get(mWlanScanObjs_2G.indexOf(tmpScanResults.get(i))).setRssi(tmpScanResults.get(i).getRssi());
				}
			}
		}


		for(int i = 0; i < mWlanScanObjs_2G.size(); i++){
			mWlanScanObjs_2G.get(i).decreaseTTL();
			if(mWlanScanObjs_2G.get(i).getTTL() == 0){
				mWlanScanObjs_2G.remove(i);
			}
		}

		sortAPByRSSI();

		ScanObj.CompareFlag = false;

		//check duplicated SSID AP (remove low RSSI repeated SSID)
		for(int i = 0; i < mWlanScanObjs_2G.size(); i++){
			if(mWlanScanObjs_2G.get(i).getSSID().length() != 0){
				if(mWlanScanObjs_2G.indexOf(mWlanScanObjs_2G.get(i)) != mWlanScanObjs_2G.lastIndexOf(mWlanScanObjs_2G.get(i))){
					int lastIndex = mWlanScanObjs_2G.lastIndexOf(mWlanScanObjs_2G.get(i));
					mWlanScanObjs_2G.remove(mWlanScanObjs_2G.lastIndexOf(mWlanScanObjs_2G.get(i)));
					if(lastIndex == i){
						i--;
					}
				}
			}

		}

		mWlanArrayList_2G = mParseScanObj.getArrayList(mWlanScanObjs_2G,ListBaseAdapter.LIST_SCAN_WLAN_AP);
		mWlanAdapter_2G = new ListBaseAdapter(mMainAct, mWlanArrayList_2G, ListBaseAdapter.LIST_SCAN_WLAN_AP);
		mWlanScanListView_2G.setAdapter(mWlanAdapter_2G);
		mWlanScanListView_2G.setVisibility(View.VISIBLE);
		mListViewHeigth.setListViewHeightBasedOnChildren(mWlanScanListView_2G);

    }

	protected void updateWlan2GScanUI(){

		List<ScanObj> tmpScanResults = mBTConfig.getWlanScanResults(0);

		ScanObj.CompareFlag = true;

		for(int i = 0; i < tmpScanResults.size(); i++){

			if(!tmpScanResults.get(i).getMac().equals("00:00:00:00:00:00")){

				if(!mWlanScanObjs_2G.contains(tmpScanResults.get(i))){
					mWlanScanObjs_2G.add(tmpScanResults.get(i));
				}
				else{
					// update TTL and RSSI for existed AP
					mWlanScanObjs_2G.get(mWlanScanObjs_2G.indexOf(tmpScanResults.get(i))).resetTTL(BTConfig.mTTL_count);
					mWlanScanObjs_2G.get(mWlanScanObjs_2G.indexOf(tmpScanResults.get(i))).setRssi(tmpScanResults.get(i).getRssi());
				}
			}
		}


		for(int i = 0; i < mWlanScanObjs_2G.size(); i++){
			mWlanScanObjs_2G.get(i).decreaseTTL();
			if(mWlanScanObjs_2G.get(i).getTTL() == 0){
				mWlanScanObjs_2G.remove(i);
			}
		}

		sortAPByRSSI();

		ScanObj.CompareFlag = false;

		//check duplicated SSID AP (remove low RSSI repeated SSID)
		for(int i = 0; i < mWlanScanObjs_2G.size(); i++){
			if(mWlanScanObjs_2G.get(i).getSSID().length() != 0){
				if(mWlanScanObjs_2G.indexOf(mWlanScanObjs_2G.get(i)) != mWlanScanObjs_2G.lastIndexOf(mWlanScanObjs_2G.get(i))){
					int lastIndex = mWlanScanObjs_2G.lastIndexOf(mWlanScanObjs_2G.get(i));
					mWlanScanObjs_2G.remove(mWlanScanObjs_2G.lastIndexOf(mWlanScanObjs_2G.get(i)));
					if(lastIndex == i){
						i--;
					}
				}
			}

			mWlanScanObjs_2G.get(i).setBand((byte) 0);
		}

		mWlanArrayList_2G = mParseScanObj.getArrayList(mWlanScanObjs_2G,ListBaseAdapter.LIST_SCAN_WLAN_AP);
		mWlanAdapter_2G = new ListBaseAdapter(mMainAct, mWlanArrayList_2G, ListBaseAdapter.LIST_SCAN_WLAN_AP);
		mWlanScanListView_2G.setAdapter(mWlanAdapter_2G);
		mWlanScanListView_2G.setVisibility(View.VISIBLE);
		mListViewHeigth.setListViewHeightBasedOnChildren(mWlanScanListView_2G);

		if(show_wifi_list_2){
			Toast.makeText(this, "Keep Searching and updating WiFi AP", Toast.LENGTH_LONG).show();
			show_wifi_list_2 = false;
		}

		if(show_wifi_list){
			Toast.makeText(this, "Please choose target AP to connect", Toast.LENGTH_LONG).show();
			show_wifi_list_2 = true;
			show_wifi_list = false;
		}

    }



    protected void updateWlan5GScanUI(){

    	if (mCheckBTConfigStateTimer.wlan2GEnabled == 1 && mCheckBTConfigStateTimer.wlan5GEnabled == 1){

    		List<ScanObj> tmpScanResults = mBTConfig.getWlanScanResults(1);

        	ScanObj.CompareFlag = true;

        	for(int i = 0; i < tmpScanResults.size(); i++){

    			if(!tmpScanResults.get(i).getMac().equals("00:00:00:00:00:00")){

    				if(!mWlanScanObjs_2G.contains(tmpScanResults.get(i))){
    					mWlanScanObjs_2G.add(tmpScanResults.get(i));
    				}
    				else{
    					// update TTL and RSSI for existed AP
    					mWlanScanObjs_2G.get(mWlanScanObjs_2G.indexOf(tmpScanResults.get(i))).resetTTL(BTConfig.mTTL_count);
    					mWlanScanObjs_2G.get(mWlanScanObjs_2G.indexOf(tmpScanResults.get(i))).setRssi(tmpScanResults.get(i).getRssi());
    				}
    			}
    		}

    		for(int i = 0; i < mWlanScanObjs_2G.size(); i++){
    			mWlanScanObjs_2G.get(i).decreaseTTL();
    			if(mWlanScanObjs_2G.get(i).getTTL() == 0){
    				mWlanScanObjs_2G.remove(i);
    			}
    		}

    		sortAPByRSSI();

    		ScanObj.CompareFlag = false;

    		//check duplicated SSID AP (remove low RSSI repeated SSID)
    		for(int i = 0; i < mWlanScanObjs_2G.size(); i++){
    			if(mWlanScanObjs_2G.get(i).getSSID().length() != 0){
    				if(mWlanScanObjs_2G.indexOf(mWlanScanObjs_2G.get(i)) != mWlanScanObjs_2G.lastIndexOf(mWlanScanObjs_2G.get(i))){
    					int lastIndex = mWlanScanObjs_2G.lastIndexOf(mWlanScanObjs_2G.get(i));
    					mWlanScanObjs_2G.remove(mWlanScanObjs_2G.lastIndexOf(mWlanScanObjs_2G.get(i)));
    					if(lastIndex == i){
    						i--;
    					}
    				}
    			}
    			mWlanScanObjs_2G.get(i).setBand((byte) 1);
    		}

    		mWlanArrayList_2G = mParseScanObj.getArrayList(mWlanScanObjs_2G,ListBaseAdapter.LIST_SCAN_WLAN_AP);
    		mWlanAdapter_2G = new ListBaseAdapter(mMainAct, mWlanArrayList_2G, ListBaseAdapter.LIST_SCAN_WLAN_AP);
    		mWlanScanListView_2G.setAdapter(mWlanAdapter_2G);
    		mWlanScanListView_2G.setVisibility(View.VISIBLE);
    		mListViewHeigth.setListViewHeightBasedOnChildren(mWlanScanListView_2G);
    	}
    	else{

    		List<ScanObj> tmpScanResults = mBTConfig.getWlanScanResults(1);

        	ScanObj.CompareFlag = true;

        	for(int i = 0; i < tmpScanResults.size(); i++){

    			if(!tmpScanResults.get(i).getMac().equals("00:00:00:00:00:00")){

    				if(!mWlanScanObjs_5G.contains(tmpScanResults.get(i))){
    					mWlanScanObjs_5G.add(tmpScanResults.get(i));
    				}
    				else{
    					// update TTL and RSSI for existed AP
    					mWlanScanObjs_5G.get(mWlanScanObjs_5G.indexOf(tmpScanResults.get(i))).resetTTL(BTConfig.mTTL_count);
    					mWlanScanObjs_5G.get(mWlanScanObjs_5G.indexOf(tmpScanResults.get(i))).setRssi(tmpScanResults.get(i).getRssi());
    				}
    			}
    		}

    		for(int i = 0; i < mWlanScanObjs_5G.size(); i++){
    			mWlanScanObjs_5G.get(i).decreaseTTL();
    			if(mWlanScanObjs_5G.get(i).getTTL() == 0){
    				mWlanScanObjs_5G.remove(i);
    			}
    		}

    		sortAPByRSSI_5G();

    		ScanObj.CompareFlag = false;

    		//check duplicated SSID AP (remove low RSSI repeated SSID)
    		for(int i = 0; i < mWlanScanObjs_5G.size(); i++){
    			if(mWlanScanObjs_5G.get(i).getSSID().length() != 0){
    				if(mWlanScanObjs_5G.indexOf(mWlanScanObjs_5G.get(i)) != mWlanScanObjs_5G.lastIndexOf(mWlanScanObjs_5G.get(i))){
    					int lastIndex = mWlanScanObjs_5G.lastIndexOf(mWlanScanObjs_5G.get(i));
    					mWlanScanObjs_5G.remove(mWlanScanObjs_5G.lastIndexOf(mWlanScanObjs_5G.get(i)));
    					if(lastIndex == i){
    						i--;
    					}
    				}
    			}
    			mWlanScanObjs_5G.get(i).setBand((byte) 1);

    		}

    		mWlanArrayList_5G = mParseScanObj.getArrayList(mWlanScanObjs_5G,ListBaseAdapter.LIST_SCAN_WLAN_AP);
    		mWlanAdapter_5G = new ListBaseAdapter(mMainAct, mWlanArrayList_5G, ListBaseAdapter.LIST_SCAN_WLAN_AP);
    		mWlanScanListView_5G.setAdapter(mWlanAdapter_5G);
    		mWlanScanListView_5G.setVisibility(View.VISIBLE);
    		mListViewHeigth.setListViewHeightBasedOnChildren(mWlanScanListView_5G);
    	}

    }

    /***  update extended AP Info UI and judge the repeater connection status * ***/
	public  void updateExtendedAPInfoUI()
	{
		if(mWaitForConnectACK || mIsDoingScanWlan)
			return;

		mExtendAPScanObjs.clear();

		List<ScanObj> tmpExtendedAPs = mBTConfig.getExtendAPObjs();

		for(int i = 0; i < tmpExtendedAPs.size(); i++){
			if(tmpExtendedAPs.get(i) != null){
				if(tmpExtendedAPs.get(i).getConfigureStatus() == 0){ // never configure the repeater before
					return;
				}
				mExtendAPScanObjs.add( tmpExtendedAPs.get(i));
			}
		}

		byte tmpConnectStatus = mExtendAPScanObjs.get(0).getConnectStatus();
		String target_SSID 		= (String)mExtendAPScanObjs.get(0).getSSID();
        String target_MAC 		= (String)mExtendAPScanObjs.get(0).getMac();
        int target_securtiy 	=  (int)mExtendAPScanObjs.get(0).getEncrpytType();
        int target_rssi 		= (int)mExtendAPScanObjs.get(0).getRssi();


        if(ap_info_name != null){
        	ap_info_name.setText(target_SSID);
		}


		judgeConnectStatus(target_SSID);

	}

	/** judge the repeater connection status * ***/
	private void judgeConnectStatus(String ssid){
		byte tmpConnectStatus = mExtendAPScanObjs.get(0).getConnectStatus();

		if(tmpConnectStatus == 0x04){ /* connect successfully */
			connectToAPOK();
		}
		else if(tmpConnectStatus == 0x0f){	/* wrong password */

			if(ap_info_rssi != null){
				ap_info_rssi.setImageResource(R.drawable.strength_disconnection);
			}
			wrongKeyForAP(ssid);
		}
		else{ /* connecting or connect fail */

			if(ap_info_rssi != null){
				ap_info_rssi.setImageResource(R.drawable.strength_disconnection);
			}
			connectToAPFail();
		}
	}

	/***  update extended AP Info UI and judge the repeater connection status * ***/
	public  void setAPDisconnectOnUI(String ssid, byte encrypt){
		if(mWaitForConnectACK)
			return;

		mExtendAPScanObjs.clear();

		ScanObj remoteAP = new ScanObj(ssid, "", (byte)0, encrypt, (byte)0, (byte)1);
		mExtendAPScanObjs.add( remoteAP );

		mExtendAPArrayList = mParseScanObj.getArrayList(mExtendAPScanObjs,ListBaseAdapter.LIST_EXTENDED_AP);
		mExtendAPAdapter = new ListBaseAdapter(mMainAct, mExtendAPArrayList, ListBaseAdapter.LIST_EXTENDED_AP);

	 }

    public void showAPConnectDialog(byte band, int index, final int tag){ //tag0:BT AP LIST  tag1:Phone AP LIST
		List<ScanObj> apScanObjs;
		if(band == 0){
			apScanObjs = mWlanScanObjs_2G;
		}else if(band == 1){
			apScanObjs = mWlanScanObjs_5G;
		}else
			return;

		//final byte sendBand = band;
		final byte sendBand = apScanObjs.get(index).getBand();
		final byte encryptType = apScanObjs.get(index).getEncrpytType();
		final String sendSSID = apScanObjs.get(index).getSSID();
		final String sendMAC = apScanObjs.get(index).getMac();

		AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(context);
		final EditText edittext = new EditText(context);

		//check security type
		if(encryptType == 1 || encryptType == 2){// 1:WPA/WPA2 Key  2:WEP Key
			String savedPwd = "";
			if(GlobalConfig.CONFIG_SAVE_AP_PWD){
				savedPwd = mSharePref.getString(sendMAC, "");
			}
			edittext.setInputType(InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_PASSWORD);
			edittext.setText(savedPwd);
			alertDialogBuilder.setTitle("Enter the password for AP \"" + sendSSID + "\"");

			alertDialogBuilder.setView(edittext);
		}else if(encryptType ==  0){//open
			alertDialogBuilder.setTitle("The target AP \"" + sendSSID + "\" has no security. Suggest choosing an secured AP.");
		}

		alertDialogBuilder.setCancelable(false);
		alertDialogBuilder.setPositiveButton("Continue",new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog,int id) {

				//hide keyboard
				if (edittext != null) {
				    InputMethodManager imm = (InputMethodManager)getSystemService(Context.INPUT_METHOD_SERVICE);
				    imm.hideSoftInputFromWindow(edittext.getWindowToken(), 0);
				}

				boolean isAPProfileRight = false;
           	 	String passwordStr = null;
           	 	byte sendEncrpyt = 1;

				if (encryptType == 0) {	/* AP is open */
					sendEncrpyt = 0;
					isAPProfileRight = true;
				}else{					/* AP is not open */
					if(encryptType == 1){
						sendEncrpyt = 1;
					}else if(encryptType == 2){
						sendEncrpyt = 2;
					}
					targetAP_password = edittext.getText().toString();
					passwordStr = targetAP_password;
					// Check the legality of the input password
					int checkPwdFlag = APPasswordCheck.checkWifiPassWord(passwordStr.getBytes(),encryptType);

					if(checkPwdFlag == 1){/* password in right format */
               			isAPProfileRight = true;
               	 	}else{/* password in wrong format */
               	 		isAPProfileRight = false;

		            	if(checkPwdFlag  ==  0 && encryptType == 1){
		            		Log.e(TAG,"WPA_WRONG_PASSWORD_FORMAT");
		            		//ToastOps.getToast( "Invalid WPA/WPA2 Key! Key Length Must >=8 characters").show();
		            	}else if(checkPwdFlag == 0 && encryptType == 2){
		            		Log.e(TAG,"WEP_WRONG_PASSWORD_FORMAT");
		            		//ToastOps.getToast( "Invalid WEP Key! Key Length Must Be 5,10,13,26 characters").show();
		            	}else{
		            		Log.e(TAG,"Invalid Key");
		            		//ToastOps.getToast( "Invalid Key!").show();
		            	}

		            	AlertDialog.Builder builder = new AlertDialog.Builder(mMainAct);
		        		builder.setTitle("Warning");
		        		builder.setMessage("The password is invalid!");
		        		builder.setPositiveButton("OK", new DialogInterface.OnClickListener(){

		        			@Override
		        			public void onClick(DialogInterface dialog, int which) {
		        				dialog.dismiss();
		        			}

		        		});
		        		builder.create().show();

               	 	}// wrong password format
				}

				if(isAPProfileRight && tag == 0){

	           		 mIsDoingConnectAP = false;
	           		 mAPConnectedFlag = false;
	           		 mChoiceForAdjust = POSITION_DEFAULT;

	           		 mUIState = UIState.STATE_UI_SHOW_CONNECTING_AP;

	           		 isCheckPostiion = true;
	           		 setAPDisconnectOnUI(sendSSID, sendEncrpyt);
	           		 startWaiting_ConnectAP("Connecting to " + sendSSID, "waiting...", 30, "Connection to AP Timeout");

	           		 mRemoteAPSSID = sendSSID;
	           		 mRemoteAPMAC = sendMAC;
	           		 targetAP_password_prefs = passwordStr;

	           		 mNeedShowAPConnectedDialog = true;
	           		 mWaitForConnectACK 		= true;
	           		 show_fail_msg 				= false;

	           		 Log.w(TAG, "send profile: "+ sendSSID.toString()+"/"+passwordStr);

	           		 mRemoteAPBuf = constructAPProfile(sendBand, sendEncrpyt, sendSSID, sendMAC, passwordStr);
	           		 Cmd4BTConfigThread.sendAPProfileCmd(mRemoteAPBuf, mBTConfigHandler);
	           	}

				else if(isAPProfileRight && tag == 1){
					// generate QRCODE for selected target AP
					targetAP_password_prefs = passwordStr;
					Log.w(TAG, "selected profile: "+ sendSSID.toString()+" / "+passwordStr);

					String AP_SSID = sendSSID.toString();
					String QRCodeContent = "";

					if(encryptType == 0){ // OPEN
						QRCodeContent  = "WIFI:S:" + AP_SSID + ";;";
					}
					else if(encryptType == 1){ // WPA/WPA2
						QRCodeContent  = "WIFI:S:" + AP_SSID + ";T:WPA;P:" + passwordStr + ";;";
					}
					else if(encryptType == 2){ // WEP
						QRCodeContent  = "WIFI:S:" + AP_SSID + ";T:WEP;P:" + passwordStr + ";;";
					}

					Hashtable hints= new Hashtable();
					hints.put(EncodeHintType.CHARACTER_SET, "UTF-8");
					hints.put(EncodeHintType.ERROR_CORRECTION, ErrorCorrectionLevel.L);

					int QRCodeWidth = 800;
					int QRCodeHeight = 800;

					QRCodeWriter writer = new QRCodeWriter();

					try {
				        BitMatrix bitMatrix = writer.encode(QRCodeContent, BarcodeFormat.QR_CODE, QRCodeWidth, QRCodeHeight, hints);
				        int width = bitMatrix.getWidth();
				        int height = bitMatrix.getHeight();
				        Bitmap bmp = Bitmap.createBitmap(width, height, Bitmap.Config.RGB_565);
				        for (int x = 0; x < width; x++) {
				            for (int y = 0; y < height; y++) {
				                bmp.setPixel(x, y, bitMatrix.get(x, y) ? Color.BLACK : Color.WHITE);
				            }
				        }

				        mWlanScanListView_2G.setVisibility(View.INVISIBLE);
				        mBTConfig.setBTConfigState(-1);
				        show_qrcode_image(bmp);

				    } catch (WriterException e) {
				    	Log.e("QR ERROR", ""+e);
				        e.printStackTrace();
				    }
				}
			}
		});


		alertDialogBuilder.setNegativeButton("Back",new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog,int id) {
				dialog.cancel();
			}
		});

		alertDialog_pwd = alertDialogBuilder.create();
		alertDialog_pwd.show();

	}

    public void show_qrcode_image(Bitmap bmp){

    	return_bnt_state = false;

    	parentView = (LinearLayout) findViewById(R.id.mainlayout);
    	View show_qrview = LayoutInflater.from(this).inflate(R.layout.qrcode_info, parentView, false);
    	parentView.addView(show_qrview);

    	image_qrcode = (ImageView) show_qrview.findViewById(R.id.imageView_qrcode);
    	image_qrcode.setImageBitmap(bmp);

    }


    public void close_dialog(){

    	if(alertDialog_pwd != null){
    		alertDialog_pwd.dismiss();
    		alertDialog_pwd = null;
		}

    	if(alertDialog_custom_dialog != null){
    		alertDialog_custom_dialog.dismiss();
    		alertDialog_custom_dialog = null;
		}


    }

    /** wifi's client connects to AP successfully, and show position adjustment dialog **/

	private void connectToAPOK()
	{
		stopWaiting();
		mAPConnectedFlag = true;

		mWlanScanListView_2G.setVisibility(View.INVISIBLE);
		mWlanScanListView_5G.setVisibility(View.INVISIBLE);

		if(mWaitForConnectACK_2){
			APConfig_info();
			mWaitForConnectACK_2 = false;
		}

		int currentRssi =  mExtendAPScanObjs.get(0).getRssi();

		if(GlobalConfig.CONFIG_POSITION_DECTECTION){
			/* parse rssi to position index */

			int tmpPositionIndex = -1; //Rssi2Position.getPostionRangeIndex(tmpRSSI);

			int highRssi = MainActivity.mSharePref.getInt(GlobalConfig.KEY_HIGH_RSSI, GlobalConfig.DEFAULT_HIGH_RSSI);
			int lowRssi  = MainActivity.mSharePref.getInt(GlobalConfig.KEY_LOW_RSSI, GlobalConfig.DEFAULT_LOW_RSSI);

			tmpPositionIndex = Rssi2Position.getRssiRangeIndex((byte)(currentRssi+100),highRssi,lowRssi);
			Log.d(TAG, "showPositionAdjustDialog--- "+tmpPositionIndex);

			/* show position adjustment dialog */
			showPositionAdjustDialog(tmpPositionIndex);

			//int current_target_rssi = currentRssi+100;
	        byte strength = mParseScanObj.parseWifiStrength(currentRssi);

	        if(ap_info_rssi != null){
	        	ap_info_rssi.setImageResource(mParseScanObj.getStrengthDrawable(strength));
	        }


		}else{
			if(!mNeedShowAPConnectedDialog)
				return;

			mNeedShowAPConnectedDialog = false;

			String target_SSID 		= (String)mExtendAPScanObjs.get(0).getSSID();
	        String target_MAC 		= (String)mExtendAPScanObjs.get(0).getMac();
	        int target_securtiy 	=  (int)mExtendAPScanObjs.get(0).getEncrpytType();
			Log.e(TAG,"Ap Connected: "+target_SSID+" security:"+target_securtiy+" rssi:"+currentRssi);

		}
	}

	/** show repeater position adjustment dialog
	 * 1. too far/too close, default choose adjust position
	 * 2. fine, default no longer adjust position
	 * **/
	protected void showPositionAdjustDialog(final int positionIndex){
		if(mClickBackToBTUI) // click back button
			return;
		if(positionIndex == -1 ) //0, 1, 2 is right value
			return;
		if(mChoiceForAdjust == POSITION_CANCEL) // cancel adjust
			return;

		mNeedShowWaitOnlineDialog = false;

		mUIState = UIState.STATE_UI_SHOW_CHOICE_FOR_ADJUSTMENT;

		mPositionIndex = positionIndex;

		Log.d(TAG,"positionIndex: "+positionIndex);

		if(isCheckPostiion){
			isCheckPostiion = false;

			if(positionIndex==0){
				Log.d(TAG,"POSITION TOO CLOSE");
				if(mCheckBTConfigStateTimer.product_type == 1){ //ameba device
					mIsDoingAdjustment = false;
				}else{
					mIsDoingAdjustment = true;

					AlertDialog.Builder builder = new AlertDialog.Builder(mMainAct);
					builder.setTitle(mSavedBTName+"'s Position Too Close From AP!");
					builder.setMessage("Suggest to adjust position far away from AP!");
					builder.setPositiveButton("NO,thanks!",new DialogInterface.OnClickListener(){

						@Override
						public void onClick(DialogInterface dialog, int which) {
							mIsDoingAdjustment = false;
							dialog.dismiss();
						}

					});
					builder.setNegativeButton("Adjusted", new DialogInterface.OnClickListener(){

						@Override
						public void onClick(DialogInterface dialog, int which) {
							isCheckPostiion = true;
							dialog.dismiss();
						}

					});
					builder.create().show();
			 }

			}else if(positionIndex==1){
				Log.d(TAG,"POSITION FINE");
				mIsDoingAdjustment = false;
			}else if(positionIndex==2){
				Log.e(TAG,"POSITION TOO FAR");

				mIsDoingAdjustment = true;

				AlertDialog.Builder builder = new AlertDialog.Builder(mMainAct);
				builder.setTitle(mSavedBTName+"'s Position Too Far From AP!");
				builder.setMessage("Suggest to adjust position close to AP!");
				builder.setPositiveButton("NO,thanks!",new DialogInterface.OnClickListener(){

					@Override
					public void onClick(DialogInterface dialog, int which) {
						mIsDoingAdjustment = false;
						dialog.dismiss();
					}

				});
				builder.setNegativeButton("Adjusted", new DialogInterface.OnClickListener(){

					@Override
					public void onClick(DialogInterface dialog, int which) {
						isCheckPostiion = true;
						dialog.dismiss();
					}

				});
				builder.create().show();
			}
		}
	}

	/** wrong password for AP **/
	private void wrongKeyForAP(String ssid){
		if(mAPConnectedFlag || mRemoteAPSSID == null)
			return;
		if(	mUIState == UIState.STATE_UI_SHOW_CHOICE_FOR_AP
				|| mUIState == UIState.STATE_UI_CANCEL_CHOICE_FOR_AP
				|| mUIState == UIState.STATE_UI_CANCEL_WRONG_PASSWORD
				|| mUIState == UIState.STATE_UI_CANCEL_CONNECTING_AP)
			return;

		mUIState = UIState.STATE_UI_SHOW_WRONG_PASSWORD;

		if(isCheckPostiion){
			isCheckPostiion = false;
			stopWaiting();

			AlertDialog.Builder builder = new AlertDialog.Builder(mMainAct);
			builder.setTitle("Warning");
			builder.setMessage("The password is wrong!");
			builder.setPositiveButton("OK", new DialogInterface.OnClickListener(){

				@Override
				public void onClick(DialogInterface dialog, int which) {

					// CONFIG_SAVE_AP_PWD
	            	if(GlobalConfig.CONFIG_SAVE_AP_PWD){
	            		SharedPreferences.Editor editor = mSharePref.edit();
	          			editor.putString(mRemoteAPMAC, "");
	          			editor.commit();
	          		}

					dialog.dismiss();

					if(mCheckBTConfigStateTimer.wlan2GEnabled == 0 && mCheckBTConfigStateTimer.wlan5GEnabled == 1){
						mBTConfig.setBTConfigState(BTConfigState.STATE_BT_SCAN_WLAN_5G_END);
						mWlanScanListView_5G.setVisibility(View.VISIBLE);
					}else{
						mBTConfig.setBTConfigState(BTConfigState.STATE_BT_SCAN_WLAN_2G_END);
						mWlanScanListView_2G.setVisibility(View.VISIBLE);
					}

					show_wifi_list = true;
					isHomeAP_exist = false;

					if(!BTConfig.wifi_list_ready){
						startWaiting("Scan WiFi AP", "Waiting...", 30,"Scan Timeout");
					}

				}

			});
			builder.create().show();
		}


	}

	/** connecting to AP **/
	private void connectToAPFail(){
		if(mAPConnectedFlag || mRemoteAPSSID == null)
			return;
		if(mUIState == UIState.STATE_UI_SHOW_CHOICE_FOR_AP
				|| mUIState == UIState.STATE_UI_CANCEL_CHOICE_FOR_AP
				|| mUIState == UIState.STATE_UI_SHOW_WRONG_PASSWORD
				|| mUIState == UIState.STATE_UI_CANCEL_WRONG_PASSWORD
				|| mUIState == UIState.STATE_UI_CANCEL_CONNECTING_AP
				|| mUIState == UIState.STATE_UI_SHOW_CHOICE_FOR_ADJUSTMENT
				|| mUIState == UIState.STATE_UI_DISMISS_CHOICE_FOR_ADJUSTMENT)
			return;

		if(isCheckPostiion){

			show_fail_msg = true;
			isCheckPostiion = false;

			stopWaiting();

			AlertDialog.Builder builder = new AlertDialog.Builder(mMainAct);
			builder.setTitle("Error");
			builder.setMessage("Connection Fail!");
			builder.setPositiveButton("OK", new DialogInterface.OnClickListener(){

				@Override
				public void onClick(DialogInterface dialog, int which) {
					dialog.dismiss();

					if(mCheckBTConfigStateTimer.wlan2GEnabled == 0 && mCheckBTConfigStateTimer.wlan5GEnabled == 1){
						mBTConfig.setBTConfigState(BTConfigState.STATE_BT_SCAN_WLAN_5G_END);
						mWlanScanListView_5G.setVisibility(View.VISIBLE);
					}else{
						mBTConfig.setBTConfigState(BTConfigState.STATE_BT_SCAN_WLAN_2G_END);
						mWlanScanListView_2G.setVisibility(View.VISIBLE);
					}

					show_wifi_list = true;
					isHomeAP_exist = false;

					if(!BTConfig.wifi_list_ready){
						startWaiting("Scan WiFi AP", "Waiting...", 30,"Scan Timeout");
					}

				}

			});
			builder.create().show();
		}

	}

	private byte[] constructAPProfile(byte band, byte encrypt, String ssid, String mac,String pwd){
		byte[] mAPProfile = new byte[104];

		byte[] mAPSsid_bytes = ssid.getBytes();
		byte[] mAPMac_bytes = MacToByteArray(mac);

		mAPProfile[0] = band;
		mAPProfile[1] = encrypt;

		System.arraycopy(mAPSsid_bytes, 0, mAPProfile, 2, mAPSsid_bytes.length);
		System.arraycopy(mAPMac_bytes, 0, mAPProfile, 34, mAPMac_bytes.length);
		if(encrypt != 0){
			byte[] mAPPwd = pwd.getBytes();
			System.arraycopy(mAPPwd, 0, mAPProfile, 40, mAPPwd.length);
		}

		return mAPProfile;
	}

	/** convert MAC in 00:00:00:00:00:00 format to byte[hex] format **/
	private byte[] MacToByteArray(String hex_str){
    	String[] hex = hex_str.split(":");
    	byte[] returnBytes = new byte[hex.length];
    	for(int i = 0; i < hex.length; i++){
    		returnBytes[i] = (byte)Integer.parseInt(hex[i].substring(0), 16);
    	}
    	return returnBytes;
    }

	public void startWaiting(final String title, String message, int timeout, String timeout_msg)
	{
		Log.d(TAG,"!!! startWaiting !!! "+timeout+ " "+title+" "+message);

		if(pd!=null){
			if(pd.isShowing()){
				pd.dismiss();

				try {
					Thread.sleep(400);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
		}

		pd = new ProgressDialog(MainActivity.this);
		pd.setTitle(title);
		pd.setMessage(message);
		pd.setProgressStyle(ProgressDialog.STYLE_SPINNER);
		pd.setIndeterminate(true);
		pd.setCancelable(false);
		pd.setButton(DialogInterface.BUTTON_NEGATIVE, "Cancel",
				new DialogInterface.OnClickListener() {
					@Override
					public void onClick(DialogInterface dialog, int which) {

						mBTConfig.setBTConfigState(-1);
		            	resetTargetData();
		            	show_home_button();
						if(BTConfig.flag){
		            		mBTConfig.disconnectBTSocket();
						}
						else{
							disconnectBT();
						}


						if(deviceList_alert!=null){
							deviceList_alert.cancel();
							deviceList_alert = null;
						}
					}
				});
		pd.show();

		if(wait_timer != null){
			wait_timer.cancel();
			wait_timer = null;
		}
		wait_timer = new Timer(true);
		WaitTimerStopTask timerTask = new WaitTimerStopTask();
		timerTask.setMsg(timeout_msg);

		if(timeout>0){
			wait_timer.schedule(timerTask, timeout*1000);
		}else{
			wait_timer.schedule(timerTask, 3600*1000);
		}
	}

	public void startWaiting_ConnectAP(final String title, String message, int timeout, String timeout_msg)
	{
		Log.d(TAG,"!!! startWaiting_ConnectAP !!! "+timeout+ " "+title+" "+message);

		if(pd!=null){
			if(pd.isShowing()){
				pd.dismiss();

				try {
					Thread.sleep(400);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
		}

		pd = new ProgressDialog(MainActivity.this);
		pd.setTitle(title);
		pd.setMessage(message);
		pd.setProgressStyle(ProgressDialog.STYLE_SPINNER);
		pd.setIndeterminate(true);
		pd.setCancelable(false);
		pd.show();

		if(wait_timer != null){
			wait_timer.cancel();
			wait_timer = null;
		}
		wait_timer = new Timer(true);
		WaitTimerStopTask timerTask = new WaitTimerStopTask();
		timerTask.setMsg(timeout_msg);

		if(timeout>0){
			wait_timer.schedule(timerTask, timeout*1000);
		}else{
			wait_timer.schedule(timerTask, 3600*1000);
		}
	}

	public void stopWaiting()
	{
		Log.d(TAG,"!!! stopWaiting !!!");
		handler_pd.sendEmptyMessage(0);

		if(wait_timer != null){
			wait_timer.cancel();
			wait_timer = null;
		}

	}

	public class WaitTimerStopTask extends TimerTask
	{
		private String msg = "";

		public void setMsg(String arg) {
			msg = arg;
		}

		public void run(){
			Log.d(TAG,"!!! stopWaiting !!! WaitTimerStopTask");

			if(wait_timer != null && msg.length()>0){
				if(BTConfig.flag){
					if(mBTConfig.getBTConfigState() == -1 ){
						mBTConfig.getBTBle().cancelBTScan();
					}else{
						mBTConfig.setBTConfigState(-1);
					}
				}

				runOnUiThread(new Runnable() {
					@Override
					public void run() {
						AlertDialog.Builder builder = new AlertDialog.Builder(mMainAct);
						builder.setTitle("Warning");
						builder.setMessage(msg);
						builder.setPositiveButton("OK", new DialogInterface.OnClickListener(){

							@Override
							public void onClick(DialogInterface dialog, int which) {

								if(msg.equals("Scan Timeout") || msg.equals("Connection Timeout")){
									mBTConfig.setBTConfigState(-1);
						            resetTargetData();
						            show_home_button();
						            if(BTConfig.flag){
			            				mBTConfig.disconnectBTSocket();
									}
									else{
										disconnectBT();
									}
								}

								msg = "";
								if(deviceList_alert!=null){
									deviceList_alert.cancel();
									deviceList_alert = null;
								}

								dialog.dismiss();
							}
						});
						builder.create().show();
					}
				});


			}

			stopWaiting();
		}
	};



    @Override
    int getContentViewId() {
        return R.layout.activity_main;
    }

    @Override
    int getNavigationMenuItemId() {
        return R.id.navigation_home;
    }
}

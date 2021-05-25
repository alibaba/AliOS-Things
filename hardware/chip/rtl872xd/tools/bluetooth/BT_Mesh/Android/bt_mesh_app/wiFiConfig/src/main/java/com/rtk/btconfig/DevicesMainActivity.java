package com.rtk.btconfig;

import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.location.LocationManager;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.Process;
import android.os.StrictMode;
import android.preference.PreferenceManager;
import android.support.design.widget.BottomNavigationView;
import android.text.InputType;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ListAdapter;
import android.widget.ListView;
import android.widget.SimpleAdapter;
import android.widget.TextView;
import android.widget.Toast;

import com.google.gson.Gson;
import com.google.gson.reflect.TypeToken;
import com.rtk.btconfig.ConfigurationDevice.DeviceInfo;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.lang.reflect.Type;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.HttpURLConnection;
import java.net.InetAddress;
import java.net.InterfaceAddress;
import java.net.MalformedURLException;
import java.net.NetworkInterface;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.net.URL;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Timer;
import java.util.TimerTask;


public class DevicesMainActivity extends BaseActivity {


	final String[] CtrlListStr = {"Rename","Remove Device","Connect to other AP"};

	public static final String EXTRA_MESSAGE = "com.realtek.amebacam.MESSAGE";

	private static final String pinfileName = "prePINFile";
	private static final String TAG = "MainActivity";


	private static final int discoveryTimeout = 3000; // 3s
	private static final int deviceNumber = 32;
	private static final int APNumber = 100;

	private static final int BROADCAST_RECV_SOCKET_PORT = 49151;
	private static final int BROADCAST_SEND_SOCKET_PORT = 49152;
	private static final int CONTROL_LED_SOCKET_PORT = 8080;


	private DatagramSocket ds,rs = null;
	private int CurrentItem;
	private String CurrentControlIP;
	private static boolean noPD = true;
	private ServerSocket serverSocket;
	private Socket LEDControlSocket;
	private BufferedWriter LEDControlBW;
	private String gMsg;


	private BottomNavigationView mNavigationView = null;
	ImageButton btn_setting;
	ImageButton btn_scan;
	TextView textConfigDeviceTitle;
	ListView listView;
	EditText edittxt_PINcode;
	View wifiPW_EntryView;
	LayoutInflater factory;



	String pinCodeText = "";
	String data_exchange_IP = "";

	boolean DiscovEnable = false;
	boolean isDeletedDevice = false;
	boolean isControlSingleDevice = false;
	boolean TimesupFlag_rename = false;
	boolean is_getIP_finish = false;
	boolean isScanFinish = false;

	private String PINGet = null;
	private String PINSet = null;
	private SimpleAdapter adapter_deviceInfo;
	private ProgressDialog pd;

	public static List<HashMap<String, Object>> BroadcastDevInfo = null;
	private List<HashMap<String, Object>> DevInfo;

	protected WifiManager mWifiManager;
	private HashMap<String, Object> tmpMap = null;


	DeviceInfo[] configuredDevices;
	DeviceInfo[] APInfo;
	DeviceInfo SelectedAPInfo;

	public static Context context = null;

	int mPosition;
	ViewGroup mParent;

	Handler handler_pd = new Handler() {
		@Override
		public void handleMessage(Message msg) {
			// Log.d(TAG,"handleMessage msg.what: " + String.valueOf(msg.what));
			switch (msg.what) {
			case 0:
				if(pd.isShowing()){
					pd.dismiss();
				}
				break;
			case 1:
				int timeout = 10;
				int coutDown = timeout;

				while (coutDown > 0) {

					try {
						Thread.sleep(1000);
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
					coutDown--;
					if (coutDown == 0) {
						pd.dismiss();
					}
				}

				break;
				case 2:
					if(pd != null) {
						if (pd.isShowing()) {
							pd.dismiss();
						}
					}
					show_discoverDevice();
					break;
				case 3:
					show_discoverDevice();
					break;
				case 4:
					AlertDialog.Builder builder = new AlertDialog.Builder(DevicesMainActivity.this);
					builder.setTitle("Warning");
					builder.setMessage(gMsg);
					builder.setCancelable(true);
					final AlertDialog dlg = builder.create();
					dlg.show();
					final Timer t = new Timer();
					t.schedule(new TimerTask() {
						public void run() {
							dlg.dismiss();
							t.cancel();
							return;
						}
					}, 3000);
					break;
			default:
				break;
			}
		}
	};


	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		if (android.os.Build.VERSION.SDK_INT > 9) {
			StrictMode.ThreadPolicy policy = new StrictMode.ThreadPolicy.Builder().permitAll().build();
			StrictMode.setThreadPolicy(policy);
		}


//
//		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
//				WindowManager.LayoutParams.FLAG_FULLSCREEN);

		initData();
		initComponent();
		initComponentAction();

		mWifiManager = (WifiManager) getApplicationContext().getSystemService(WIFI_SERVICE);

	}

	protected void onStart() {
		super.onStart();

		if(context==null)
			context = getApplicationContext();
	}

	@Override
	public void onPause() {
		super.onPause();
		SharedPreferences pref = this.getPreferences(MODE_PRIVATE);
		SharedPreferences.Editor editor = pref.edit();

		Gson gson = new Gson();
		String BDevInfoStr = gson.toJson(BroadcastDevInfo);
		editor.putString(TAG,BDevInfoStr);
		editor.commit();
	}

	@Override
	protected void onResume() {
		super.onResume();

	}

	@Override
	protected void onStop() {
		super.onStop();

		Log.e(TAG,"onStop");
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
		if (ds != null) {
			ds.close();
		}
		if (rs != null) {
			rs.close();
		}
		DiscovEnable = false;
	}


	//Handling callback
	@Override
	public void onRequestPermissionsResult(int requestCode,
                                           String permissions[], int[] grantResults) {
		switch (requestCode) {
			case 1: {
				if (grantResults.length > 0
						&& grantResults[0] == PackageManager.PERMISSION_GRANTED) {


				} else {
					Toast.makeText(this, "Permissions Denied to use MIC", Toast.LENGTH_LONG).show();
				}
				return;
			}
		}
	}


	private void initData() {
		configuredDevices = new DeviceInfo[deviceNumber];
		for (int i = 0; i < deviceNumber; i++) {
			configuredDevices[i] = new DeviceInfo();
			configuredDevices[i].setaliveFlag(1);
			configuredDevices[i].setName("");
			configuredDevices[i].setIP("");
			configuredDevices[i].setmacAdrress("");
			configuredDevices[i].setimg(null);
			configuredDevices[i].setconnectedflag(false);
		}

		APInfo = new DeviceInfo[APNumber];
		for (int i = 0; i < APNumber; i++) {
			APInfo[i] = new DeviceInfo();
		}

		SelectedAPInfo = new DeviceInfo();
		APList_Clear();

	}

	private void resetData() {
		for (int i = 0; i < deviceNumber; i++) {
			configuredDevices[i] = new DeviceInfo();
			configuredDevices[i].setaliveFlag(0);
			configuredDevices[i].setName("");
			configuredDevices[i].setIP("");
			configuredDevices[i].setmacAdrress("");
			configuredDevices[i].setimg(null);
			configuredDevices[i].setconnectedflag(false);
		}
	}

	private void initComponent() {

		DisplayMetrics metrics = new DisplayMetrics();
		getWindowManager().getDefaultDisplay().getMetrics(metrics);
		btn_scan = (ImageButton) findViewById(R.id.btn_scanDevices);

		textConfigDeviceTitle = (TextView) findViewById(R.id.textDeviceTitle);
		listView = (ListView) findViewById(R.id.listView1);
		listView.setBackgroundColor(0xFFE6E6E2);
//!!
		mNavigationView = (BottomNavigationView)findViewById(R.id.navigation);

//		mNavigationView.setVisibility(View.GONE);
	}

	private void initComponentAction() {

		int androidBuildVersion = android.os.Build.VERSION.SDK_INT;// (Android 4.0:14, 5.0:22, 6.0:23)
		
		btn_scan.setOnTouchListener(new Button.OnTouchListener() {

			@Override
			public boolean onTouch(View arg0, MotionEvent event) {

				if (event.getAction() == MotionEvent.ACTION_DOWN) {
					btn_scan.getBackground().setAlpha(100);
				} else if (event.getAction() == MotionEvent.ACTION_UP) {
					btn_scan.getBackground().setAlpha(255);
				}

				return false;
			}
		});
			SharedPreferences db = PreferenceManager.getDefaultSharedPreferences(this.getApplicationContext());
			Gson gson = new Gson();
			String retBDevInfoStr = db.getString(TAG, null);
			Type type = new TypeToken<ArrayList<HashMap<String, Object>>>() {}.getType();
			BroadcastDevInfo = gson.fromJson(retBDevInfoStr, type);
			String defBDevInfoStr = gson.toJson(BroadcastDevInfo);
			if (!defBDevInfoStr.contains("null")) {
				Iterator<HashMap<String, Object>> it = BroadcastDevInfo.iterator();
				HashMap<String, Object> map = null;
				for (; it.hasNext(); ) {
					map = it.next();
					map.put("Status", "");
				}
				noPD = true; //no pogressdialog
				show_discoverDevice();
				DiscoveryDevice(3000);
			}

		if(androidBuildVersion >= 23 && !isLocationServiceEnabled()){
			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					AlertDialog.Builder errorAlert = new AlertDialog.Builder(
							DevicesMainActivity.this);

					errorAlert.setTitle("Warning");
					errorAlert.setMessage("Please check the location service enabled if the WiFi scan failed.");
					errorAlert.setPositiveButton("OK",null);
					errorAlert.show();
				}
			});
		}
	}

	private boolean isLocationServiceEnabled()
	{
		final LocationManager manager = (LocationManager) getSystemService( Context.LOCATION_SERVICE );
		
		return manager.isProviderEnabled( LocationManager.GPS_PROVIDER );
	}

    public InetAddress getIpAddress() {
        InetAddress inetAddress = null;
        InetAddress myAddr = null;

        try {
            for (Enumeration<NetworkInterface> networkInterface = NetworkInterface
                    .getNetworkInterfaces(); networkInterface.hasMoreElements();) {

                NetworkInterface singleInterface = networkInterface.nextElement();

                for (Enumeration< InetAddress > IpAddresses = singleInterface.getInetAddresses(); IpAddresses
                        .hasMoreElements();) {
                    inetAddress = IpAddresses.nextElement();

                    if (!inetAddress.isLoopbackAddress() && (singleInterface.getDisplayName()
                            .contains("wlan0") ||
                            singleInterface.getDisplayName().contains("eth0") ||
                            singleInterface.getDisplayName().contains("ap0"))) {

                        myAddr = inetAddress;
                    }
                }
            }

        } catch (SocketException ex) {
            Log.e(TAG, ex.toString());
        }
        return myAddr;
    }

    public InetAddress getBroadcast(InetAddress inetAddr) {

        NetworkInterface temp;
        InetAddress iAddr = null;
        try {
            temp = NetworkInterface.getByInetAddress(inetAddr);
            List<InterfaceAddress> addresses = temp.getInterfaceAddresses();

            for (InterfaceAddress inetAddress: addresses)

                iAddr = inetAddress.getBroadcast();
            Log.d(TAG, "iAddr=" + iAddr);
            return iAddr;

        } catch (SocketException e) {

            e.printStackTrace();
            Log.d(TAG, "getBroadcast" + e.getMessage());
        }
        return null;
    }

	private void DiscoveryDevice(final int counts) 
	{
		if(!noPD) {
			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					pd = new ProgressDialog(DevicesMainActivity.this);
					pd.setMessage("Discovering devices ...");
					pd.setProgressStyle(ProgressDialog.STYLE_SPINNER);
					pd.setIndeterminate(true);
					pd.setCancelable(false);
					pd.setButton(DialogInterface.BUTTON_NEGATIVE, "Cancel",
							new DialogInterface.OnClickListener() {
								@Override
								public void onClick(DialogInterface dialog, int which) {
									dialog.dismiss();
									DiscovEnable = false;
								}
							});
					pd.show();
				}
			});
		}
		new Thread(new Runnable() {
			public void run() {
				try {
					InetAddress broadCastAddress = InetAddress.getByName("255.255.255.255");


                	WifiManager wifi = (WifiManager) getApplicationContext().getSystemService(Context.WIFI_SERVICE);
                	Method[] wmMethods = wifi.getClass().getDeclaredMethods();
                	for (Method method: wmMethods) {
                    	if (method.getName().equals("isWifiApEnabled")) {

                        	try {
                            	if ((Boolean) method.invoke(wifi)) {
									broadCastAddress = getBroadcast(getIpAddress());
                               	 	Log.e(TAG, "WifiTether on");
                            	} else {

                                	Log.e(TAG, "WifiTether off");
                            	}
                        	} catch (IllegalArgumentException e) {
                            	e.printStackTrace();
                        	} catch (IllegalAccessException e) {
                            	e.printStackTrace();
                        	} catch (InvocationTargetException e) {
                            	e.printStackTrace();
                        	}

                    	}
                	}


					DatagramPacket sp;
					DatagramPacket rp;
					HashMap<String, Object> map = null;
					if(BroadcastDevInfo == null)
						BroadcastDevInfo = new ArrayList<HashMap<String, Object>>();
					else{
						Iterator<HashMap<String, Object>> it = BroadcastDevInfo.iterator();
						HashMap<String, Object> Map = null;
						for(;it.hasNext();){
							Map = it.next();
							Map.put("Status", "");
						}
					}

					byte[] rcvmsg = new byte[64];

					byte[] testmsg = ("AMEBA:DISCOVER:"+ System.currentTimeMillis() +"\0").getBytes();
					sp = new DatagramPacket(testmsg, testmsg.length);
					rp = new DatagramPacket(rcvmsg, rcvmsg.length);

					try {
						sp.setAddress(broadCastAddress);
						sp.setPort(BROADCAST_SEND_SOCKET_PORT);
						ds = new DatagramSocket(BROADCAST_SEND_SOCKET_PORT);
						ds.setBroadcast(true);

						rp.setAddress(broadCastAddress);
						rp.setPort(BROADCAST_RECV_SOCKET_PORT);
						rs = new DatagramSocket(BROADCAST_RECV_SOCKET_PORT);
						rs.setBroadcast(true);
						rs.setSoTimeout(counts);
					} catch (SocketException e) {
						e.printStackTrace();
						String error = e.toString();
						Log.e(TAG, error);
					}  /*catch (UnknownHostException e) {
						e.printStackTrace();
						String error = e.toString();
						Log.e(TAG, error);
					}catch (IOException e) {
						e.printStackTrace();
						String error = e.toString();
						Log.e(TAG, error);
					}*/ catch (Exception e) {
						e.printStackTrace();
						String error = e.toString();
						Log.e(TAG, error);
					}

				for (int j = 0; j < counts/1000; j++) {
						try {
							ds.send(sp);
							rs.receive(rp);

							String rcvstr = new String(rcvmsg, "UTF-8").trim();
							Log.e(TAG,"rcvstr " +rcvstr.toString());
							String[] str = rcvstr.split(";");

							String buf = new String();
							for (int i = 0; i < 6; i++) {
								buf += String.format("%02x", (byte) Character.digit(str[0].charAt(i * 2), 16) * 16 + Character.digit(str[0].charAt((i * 2) + 1), 16));
								if (i < 5)
									buf += ":";
							}

							map = new HashMap<String, Object>();
							map.put("MAC", buf);
							map.put("Status", "LAN Access");
							map.put("Type", "Any type");
							map.put("IP", rp.getAddress().getHostAddress().toString());

							map.put("PIN", false);

							try {
								map.put("RELAYPORT", str[1]);
							}catch(ArrayIndexOutOfBoundsException e){
								map.put("RELAYPORT", "554");
								Log.e(TAG, "rcvstr.split RELAYPORT failed");
							}
							map.put("Name", str[2]);
							map.put("RELAYSUFFIX", str[2]);
							try {
                                map.put("RESOLUTION", str[3]);
                                Log.e(TAG, "RESOLUTION "+ str[3]);
							}catch(ArrayIndexOutOfBoundsException e){
                                map.put("RESOLUTION", "720");
                                    Log.e(TAG, "rcvstr.split RESOLUTION failed");
                                }
								try {
									map.put("VERSION", str[4]);
									Log.e(TAG, "VERSION "+ str[4]);
								}catch(ArrayIndexOutOfBoundsException e){
									map.put("VERSION", "unknown");
									Log.e(TAG, "rcvstr.split VERSION failed");
								}
								try {
									map.put("IRLED", str[5]);
									Log.e(TAG, "IRLED "+ str[5]);
								}catch(ArrayIndexOutOfBoundsException e){
									map.put("IRLED", "unknown");
									Log.e(TAG, "rcvstr.split IRLED failed");
								}



                                Iterator<HashMap<String, Object>> it = BroadcastDevInfo.iterator();
                                boolean deviceExisted = false;
                                HashMap<String, Object> defMap = null;
                                for(;it.hasNext();){
                                    defMap = it.next();

                                    if(defMap.containsValue(map.get("MAC"))){//compare MAC address to check if device existed

                                        deviceExisted = true;
                                        defMap.put("Status", "LAN Access");
                                        defMap.put("IP", rp.getAddress().getHostAddress().toString());
                                        defMap.put("RELAYPORT", map.get("RELAYPORT"));
                                        defMap.put("RELAYSUFFIX",  map.get("RELAYSUFFIX"));
										try {
											defMap.put("RESOLUTION", str[3]);
											defMap.put("VERSION", str[4]);
											defMap.put("IRLED", str[5]);
										}catch(ArrayIndexOutOfBoundsException e){
											defMap.put("RESOLUTION", "720");
											defMap.put("VERSION", "unknown");
											defMap.put("IRLED", "unknown");
											Log.e(TAG, "rcvstr.split ArrayIndexOutOfBoundsException failed");
										}

                                        break;
                                    }
                                }
                                if (!deviceExisted) {
                                    BroadcastDevInfo.add(map);
                                }

                            } catch (IOException e) {
                                e.printStackTrace();
                            }catch (NullPointerException e) {
                                e.printStackTrace();
                            }
                    }

                    try {
                        if (ds != null) {
                            ds.close();
                        }
                        if (rs != null) {
                            rs.close();
                        }
                        SharedPreferences db = PreferenceManager.getDefaultSharedPreferences(context);
                        Gson gson = new Gson();
                        String retBDevInfoStr = db.getString(TAG, null);

                        Type type = new TypeToken<ArrayList<HashMap<String, Object>>>() {
                        }.getType();
                        ArrayList<HashMap<String, Object>> defBDevInfo = gson.fromJson(retBDevInfoStr, type);
                        String defBDevInfoStr = gson.toJson(defBDevInfo);
                        String BDevInfoStr = gson.toJson(BroadcastDevInfo);
                        //if (defBDevInfoStr.contains("null"))
                        {
                            SharedPreferences.Editor defCollection = db.edit();
                            defCollection.putString(TAG, BDevInfoStr);
                            defCollection.commit();
                        }

				}catch(NullPointerException e)
				{
					e.printStackTrace();
				}
				if (noPD) {
					noPD = false;
					handler_pd.sendEmptyMessage(3);
				} else {
					handler_pd.sendEmptyMessage(2);
					Log.e(TAG, "NOPD!!!!");
				}
				Log.e(TAG, " DiscoveryDevice finish");
				}catch (UnknownHostException e){
					e.printStackTrace();
				}
			}
		}).start();

	}




	private void APList_Clear() {
		for (int i = 0; i < APNumber; i++) {
			APInfo[i].setconnectedflag(false);
			APInfo[i].setaliveFlag(0);
			APInfo[i].setName("");
			APInfo[i].setIP("");
			APInfo[i].setmacAdrress("");
			APInfo[i].setsecurityType(0);
			APInfo[i].setimg(null);
		}

		SelectedAPInfo = new DeviceInfo();
		SelectedAPInfo.setconnectedflag(false);
		SelectedAPInfo.setaliveFlag(0);
		SelectedAPInfo.setName("");
		SelectedAPInfo.setIP("");
		SelectedAPInfo.setmacAdrress("");
		SelectedAPInfo.setsecurityType(0);
		SelectedAPInfo.setimg(null);

	}

	private boolean checkDeviceInformation(String macAddress) {
		boolean ret = false;
		int deviceNum = 0;
		String tmp_mac = "";

		if (macAddress.length() == 0 || macAddress == null)
			return ret;

		deviceNum = DevInfo.size();

		for (int i = 0; i < deviceNum; i++) {

			tmp_mac = (String) DevInfo.get(i).get("MAC");
			if (macAddress.equals(tmp_mac)) {
				CurrentControlIP = (String) DevInfo.get(i).get("IP");
				if (CurrentControlIP.length() == 0
						|| CurrentControlIP.equals("0.0.0.0"))
					continue;
				ret = true;
				break;
			}
		}

		return ret;
	}

	private void getkDeviceInformation(final String device,
			final String macAddress) {
		is_getIP_finish = false;

		pd = new ProgressDialog(DevicesMainActivity.this);

		pd.setTitle("Waiting");
		pd.setMessage("Getting " + device + " information...");
		pd.setProgressStyle(ProgressDialog.STYLE_SPINNER);
		pd.setIndeterminate(true);
		pd.setCancelable(false);
		pd.setButton(DialogInterface.BUTTON_NEGATIVE, "Cancel",
				new DialogInterface.OnClickListener() {
					@Override
					public void onClick(DialogInterface dialog, int which) {
						dialog.dismiss();
					}
				});
		pd.show();

		new Thread(new Runnable() {
			public void run() {
				Process.setThreadPriority(Process.THREAD_PRIORITY_DEFAULT);

				boolean isCtlPIN = false;
				int deviceNum = -1;
				int iDevice = -1;
				int i = -1;
				String tmp_mac = "";
				long startTime = System.currentTimeMillis();
				long endTime = System.currentTimeMillis();
				while (!is_getIP_finish && (endTime - startTime) < 30000) {

					try {
						Thread.sleep(500);
					} catch (InterruptedException e) {
						e.printStackTrace();
					}

					endTime = System.currentTimeMillis();

					// check the ip of device
					if (checkDeviceInformation(macAddress)) {
						is_getIP_finish = true;
						break;
					}
				}
				handler_pd.sendEmptyMessage(0);

				// Update Status
				Message msg = Message.obtain();
				msg.obj = null;
				deviceNum = DevInfo.size();

				if (is_getIP_finish) {

					for (i = 0; i < deviceNum; i++) {
						tmp_mac = (String) DevInfo.get(i).get("MAC");
						if (macAddress.equals(tmp_mac)) {
							iDevice = i;
							break;
						}
					}

					if (iDevice >= 0) {

						if (DevInfo.get(iDevice).get("PIN") != null) {
							isCtlPIN = (Boolean) DevInfo.get(iDevice).get("PIN");
						}

						final boolean isDevicePIN = isCtlPIN;
						final String deviceName = (String) DevInfo.get(iDevice).get("Name");
						final String deviceMAC = (String) DevInfo.get(iDevice).get("MAC");
						final String deviceType = (String) DevInfo.get(CurrentItem).get("Type");

						runOnUiThread(new Runnable() {
							@Override
							public void run() {
								Log.d(TAG, "Setting: " + deviceName + " "+ deviceMAC + " " + isDevicePIN);

								if (!isDevicePIN)
									CtlDefaultPINcode(deviceName, deviceMAC, deviceType, -1);
								else
									ControlPINcode(deviceName, deviceMAC, deviceType);
							}
						});
					}

				}

			}
		}).start();
	}

	private boolean show_discoverDevice() {

		adapter_deviceInfo = new SimpleAdapter(this, getData_Device(),
				R.layout.device_main_list, new String[] { "title", "info" },
				new int[] { R.id.title, R.id.info })
		{
			@Override
			public View getView(int position, View convertView, ViewGroup parent)
			{
				View v = super.getView(position, convertView, parent);
				btn_setting = (ImageButton) v.findViewById(R.id.img_btn_setting);
				mParent = parent;
				mPosition = position;

				HashMap<String, Object> map = null;
				String status = null, mac = null;
				int i=0;

				Iterator<HashMap<String, Object>> it = BroadcastDevInfo.iterator();
				for(;it.hasNext();){
					map = it.next();
					status = (String) map.get("Status");
					if(i == position)
						break;
					i++;
				}

				btn_setting.setOnClickListener(new View.OnClickListener()  {
					@Override
					public void onClick(View view) {

						boolean isCtlPIN = false;
						String deviceType = "";
						ListView listView_parent = (ListView) mParent;
						listView_parent.setTag(view);

						String[] tmp = listView_parent.getItemAtPosition(mPosition)
								.toString().split("title=");
						String tmp1 = tmp[1];
						String[] deviceName = tmp1.split(",");

						tmp = listView_parent.getItemAtPosition(mPosition).toString().split("info=");
						String macAddress = tmp[1];
						macAddress = macAddress.replaceFirst("\\}", "");
						macAddress = macAddress.substring(0, 17);
						CurrentItem = mPosition;

						Log.d(TAG, "listView choose " + deviceName[0] + " "	+ macAddress);


						if (isScanFinish) {
							CurrentControlIP = (String) DevInfo.get(CurrentItem).get("IP");

							deviceType = (String) DevInfo.get(CurrentItem).get("Type");

							if (DevInfo.get(CurrentItem).get("PIN") != null) {
								isCtlPIN = (Boolean) DevInfo.get(CurrentItem).get("PIN");
							}

							if (!isCtlPIN) {
								CtlDefaultPINcode(deviceName[0], (String) DevInfo.get(CurrentItem).get("MAC"), deviceType, mPosition);
							}else {
								ControlPINcode(deviceName[0], (String) DevInfo.get(CurrentItem).get("MAC"), deviceType);
							}
						} else {
							// check list info after first confirm device
							getkDeviceInformation(deviceName[0], macAddress);
						}
					}
				});
				return v;
			}

		};
		listView.setAdapter(adapter_deviceInfo);
		if (adapter_deviceInfo.getCount() > 0) {
			setListViewHeightBasedOnChildren(listView);
		}
		else if (adapter_deviceInfo.getCount() == 0) {
			listView.setAdapter(null);

		}

		listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {

			@Override
			public void onItemClick(AdapterView<?> parent, View view,
                                    int itemId, long arg3) {
				HashMap<String, Object> map = null;
				int i=0;

				Iterator<HashMap<String, Object>> it = BroadcastDevInfo.iterator();
				for(;it.hasNext();){
					map = it.next();
					if(i == itemId)
						break;
					i++;
				}


//waitting by modified by sean
				Intent intent = new Intent(context,httpRequestActivity.class);
				intent.putExtra(EXTRA_MESSAGE, map);
				startActivity(intent);

			}
		});
		return true;
	}


	public void ScanDevices_OnClick(View view) {

		ConnectivityManager connManager = (ConnectivityManager) getSystemService(Context.CONNECTIVITY_SERVICE);
		NetworkInfo mWifi = connManager.getNetworkInfo(ConnectivityManager.TYPE_WIFI);

			DiscovEnable = true;
			noPD = false;
			DiscoveryDevice(discoveryTimeout);
	
	}



	public void About_OnClick(View view) {
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
			appVersion = "1.0";

		new AlertDialog.Builder(DevicesMainActivity.this)
				.setIcon(android.R.drawable.ic_menu_info_details)
				.setTitle("About").setCancelable(false)
				.setMessage("Realtek BT MESH v" + appVersion)
				.setPositiveButton("OK", null).show();
	}

	// <func>
	public void ControlPINcode(final String deviceName, final String macAddress, final String deviceType) {

		factory = LayoutInflater.from(DevicesMainActivity.this);

		edittxt_PINcode.setText("", TextView.BufferType.EDITABLE);
		edittxt_PINcode.setInputType(InputType.TYPE_CLASS_NUMBER);
		edittxt_PINcode.setTextSize(20);

		// try to get success pin code by pinfilename
		String content = "";
		byte[] buff = new byte[256]; // input stream buffer
		try {
			FileInputStream reader = openFileInput(pinfileName);
			while ((reader.read(buff)) != -1) {
				content += new String(buff).trim();
			}
			reader.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
			Log.e(TAG, "pin code FileNotFoundException: " + content);
		} catch (IOException e) {
			Log.e(TAG, "pin code IOException: " + content);
		}

		if (content.length() > 0) {
			String[] DeviceItem = content.split(";");
			int itemNumber = DeviceItem.length;

			for (int i = 0; i < itemNumber; i++) {
				String[] array = DeviceItem[i].split(",");
				if (macAddress.equals(array[0]) == true) {// bssid is same
					edittxt_PINcode.setText(array[1],
							TextView.BufferType.EDITABLE);
					break;
				}
			}
		}

		pinCodeText = edittxt_PINcode.getText().toString();
		PINGet = edittxt_PINcode.getText().toString();

		AlertDialog.Builder alert = new AlertDialog.Builder(DevicesMainActivity.this);
		alert.setCancelable(false);
		alert.setTitle("Key in " + deviceName + " PIN code");
		alert.setMessage("The PIN code will be display on device if the PIN code is exist.");
		alert.setCancelable(false);
		alert.setView(wifiPW_EntryView);
		alert.setPositiveButton("Cancel", null);
		alert.setNegativeButton("OK", new DialogInterface.OnClickListener() {

			@Override
			public void onClick(DialogInterface arg0, int arg1) {

				if (edittxt_PINcode.getText().toString().length() <= 0) {
					Toast.makeText(DevicesMainActivity.this,
							"Warning: The PIN code is empty!",
							Toast.LENGTH_LONG).show();
					return;
				}


				if (edittxt_PINcode.getText().toString().length() > 0) {
					pinCodeText = edittxt_PINcode.getText().toString();
					PINGet = edittxt_PINcode.getText().toString();

					byte[] pinget = PINGet.getBytes();
					byte[] pinset;
					if (pinget.length > 0) {
						if (pinget.length < 8) {
							pinset = new byte[8];
							System.arraycopy(pinget, 0, pinset, 0,
									pinget.length);
							for (int i = pinget.length; i < 8; i++) {
								pinset[i] = '0';
							}
						} else if (pinget.length >= 8 && pinget.length <= 64) {
							pinset = new byte[pinget.length];
							System.arraycopy(pinget, 0, pinset, 0,
									pinget.length);
						} else {
							pinset = new byte[64];
							System.arraycopy(pinget, 0, pinset, 0, 64);
						}
						PINSet = new String(pinset);
					} else {
						PINSet = new String(pinget);
					}
				}

				AlertDialog.Builder dialogaction = new AlertDialog.Builder(DevicesMainActivity.this);
				// Get the layout inflater
				dialogaction.setCancelable(false);
				dialogaction.setTitle("Setting action");
				dialogaction.setIcon(R.drawable.settings);

				
				if(deviceType.equals("Multi_Media")){
					dialogaction.setItems(CtrlListStr,new DialogInterface.OnClickListener(){

						@Override
						public void onClick(DialogInterface dialog, int which) {
							 if(which==0){
								 rename_OnClick(deviceName);
							 }else if(which==1){
								 remove_OnClick(-1);
							 }
						}
					});
				}else{
					dialogaction.setNeutralButton("Remove device",
							new DialogInterface.OnClickListener() {
								public void onClick(DialogInterface dialog, int id) {
									dialog.cancel();
									remove_OnClick(-1);

								}
							});

					dialogaction.setNegativeButton("Rename",
							new DialogInterface.OnClickListener() {
								public void onClick(DialogInterface dialog, int id) {
									dialog.cancel();
									rename_OnClick(deviceName);
								}
							});
				}
				
				
				dialogaction.setPositiveButton("Exit", new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int id) {
						dialog.cancel();
					}
				});
				dialogaction.show();
				
			}

		});
		alert.show();
	}


	
	public void CtlDefaultPINcode(final String deviceName, final String macAddress, final String deviceType, final int itemId) {

		AlertDialog.Builder dialogaction = new AlertDialog.Builder(
				DevicesMainActivity.this);
		// Get the layout inflater
		dialogaction.setCancelable(false);
		dialogaction.setTitle("Setting action\n");
		dialogaction.setIcon(R.drawable.settings);
		
		if(deviceType.equals("Multi_Media")){
		//do something
		}else{
			final String[] SettingListStr  ;
			int i = 0;
			Iterator<HashMap<String, Object>> it = BroadcastDevInfo.iterator();

			for(;it.hasNext();) {
				tmpMap = it.next();
				if (i == itemId )
					break;
				i++;
			}


	
			SettingListStr = new String[]{ "Remove Device", "CVR:"+tmpMap.get("VERSION").toString(),  tmpMap.get("IP").toString()};
			
//			dialogaction.setNeutralButton("Remove device",
//					new DialogInterface.OnClickListener() {
//						public void onClick(DialogInterface dialog, int id) {
//							dialog.cancel();
//							remove_OnClick(itemId);
//
//						}
//					});
			dialogaction.setItems(SettingListStr,new DialogInterface.OnClickListener(){

				@Override
				public void onClick(DialogInterface dialog, int which) {
					if(which==0){
						dialog.cancel();
						remove_OnClick(itemId);
					}else if(which==2) {


						if(tmpMap.get("IP").toString() == null){
							gMsg ="No IP found";
							handler_pd.sendEmptyMessage(4);
							return;
						}else if(tmpMap.get("IRLED").toString().contains("unknown")){
							gMsg ="Unknow IR Led Status";
							handler_pd.sendEmptyMessage(4);
							return;
						}

						new Thread(new Runnable() {
							public void run() {
								String IRLEDSendStr = "";
								if(tmpMap.get("IRLED").toString().equals("0")){
									IRLEDSendStr = "1";
								}else if(tmpMap.get("IRLED").toString().equals("1")){
									IRLEDSendStr = "0";
								}

								try {
									String msg_ctrl = "AMEBA:CONTROL:IRLED:"+ IRLEDSendStr  +"\0";
									InetAddress serverIp = InetAddress.getByName(tmpMap.get("IP").toString());
									LEDControlSocket = new Socket(serverIp, CONTROL_LED_SOCKET_PORT);
									LEDControlSocket.setSoTimeout(30);
									LEDControlBW = new BufferedWriter( new OutputStreamWriter(LEDControlSocket.getOutputStream()));
									while (!LEDControlSocket.isConnected()) {
										Thread.sleep(100);
									}
									LEDControlBW.write(msg_ctrl);
									LEDControlBW.close();
									LEDControlSocket.close();
									DiscoveryDevice(discoveryTimeout);
								}catch (InterruptedException |UnknownHostException e){
									e.printStackTrace();
									gMsg ="Server Connection timeout";
									handler_pd.sendEmptyMessage(4);
								}catch (IOException e){
									gMsg ="Server Connection timeout";
									handler_pd.sendEmptyMessage(4);
								}

							}
						}).start();

					}

				}
			});
		}
		
		
		dialogaction.setPositiveButton("Exit", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int id) {
				dialog.cancel();
			}
		});

		dialogaction.show();
	}



	@SuppressLint("NewApi")

	public void rename_OnClick(String deviceName) {
		LayoutInflater factory = LayoutInflater.from(this);
		final View textEntryView = factory
				.inflate(R.layout.device_rename, null);
		final EditText edit_device_name = (EditText) textEntryView
				.findViewById(R.id.id_device_name);

		edit_device_name.setText(deviceName, TextView.BufferType.EDITABLE);
		edit_device_name.setTextSize(20);

		AlertDialog.Builder alert = new AlertDialog.Builder(DevicesMainActivity.this);
		alert.setTitle("Enter the Device Name:");
		alert.setCancelable(false);
		alert.setView(textEntryView);
		alert.setPositiveButton("Cancel",
				new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int whichButton) {

					}
				});
		alert.setNegativeButton("Save", new DialogInterface.OnClickListener() {

			@Override
			public void onClick(DialogInterface arg0, int arg1) {

				String new_deviceName = edit_device_name.getText().toString();
				if (new_deviceName.length() > 0) {

					pd = new ProgressDialog(DevicesMainActivity.this);
					pd.setTitle("Device Renaming ");
					pd.setMessage("Please wait...");
					pd.setProgressStyle(ProgressDialog.STYLE_SPINNER);
					pd.setIndeterminate(true);
					pd.setCancelable(false);
					pd.setButton(DialogInterface.BUTTON_NEGATIVE, "Cancel",
							new DialogInterface.OnClickListener() {
								@Override
								public void onClick(DialogInterface dialog,
										int which) {
									dialog.dismiss();
									DiscovEnable = false;
									TimesupFlag_rename = false;
									AlertDialog.Builder alert = new AlertDialog.Builder(DevicesMainActivity.this);
									alert.setCancelable(false);
									// switch password input type
									alert.setTitle("Rename Device Failed");
									alert.setCancelable(false);
									alert.setPositiveButton("OK", null);
									alert.show();
								}
							});
					pd.show();
					isControlSingleDevice = true;
					TimesupFlag_rename = false;
					new Thread() {
						public void run() {
							try {
								Thread.sleep(10000);
								isControlSingleDevice = false;
								if (TimesupFlag_rename == false) {
									runOnUiThread(new Runnable() {

										@Override
										public void run() {

											handler_pd.sendEmptyMessage(0);

											AlertDialog.Builder alert = new AlertDialog.Builder(
													DevicesMainActivity.this);
											alert.setCancelable(false);
											alert.setTitle("Rename Device Failed");
											alert.setCancelable(false);
											alert.setPositiveButton("OK", null);
											alert.show();
										}
									});
								} else {

									DiscovEnable = true;
									Process.setThreadPriority(Process.THREAD_PRIORITY_DEFAULT);
									byte[] DiscovCmdBuf ;
									for (int i = 0; i < 15; i++) {
										Thread.sleep(200);
									}

									// Update Status
									Message msg = Message.obtain();
									msg.obj = null;

									runOnUiThread(new Runnable() {

										@Override
										public void run() {
											handler_pd.sendEmptyMessage(0);
											show_discoverDevice();
										}
									});

								}

							} catch (InterruptedException e) {
								e.printStackTrace();
							}
						};
					}.start();

				} else {
					Toast.makeText(DevicesMainActivity.this,
							"Warning: The device name is empty!",
							Toast.LENGTH_LONG).show();
				}
			}

		});
		alert.show();
	}

	public void remove_OnClick(final int itemId) {
		pd = new ProgressDialog(DevicesMainActivity.this);
		pd.setTitle("Device Removing ");
		pd.setMessage("Please wait...");
		pd.setProgressStyle(ProgressDialog.STYLE_SPINNER);
		pd.setIndeterminate(true);
		pd.setCancelable(false);
		pd.setButton(DialogInterface.BUTTON_NEGATIVE, "Cancel",
				new DialogInterface.OnClickListener() {
					@Override
					public void onClick(DialogInterface dialog, int which) {
						dialog.dismiss();
						DiscovEnable = false;
						isDeletedDevice = false;
					}
				});
		pd.show();

		isControlSingleDevice = true;

		new Thread() {
			public void run() {
				try {

					isControlSingleDevice = false;

					if (isDeletedDevice == true) {
						DiscovEnable = true;
						Process.setThreadPriority(Process.THREAD_PRIORITY_DEFAULT);
						byte[] DiscovCmdBuf ;
						for (int i = 0; i < 15; i++) {
							Thread.sleep(200);
						}

						// Update Status
						Message msg = Message.obtain();
						msg.obj = null;

						runOnUiThread(new Runnable() {

							@Override
							public void run() {
								handler_pd.sendEmptyMessage(0);
								show_discoverDevice();
							}
						});

					} else {
						runOnUiThread(new Runnable() {

							@Override
							public void run() {
								String ret = "Failed";
								handler_pd.sendEmptyMessage(0);
								AlertDialog.Builder alert = new AlertDialog.Builder(
										DevicesMainActivity.this);
								alert.setCancelable(false);
								// switch password input type
								if(itemId != -1)
								{
									int i = 0;
									Iterator<HashMap<String, Object>> it = BroadcastDevInfo.iterator();
									HashMap<String, Object> map = null;
									for (; it.hasNext(); ) {
										map = it.next();
										if(i == itemId){
											Gson gson = new Gson();
											BroadcastDevInfo.remove(map);
											SharedPreferences db = PreferenceManager.getDefaultSharedPreferences(context);
											SharedPreferences.Editor defCollection = db.edit();
											String BDevInfoStr = gson.toJson(BroadcastDevInfo);
											defCollection.putString(TAG, BDevInfoStr);
											defCollection.commit();
											break;
										}
										i++;
									}
									show_discoverDevice();
									ret = "Successful";
									isDeletedDevice = false;
									return;
								}
								alert.setTitle("Remove Device " + ret);
								alert.setCancelable(false);
								alert.setPositiveButton("OK", null);
								alert.show();
								isDeletedDevice = false;
							}
						});
					}

				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			};
		}.start();

	}


	private List<? extends Map<String, ?>> getData_Device() {

		List<Map<String, Object>> list = new ArrayList<Map<String, Object>>();
		Map<String, Object> map;
		String tmp = "";
		int i = 0;
		if(BroadcastDevInfo.size()>0){
			DevInfo = BroadcastDevInfo;
		}else {
			DevInfo = new ArrayList<HashMap<String, Object>>();
		}

		for (i = 0; i < DevInfo.size(); i++)
		{
			map = new HashMap<String, Object>();
			if (DevInfo.get(i).get("Name") == null) {
				map.put("title", DevInfo.get(i).get("MAC"));
				map.put("info", DevInfo.get(i).get("MAC") + "   "
						+ DevInfo.get(i).get("Status"));
			} else {
				map.put("title", DevInfo.get(i).get("Name"));
				map.put("info", DevInfo.get(i).get("MAC") + "   "
						+ DevInfo.get(i).get("Status"));
			}

			if (DevInfo.get(i).get("VERSION") != null)
				map.put("VERSION", DevInfo.get(i).get("VERSION"));


			tmp = (String) DevInfo.get(i).get("IP");
			if (DevInfo.get(i).get("IP") != null && tmp.length() > 0)
				list.add(map);

		}

		isScanFinish = true;

		return list;
	}

	private void setListViewHeightBasedOnChildren(ListView adjuestlistView) {

		ListAdapter listAdapter = adjuestlistView.getAdapter();
		if (listAdapter == null) {
			return;
		}
		View listItem = null;
		int totalHeight = 0;

		listItem = listAdapter.getView(0, null, adjuestlistView);
		listItem.measure(0, 0);
		totalHeight = listItem.getMeasuredHeight() * listAdapter.getCount();

		ViewGroup.LayoutParams params = adjuestlistView.getLayoutParams();
		params.height = totalHeight
				+ (adjuestlistView.getDividerHeight() * (listAdapter.getCount() - 1));
		adjuestlistView.setLayoutParams(params);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		// getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}


	@Override
	int getContentViewId() {
		return R.layout.activity_devices;
	}

	@Override
	int getNavigationMenuItemId() {
		return R.id.navigation_dashboard;
	}
}

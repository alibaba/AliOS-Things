package com.rtk.btconfig;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.TextView;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLConnection;
import java.net.URLEncoder;
import java.util.HashMap;
import java.util.TreeMap;

public class httpRequestActivity extends BaseActivity {

    private static final String TAG = "httpRequestActivity";
    private String httpinputline = null;
    private static HashMap<String, Object> hashMap;
    TextView queryTextView;
    TextView responseTextView;
    String http_response;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Intent intent = getIntent();
        Button httpSendBtn = (Button) findViewById(R.id.sendButton);
        queryTextView = (TextView) findViewById(R.id.queryText);
        responseTextView = (TextView) findViewById(R.id.responseText);
        queryTextView.setText("test1=one&test2=two");

        hashMap = (HashMap<String, Object>) intent.getSerializableExtra(DevicesMainActivity.EXTRA_MESSAGE);


        httpSendBtn.setOnClickListener(new Button.OnClickListener()  {
            @Override
            public void onClick(View view) {
                if(hashMap != null) {
                    GET_HTTP_FILELIST((String) hashMap.get("IP"));
                }else{
                    responseTextView.setText("NO Server IP found!!");
                }

            }
        });


    }


    private void GET_HTTP_FILELIST(final String server_addr){


        new Thread() {
            public void run() {

                try {
                    StringBuilder stringBuilder = new StringBuilder("http://" + server_addr + "/");
                    stringBuilder.append("test_get?");
                    String[] DeviceItem = queryTextView.getText().toString().split("&");
                    Log.e(TAG, "key" + DeviceItem[0] );
                    int itemNumber = DeviceItem.length;

                    for (int i = 0; i < itemNumber; i++) {
                        String[] array = DeviceItem[i].split("=");
                        if( array.length < 2)
                            break;
                        if(i>0)
                            stringBuilder.append("&");
                        stringBuilder.append(array[0] + "=");
                        stringBuilder.append(URLEncoder.encode(array[1], "UTF-8"));

                    }

//                    stringBuilder.append(URLEncoder.encode(queryTextView.getText().toString(), "UTF-8"));
//                    stringBuilder.append("&test2=");
//                    stringBuilder.append(URLEncoder.encode("you", "UTF-8"));

                    URL obj = new URL(stringBuilder.toString());

                    HttpURLConnection con = (HttpURLConnection) obj.openConnection();
                    con.setRequestMethod("GET");
                    con.setRequestProperty("User-Agent", "Mozilla/5.0");
                    con.setRequestProperty("Accept-Charset", "UTF-8");
                    con.setUseCaches(false);

                    Log.e(TAG, "\nSending request to URL : " + obj.toString());
                    Log.e(TAG, "Response Code : " + con.getResponseCode());
                    Log.e(TAG, "Response Message : " + con.getResponseMessage());

                    BufferedReader in = new BufferedReader(
                            new InputStreamReader(con.getInputStream()));
                    String line;
                    StringBuffer response = new StringBuffer();

                    while ((line = in.readLine()) != null) {
                        response.append(line);
                    }

                    http_response = response.toString();
                    Log.e(TAG, "http content: " + response.toString());
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            responseTextView.setText(http_response);
                        }
                    });
                    in.close();
                }catch (UnsupportedEncodingException e){
                    Log.e(TAG, "UnsupportedEncodingException !! " );
                }catch (MalformedURLException e){
                    Log.e(TAG, "MalformedURLException !! " );
                }catch (IOException e){
                    Log.e(TAG, "IOException !! " );
                }


            }
        }.start();
    }


    @Override
    int getContentViewId() {
        return R.layout.http_request;
    }

    @Override
    int getNavigationMenuItemId() {
        return R.id.navigation_notificaion;
    }

}

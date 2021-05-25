package com.rtk.btconfig;

import android.content.Intent;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.design.widget.BottomNavigationView;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;

public abstract class BaseActivity extends AppCompatActivity implements BottomNavigationView.OnNavigationItemSelectedListener {

    public BottomNavigationView navigationView;
    MenuItem mItem;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(getContentViewId());

        navigationView = (BottomNavigationView) findViewById(R.id.navigation);
        navigationView.setOnNavigationItemSelectedListener(this);

    }

    @Override
    protected void onStart() {
        super.onStart();
        updateNavigationBarState();
    }

    // Remove inter-activity transition to avoid screen tossing on tapping bottom navigation items
    @Override
    public void onPause() {
        super.onPause();
        overridePendingTransition(0, 0);
    }

    @Override
    public boolean onNavigationItemSelected(@NonNull MenuItem item) {
        mItem = item;

        navigationView.postDelayed(new Runnable() {
            @Override
            public void run() {
                int itemId = mItem.getItemId();
                if (itemId == R.id.navigation_home) {
                    startActivity(new Intent(getApplicationContext() , MainActivity.class));
//                    finish();
                } else if (itemId == R.id.navigation_dashboard) {
                    startActivity(new Intent(getApplicationContext(), DevicesMainActivity.class));
//                    finish();
                } else if (itemId == R.id.navigation_notificaion) {
                    startActivity(new Intent(getApplicationContext(), httpRequestActivity.class));
//                    finish();
                }
            }
        }, 0);
//
////        navigationView.postDelayed(() -> {
//            int itemId = item.getItemId();
//        Log.e("fuck","fucking tag");
//            if (itemId == R.id.navigation_home) {
//                startActivity(new Intent(this, MainActivity.class));
////                finish();
//            } else if (itemId == R.id.navigation_dashboard) {
//
//                    startActivity(new Intent(this, DevicesMainActivity.class));
//            } /*else if (itemId == R.id.navigation_notifications) {
//                    startActivity(new Intent(this, JavaActivity.class));
//            }*/
//            finish();
////        }, 0);
        return true;
    }

    private void updateNavigationBarState(){
        int actionId = getNavigationMenuItemId();
        selectBottomNavigationBarItem(actionId);
    }

    void selectBottomNavigationBarItem(int itemId) {
        Menu menu = navigationView.getMenu();
        for (int i = 0, size = menu.size(); i < size; i++) {
            MenuItem item = menu.getItem(i);
            boolean shouldBeChecked = item.getItemId() == itemId;
            if (shouldBeChecked) {
                item.setChecked(true);
                break;
            }
        }
    }

    abstract int getContentViewId();

    abstract int getNavigationMenuItemId();

}

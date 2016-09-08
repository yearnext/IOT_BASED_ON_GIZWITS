package com.example.xzy.myhome.activity;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;

import com.example.xzy.myhome.Config;
import com.example.xzy.myhome.R;
import com.gizwits.gizwifisdk.api.GizWifiSDK;
import com.gizwits.gizwifisdk.enumration.GizLogPrintLevel;

public class StartActivity extends BaseActivity {

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_start);
        Log.d(TAG, "initSdk: 开始初始化sdk");
        GizWifiSDK.sharedInstance().startWithAppID(getApplicationContext(), Config.APP_ID);
        GizWifiSDK.sharedInstance().setLogLevel(GizLogPrintLevel.GizLogPrintI);
        //启动界面选择
        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                if (false) {
                    Intent inter = new Intent(StartActivity.this, LoginActivity.class);
                    startActivity(inter);
                    //// TODO: 16/6/28  select the start interface
                } else {
                    Log.i(TAG, "run: 开始启动登录界面");
                    Intent inter = new Intent(StartActivity.this, LoginActivity.class);
                    startActivity(inter);

                }
                finish();
            }
        }, 1000);
    }
}


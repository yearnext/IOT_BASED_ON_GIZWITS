package com.example.xzy.myhome.activity;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;

import com.example.xzy.myhome.R;
import com.gizwits.gizwifisdk.api.GizWifiSDK;
import com.gizwits.gizwifisdk.enumration.GizLogPrintLevel;

public class StartActivity extends BaseActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_start);
        //init sdk
        GizWifiSDK.sharedInstance().setListener(mListener);
        GizWifiSDK.sharedInstance().startWithAppID(getApplicationContext(), "04d8c15be81a4fafa23ef842f8b8047b");
        GizWifiSDK.sharedInstance().setLogLevel(GizLogPrintLevel.GizLogPrintI);
        //启动界面选择
        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                if (false) {
                    Intent inter = new Intent(StartActivity.this, LoginActivity.class);
                    startActivity(inter);
                    //// TODO: 16/6/28  Judge select the start interface
                } else {
                    Intent inter = new Intent(StartActivity.this, LoginActivity.class);
                    startActivity(inter);
                }
                finish();
            }
        },1000);


    }

}

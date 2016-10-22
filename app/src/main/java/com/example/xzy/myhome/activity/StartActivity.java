package com.example.xzy.myhome.activity;

import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Handler;
import android.preference.PreferenceManager;
import android.util.Log;

import com.example.xzy.myhome.Config;
import com.example.xzy.myhome.R;
import com.example.xzy.myhome.util.ExceptionUtil;
import com.example.xzy.myhome.util.ToastUtil;
import com.gizwits.gizwifisdk.api.GizWifiSDK;
import com.gizwits.gizwifisdk.enumration.GizLogPrintLevel;
import com.gizwits.gizwifisdk.enumration.GizWifiErrorCode;

public class StartActivity extends BaseActivity {
    private SharedPreferences mSharedPreferences;


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_start);
        mSharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
        GizWifiSDK.sharedInstance().startWithAppID(getApplicationContext(), Config.APP_ID);
        GizWifiSDK.sharedInstance().setLogLevel(GizLogPrintLevel.GizLogPrintI);
        }

    @Override
    protected void onStart() {
        super.onStart();
            //启动界面选择
            new Handler().postDelayed(new Runnable() {
                @Override
                public void run() {
                    if (mSharedPreferences.getBoolean("loginState", false)) {
                        String account = mSharedPreferences.getString("account", "");
                        String password = mSharedPreferences.getString("password", "");
                        if (!ExceptionUtil.isException(account, password, StartActivity.this))
                            GizWifiSDK.sharedInstance().userLogin(account, password);
                    }else {
                    Log.i(TAG, "run: 开始启动登录界面");
                    Intent inter = new Intent(StartActivity.this, LoginActivity.class);
                    startActivity(inter);
                    finish();}
                }
            }, 800);
        }


    @Override
    protected void mDidUserLogin(GizWifiErrorCode result, String uid, String token) {
        Log.e(TAG, "didUserLogin: 准备判断result");
        if (result == GizWifiErrorCode.GIZ_SDK_SUCCESS) {
            Intent intent = new Intent(StartActivity.this, MainActivity.class);
            startActivity(intent);
            ToastUtil.showToast(StartActivity.this, "登录成功");
            finish();
        } else {
                    Log.i(TAG, "run: 自动登录失败");
                    Intent inter = new Intent(StartActivity.this, LoginActivity.class);
                    startActivity(inter);

                    finish();
                }


        }
    }



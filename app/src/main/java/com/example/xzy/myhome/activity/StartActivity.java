package com.example.xzy.myhome.activity;

import android.Manifest;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.preference.PreferenceManager;
import android.support.annotation.RequiresApi;
import android.util.Log;

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
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            insertDummyContactWrapper();
        } else {
            GizWifiSDK.sharedInstance().startWithAppID(getApplicationContext(), getApplication().getString(R.string.app_id));
            GizWifiSDK.sharedInstance().setLogLevel(GizLogPrintLevel.GizLogPrintI);
        }

    }

    @Override
    protected void onStart() {
        super.onStart();
        final String account = mSharedPreferences.getString("account", "");
        final String password = mSharedPreferences.getString("password", "");
        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                if (mSharedPreferences.getBoolean("loginState", false) || !ExceptionUtil.isException(account, password, StartActivity.this)) {
                    //GizWifiSDK.sharedInstance().userLogin(account, password);
                    Log.i(TAG, "run: 开始启动登录界面");
                    Intent inter = new Intent(StartActivity.this, LoginActivity.class);
                    startActivity(inter);
                    finish();
                } else {
                    Log.i(TAG, "run: 开始启动登录界面");
                    Intent inter = new Intent(StartActivity.this, LoginActivity.class);
                    startActivity(inter);
                    finish();
                }
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


    @RequiresApi(api = Build.VERSION_CODES.M)
    private void insertDummyContactWrapper() {
        int hasWriteContactsPermission = checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE);
        if (hasWriteContactsPermission != PackageManager.PERMISSION_GRANTED) {
            requestPermissions(new String[]{Manifest.permission.READ_PHONE_STATE,
                            Manifest.permission.WRITE_EXTERNAL_STORAGE,
                            Manifest.permission.ACCESS_COARSE_LOCATION,
                            Manifest.permission.READ_PHONE_STATE,},
                    1);
            //GizWifiSDK.sharedInstance().startWithAppID(getApplicationContext(), getApplication().getString(R.string.app_id));
            //GizWifiSDK.sharedInstance().setLogLevel(GizLogPrintLevel.GizLogPrintI);
        }
    /*    //API23需授权权限
    <uses-permission android:name="android.permission.READ_PHONE_STATE" />
    <uses-permission android:name="android.permission.ACCESS_COARSE_LOCATION" />
    <uses-permission android:name="android.permission.ACCESS_FINE_LOCATION" />
    <uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE" />*/
    }
}



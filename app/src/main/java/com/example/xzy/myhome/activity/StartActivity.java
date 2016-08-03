package com.example.xzy.myhome.activity;

import android.Manifest;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.support.annotation.NonNull;
import android.support.v7.app.AlertDialog;
import android.util.Log;

import com.example.xzy.myhome.R;
import com.gizwits.gizwifisdk.api.GizWifiSDK;
import com.gizwits.gizwifisdk.enumration.GizLogPrintLevel;

import static android.Manifest.permission.ACCESS_COARSE_LOCATION;
import static android.Manifest.permission.WRITE_EXTERNAL_STORAGE;

public class StartActivity extends BaseActivity {
    boolean isPermission;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_start);
        //检查权限
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.M) {
            Log.i(TAG, "onCreate: 检查是否拥有权限");
            boolean isPhonePermission = checkSelfPermission(Manifest.permission.READ_PHONE_STATE)
                    == PackageManager.PERMISSION_GRANTED;
            boolean isLocatinPermission = checkSelfPermission(ACCESS_COARSE_LOCATION)
                    == PackageManager.PERMISSION_GRANTED;
            boolean isStoragePermission = checkSelfPermission(WRITE_EXTERNAL_STORAGE)
                    == PackageManager.PERMISSION_GRANTED;
            isPermission = isPhonePermission && isLocatinPermission && isStoragePermission;
            if (isPhonePermission) {
                Log.i(TAG, "onCreate: 拥有权限,直接初始化");
                initSdk();
            }else {
                Log.i(TAG, "onCreate: 没权限,申请权限");
                initPermission();
            }
        }else {
            initSdk();
        }

    }


    /*//API23需授权权限
    <uses-permission android:name="android.permission.READ_PHONE_STATE" />
    <uses-permission android:name="android.permission.ACCESS_COARSE_LOCATION" />
    <uses-permission android:name="android.permission.ACCESS_FINE_LOCATION" />
    <uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE" />*/
    //6.0获取权限
    private void initPermission() {
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.M) {
            //判断是否拥有全部权限,没有申请权限
            if (!isPermission) {
                if (shouldShowRequestPermissionRationale(Manifest.permission.READ_PHONE_STATE)
                        ||shouldShowRequestPermissionRationale(Manifest.permission.ACCESS_COARSE_LOCATION)
                        ||shouldShowRequestPermissionRationale(Manifest.permission.WRITE_EXTERNAL_STORAGE)) {
                    new AlertDialog.Builder(StartActivity.this)
                            .setMessage("当前应用确少相关运行权限")
                            .setPositiveButton("给与权限", new DialogInterface.OnClickListener() {
                                @Override
                                public void onClick(DialogInterface dialogInterface, int i) {
                                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                                        requestPermissions(new String[]{Manifest.permission.READ_PHONE_STATE,
                                                Manifest.permission.ACCESS_COARSE_LOCATION,Manifest.permission.WRITE_EXTERNAL_STORAGE}, 1);
                                    }
                                }
                            })
                            .setNegativeButton("退出", new DialogInterface.OnClickListener() {
                                @Override
                                public void onClick(DialogInterface dialog, int which) {
                                    finish();
                                }
                            })
                            .create()
                            .show();

                } else {
                    //请求权限。第一个参数 是要获取的权限(可以是多个) 第二个参数用于回调时判断用
                    requestPermissions(new String[]     {Manifest.permission.READ_PHONE_STATE,
                            ACCESS_COARSE_LOCATION,WRITE_EXTERNAL_STORAGE}, 1);
                }
            }
        }

    }



    //6.0权限回调方法
    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        //requestCode之前申请权限传过来的值
        if (requestCode == 1) {
            // permissions[]为申请的权限 android.per....(不是Manifest.per.......)
            // grantResults[]为权限获取是否成功     成功返回 PackageManager.PERMISSION_GRANTED
            //                                    失败返回 PackageManager.PERMISSION_DENIED
            if ((grantResults[0]==PackageManager.PERMISSION_GRANTED)
                    &&(grantResults[1]==PackageManager.PERMISSION_GRANTED)
                    &&(grantResults[2]==PackageManager.PERMISSION_GRANTED)) {
                Log.i(TAG, "onRequestPermissionsResult: 成功申请权限");
                initSdk();
            }

        }

    }

    private void initSdk() {
        Log.i(TAG, "initSdk: 开始初始化sdk");
        GizWifiSDK.sharedInstance().setListener(mListener);
        GizWifiSDK.sharedInstance().startWithAppID(getApplicationContext(), "04d8c15be81a4fafa23ef842f8b8047b");
        GizWifiSDK.sharedInstance().setLogLevel(GizLogPrintLevel.GizLogPrintI);
        Log.i(TAG, "initSdk: 初始化成功");
        Log.i(TAG, "initSDK: 开始启动登录界面");
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
        },1000);
    }


}

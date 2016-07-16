package com.example.xzy.myhome.activity;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.example.xzy.myhome.R;
import com.gizwits.gizwifisdk.api.GizWifiDevice;
import com.gizwits.gizwifisdk.api.GizWifiSDK;
import com.gizwits.gizwifisdk.enumration.GizWifiErrorCode;
import com.gizwits.gizwifisdk.listener.GizWifiDeviceListener;
import com.gizwits.gizwifisdk.listener.GizWifiSDKListener;

import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {
    String uid;
    String token;
    List<String> pks;
    String ProductKey="db8d532e5c8f46a597e5b434a0198a66";
    String Did = "2kgR5sJHLiwtxuuifH7MiP";
    GizWifiDevice mDevice = null;
    // 使用缓存的设备列表刷新UI
    List<GizWifiDevice> devices = GizWifiSDK.sharedInstance().getDeviceList();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        GizWifiSDK.sharedInstance().setListener(mListener);

        Button buttonWifi = (Button) findViewById(R.id.button_wifi);
        Button buttonFind = (Button) findViewById(R.id.button_find);
        Button buttonSubscription = (Button) findViewById(R.id.button_subscription);
        Button buttonBoundNoLan = (Button) findViewById(R.id.button_bound_no_lan);
        Button setMassage = (Button) findViewById(R.id.set_massage);
        Button buttoneMoveBound = (Button) findViewById(R.id.button_remove_bound);
        Button buttonMassage = (Button) findViewById(R.id.button_massage);
        Button buttonDevice = (Button) findViewById(R.id.button_device);
        buttonWifi.setOnClickListener(this);
        buttonFind.setOnClickListener(this);
        buttonSubscription.setOnClickListener(this);
        buttonBoundNoLan.setOnClickListener(this);
        setMassage.setOnClickListener(this);
        buttoneMoveBound.setOnClickListener(this);
        buttonMassage.setOnClickListener(this);
        buttonDevice.setOnClickListener(this);

        Intent intent = getIntent();
        uid = intent.getStringExtra("uid");
        token = intent.getStringExtra("token");

    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.button_wifi:
                startActivity(new Intent(MainActivity.this, WIFIActivity.class));
                break;
            case R.id.button_find:
                pks = new ArrayList<String>();
                pks.add(ProductKey);   //Product Key
                GizWifiSDK.sharedInstance().getBoundDevices(uid, token, pks);
                break;
            case R.id.button_subscription:
                // 以设备列表中的第一个设备实例为例，为其设置监听
                Toast.makeText(MainActivity.this, "当前可订阅设备数："+devices.size(), Toast.LENGTH_SHORT).show();
                for (int i = 0; i < devices.size(); i++) {

                    mDevice = devices.get(0);
                    mDevice.setListener(mDeviceListener);
                    mDevice.setSubscribe(true);
                    break;
                }

                break;
            case R.id.button_bound_no_lan:
//                GizWifiSDK.sharedInstance().bindRemoteDevice (uid, token,
//                        Did,ProductKey, "123456");
                break;
            case R.id.set_massage:

                break;
            case R.id.button_remove_bound:

                break;
            case R.id.button_massage:

                break;
            case R.id.button_device:
                if (mDevice == null) {
                    Toast.makeText(MainActivity.this, "设备未订阅", Toast.LENGTH_SHORT).show();
                }else {


                }

                break;
        }
    }


    // 接收设备列表变化上报，刷新UI
    GizWifiSDKListener mListener = new GizWifiSDKListener() {
        @Override
        public void didDiscovered(GizWifiErrorCode result, List<GizWifiDevice> deviceList) {
            // 提示错误原因
            if (result != GizWifiErrorCode.GIZ_SDK_SUCCESS) {
                Log.d("", "result: " + result.name());
                Toast.makeText(MainActivity.this,"result: " + result.name(), Toast.LENGTH_SHORT).show();

            }
            // 显示变化后的设备列表
            Log.d("", "discovered deviceList: " + deviceList);
            devices = deviceList;
            Toast.makeText(MainActivity.this,"设备列表变化"+ deviceList , Toast.LENGTH_SHORT).show();

        }

        @Override
        public void didBindDevice(GizWifiErrorCode result, String did) {
            if (result == GizWifiErrorCode.GIZ_SDK_SUCCESS) {
                Toast.makeText(MainActivity.this,"绑定成功" , Toast.LENGTH_SHORT).show();

                // 绑定成功
            } else {
                Toast.makeText(MainActivity.this,"绑定失败" , Toast.LENGTH_SHORT).show();

                // 绑定失败
            }
        }


    };
    GizWifiDeviceListener mDeviceListener = new GizWifiDeviceListener() {
        @Override
        public  void didSetSubscribe(GizWifiErrorCode result, GizWifiDevice device, boolean isSubscribed) {
            if (result == GizWifiErrorCode.GIZ_SDK_SUCCESS) {
                Toast.makeText(MainActivity.this,"订阅或解除订阅成功" , Toast.LENGTH_SHORT).show();

                // 订阅或解除订阅成功
            } else {
                // 失败
                Toast.makeText(MainActivity.this,"订阅或解除订阅失败" , Toast.LENGTH_SHORT).show();


            }
        }
    };



}

package com.example.xzy.myhome.activity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.example.xzy.myhome.R;
import com.gizwits.gizwifisdk.api.GizWifiDevice;
import com.gizwits.gizwifisdk.api.GizWifiSDK;
import com.gizwits.gizwifisdk.enumration.GizWifiErrorCode;

import java.util.ArrayList;
import java.util.List;

public class MainActivity extends BaseActivity implements View.OnClickListener {
    String uid;
    String token;
    List<String> pks;
    String ProductKey="c39bb944b86f4b55a51e74cdf11b1286";
    String Did = "FD5NHUDiroQ7XZycWMJRuV";
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
                GizWifiSDK.sharedInstance().bindRemoteDevice (uid, token,
                        Did,"123456", null);
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
                    Intent intent = new Intent(MainActivity.this, DeviceActivity.class);
                    Bundle bundle = new Bundle();
                    bundle.putParcelable("mDevice", mDevice);
                    intent.putExtras(bundle);
                    startActivity(intent);
                }

                break;
        }
    }


    @Override
    public void MdidDiscovered(GizWifiErrorCode result, List<GizWifiDevice> deviceList) {
        // 提示错误原因
        if (result != GizWifiErrorCode.GIZ_SDK_SUCCESS) {
            Log.d("", "result: " + result.name());
            Toast.makeText(MainActivity.this,"result: " + result.name(), Toast.LENGTH_SHORT).show();

        }
        // 显示变化后的设备列表
        Log.d("", "discovered deviceList: " + deviceList);
        devices = deviceList;
        if (devices != null) {
            Toast.makeText(MainActivity.this, "没有可用设备" , Toast.LENGTH_SHORT).show();
        }else {
            Toast.makeText(MainActivity.this, "设备列表变化" + devices, Toast.LENGTH_SHORT).show();
        }
    }



}

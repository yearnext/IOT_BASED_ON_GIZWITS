package com.example.xzy.myhome.activity;

import android.graphics.Color;
import android.os.Bundle;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;

import com.example.xzy.myhome.Config;
import com.example.xzy.myhome.R;
import com.gizwits.gizwifisdk.api.GizWifiDevice;
import com.gizwits.gizwifisdk.api.GizWifiSDK;

import java.util.ArrayList;
import java.util.List;

import butterknife.BindView;
import butterknife.ButterKnife;

public class MainActivity extends BaseActivity {
    List<String> pks;
    GizWifiDevice mDevice = null;
    @BindView(R.id.tb_device_list)
    Toolbar tbDeviceList;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        ButterKnife.bind(this);
        tbDeviceList.setLogo(R.drawable.ic_storage_black_24dp);
        tbDeviceList.setTitle(R.string.toolbar);
        tbDeviceList.setTitleTextColor(Color.WHITE);
        tbDeviceList.inflateMenu(R.menu.devicelist_toolbar_menu);
        tbDeviceList.setOnMenuItemClickListener(new Toolbar.OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(MenuItem item) {
                int itemId = item.getItemId();
                if (itemId == R.id.add_item) {

                } else if (itemId == R.id.delete_item){

                } else if (itemId == R.id.refresh_item) {
                    pks = new ArrayList<String>();
                    pks.add(Config.PRODUCT_KEY);   //Product Key
                    GizWifiSDK.sharedInstance().getBoundDevices(mUid, mToken, pks);
                }
                return true;
            }
        });
    }


   /* public void onClick(View view) {
        switch (view.getId()) {
            case R.id.button_wifi:
                startActivity(new Intent(MainActivity.this, WIFIActivity.class));
                break;
            case R.id.button_find:
                pks = new ArrayList<String>();
                pks.add(Config.PRODUCT_KEY);   //Product Key
                GizWifiSDK.sharedInstance().getBoundDevices(mUid, mToken, pks);
                break;
            case R.id.button_subscription:
                // 以设备列表中的第一个设备实例为例，为其设置监听
                Toast.makeText(MainActivity.this, "当前可订阅设备数：" + devices.size(), Toast.LENGTH_SHORT).show();
                for (int i = 0; i < devices.size(); i++) {
                    mDevice = devices.get(0);
                    mDevice.setListener(mDeviceListener);
                    mDevice.setSubscribe(true);
                    break;
                }

                break;
            case R.id.button_bound_no_lan:
                mDevice = devices.get(0);
                break;
            case R.id.set_massage:
                break;
            case R.id.button_remove_bound:
                GizWifiSDK.sharedInstance().setListener(mListener);
                GizWifiSDK.sharedInstance().unbindDevice(mUid, mToken, mDevice.getDid());
                Toast.makeText(MainActivity.this, "当前可订阅设备数：" + devices.size(), Toast.LENGTH_SHORT).show();

                break;
            case R.id.button_massage:

                break;
            case R.id.tb_device_list:
                if (mDevice == null) {
                    Toast.makeText(MainActivity.this, "设备未订阅", Toast.LENGTH_SHORT).show();
                } else {
                    Intent intent = new Intent(MainActivity.this, DeviceActivity.class);
                    Bundle bundle = new Bundle();
                    bundle.putParcelable("mDevice", mDevice);
                    intent.putExtras(bundle);
                    startActivity(intent);
                }

                break;
        }
    }*/


}

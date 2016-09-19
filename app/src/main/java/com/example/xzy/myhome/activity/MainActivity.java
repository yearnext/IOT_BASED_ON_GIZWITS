package com.example.xzy.myhome.activity;

import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.support.design.widget.Snackbar;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;

import com.example.xzy.myhome.Config;
import com.example.xzy.myhome.R;
import com.example.xzy.myhome.util.ToastUtil;
import com.gizwits.gizwifisdk.api.GizWifiDevice;
import com.gizwits.gizwifisdk.api.GizWifiSDK;
import com.gizwits.gizwifisdk.enumration.GizWifiErrorCode;

import java.util.ArrayList;
import java.util.List;

import butterknife.BindView;
import butterknife.ButterKnife;

public class MainActivity extends BaseActivity {
    GizWifiDevice mDevice = null;
    @BindView(R.id.tb_device_list)
    Toolbar tbDeviceList;
    @BindView(R.id.lv_device_list)
    ListView lvDeviceList;
    ArrayAdapter<String> arrayAdapter;
    String[] data = new String[]{"不要点!","不要点!","不要点!","不要点!","不要点!","不要点!","不要点!","不要点!"};
    List<GizWifiDevice> devices = GizWifiSDK.sharedInstance().getDeviceList();

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        ButterKnife.bind(this);

        Intent intent = getIntent();
        String  mUid = intent.getStringExtra("mUid");
        String mToken = intent.getStringExtra("mToken");

        // 主动刷新绑定设备列表、指定筛选的设备productKey
        List<String> pks = new ArrayList<String> ();
        pks.add(Config.PRODUCT_KEY);
        GizWifiSDK.sharedInstance().getBoundDevices(mUid, mToken, pks);
        if (devices.size() != 0) {
            data = new String[devices.size()];
            for (int i = 0; i < devices.size(); i++) {
                String aDevice = devices.get(i).getDid();
                data[i] = aDevice;
            }
        }

        //toolbarList
        tbDeviceList.setLogo(R.drawable.ic_storage_black_24dp);
        tbDeviceList.setTitle(R.string.toolbar);
        tbDeviceList.setTitleTextColor(Color.WHITE);
        tbDeviceList.inflateMenu(R.menu.devicelist_toolbar_menu);
        tbDeviceList.setOnMenuItemClickListener(new Toolbar.OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(MenuItem item) {
                int itemId = item.getItemId();
                switch (itemId) {
                    case R.id.add_item:
                        Intent intent = new Intent(MainActivity.this, WIFIActivity.class);
                        startActivity(intent);
                        break;

                    case R.id.refresh_item:
                        ToastUtil.showToast(MainActivity.this,mDevice.getNetStatus().toString());

                        break;

                    case R.id.delete_item:

                        break;
                }
                return true;

            }
        });

        //listView
        arrayAdapter = new ArrayAdapter<String>(MainActivity.this,
                android.R.layout.simple_list_item_1, data);
        lvDeviceList.setAdapter(arrayAdapter);
        lvDeviceList.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                mDevice = devices.get(i);
                mDevice.setListener(mDeviceListener);
                mDevice.setSubscribe(true);
                Intent intent = new Intent(MainActivity.this, DeviceActivity.class);
                Bundle bundle = new Bundle();
                bundle.putParcelable("mDevice", mDevice);
                intent.putExtras(bundle);
                startActivity(intent);
            }
        });
        data[0] = "nin";
        arrayAdapter.notifyDataSetChanged();
    }

    @Override
    protected void mDidDiscovered(GizWifiErrorCode result, List<GizWifiDevice> deviceList) {
        // 提示错误原因
        if(result != GizWifiErrorCode.GIZ_SDK_SUCCESS) {
            Log.d("", "result: " + result.name());
        }
        // 显示变化后的设备列表
        Log.d("", "discovered deviceList: " + deviceList);
        devices= deviceList;

        Log.i(TAG, "设备数目:" + devices.size() + "  数组长度:" + data.length);
        if (devices.size() != data.length) {
            for (int i = 0; i < devices.size(); i++) {
                String mDid = devices.get(i).getDid();
                Log.i(TAG, "data值: data=" + mDid);
                data[i] = mDid;
            }
            arrayAdapter.notifyDataSetChanged();
            Snackbar.make(lvDeviceList, "列表发生变化", Snackbar.LENGTH_SHORT).show();
        }
    }
}

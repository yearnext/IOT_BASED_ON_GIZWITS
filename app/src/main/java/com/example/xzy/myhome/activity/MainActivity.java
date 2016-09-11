package com.example.xzy.myhome.activity;

import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
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

import butterknife.BindView;
import butterknife.ButterKnife;

public class MainActivity extends BaseActivity {
    GizWifiDevice mDevice = null;
    @BindView(R.id.tb_device_list)
    Toolbar tbDeviceList;
    @BindView(R.id.lv_device_list)
    ListView lvDeviceList;
    ArrayAdapter<String> arrayAdapter;
    String[] data=new String[5];

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        ButterKnife.bind(this);
        Intent intent = getIntent();
        mDevice = intent.getParcelableExtra("mDevice");
        mDid = intent.getStringExtra("mDid");
        mToken = intent.getStringExtra("mToken");
        if (mDevice != null) {
            mDid = mDevice.getDid();
            data = new String[]{mDid};

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
                        if (pks==null){
                            pks = new ArrayList<String>();
                            pks.add(Config.PRODUCT_KEY);   //Product Key
                            ToastUtil.showToast(MainActivity.this,"pak null");
                        }

                        GizWifiSDK.sharedInstance().getBoundDevices(mUid, mToken, pks);
                        break;

                    case R.id.delete_item:
                        GizWifiSDK.sharedInstance().unbindDevice(mUid, mToken, mDid);
                        break;

                }
                return true;

            }
        });

        //listView
        if (data==null) data = new String[]{};
        arrayAdapter = new ArrayAdapter<String>(MainActivity.this,
                android.R.layout.simple_list_item_1,data);
        lvDeviceList.setAdapter(arrayAdapter);
        lvDeviceList.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                GizWifiDevice mDevice = devices.get(1);
                mDevice.setListener(mDeviceListener);
                mDevice.setSubscribe(true);
                Intent intent = new Intent(MainActivity.this, DeviceActivity.class);
                Bundle bundle = new Bundle();
                bundle.putParcelable("mDevice",mDevice);
                intent.putExtras(bundle);
                startActivity(intent);
            }
        });
    }

    @Override
    protected void mDidDiscovered(GizWifiErrorCode result) {
        //Snackbar.make(tbDeviceList, "设备数量:" + devices.size(), Snackbar.LENGTH_LONG).show();
        //StringBuffer stringBuffer = new StringBuffer();
        Log.e(TAG, "mDidDiscovered: "+devices.size());
        if (devices.size() != 0) {
            for (int i1=0;i1 < devices.size();i1++){
                String aDevice = devices.get(i1).getDid();
                data[i1] = aDevice;
                //stringBuffer.append(aDevice);
            }
            arrayAdapter.notifyDataSetChanged();
        }
    }



}

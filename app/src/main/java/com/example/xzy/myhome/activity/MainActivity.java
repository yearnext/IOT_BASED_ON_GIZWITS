package com.example.xzy.myhome.activity;

import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.support.design.widget.Snackbar;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.widget.TextView;

import com.example.xzy.myhome.Config;
import com.example.xzy.myhome.R;
import com.example.xzy.myhome.adapter.IotRecyckerViewAdapter;
import com.example.xzy.myhome.bean.IotDevice;
import com.example.xzy.myhome.util.BooleanTranslateUtil;
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
    @BindView(R.id.rv_device_list)
    RecyclerView rvDeviceList;
    @BindView(R.id.empty_list)
    TextView emptyList;

    List<GizWifiDevice> devices = GizWifiSDK.sharedInstance().getDeviceList();
    String mUid;
    String mToken;
    List<IotDevice> iotDeviceList;
    IotRecyckerViewAdapter iotAdapter;



    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        ButterKnife.bind(this);
        Intent intent = getIntent();
        mUid = intent.getStringExtra("mUid");
        mToken = intent.getStringExtra("mToken");
        initView();
        // 主动刷新绑定设备列表、指定筛选的设备productKey
        List<String> pks = new ArrayList<String>();
        pks.add(Config.PRODUCT_KEY);
        GizWifiSDK.sharedInstance().getBoundDevices(mUid, mToken, pks);

    }

    private void initView() {

        //toolbarList
        //tbDeviceList.setLogo(R.drawable.ic_storage_black_24dp);
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
                        if (mDevice != null) {
                            ToastUtil.showToast(MainActivity.this, mDevice.getNetStatus().toString());
                        }

                        break;

                    case R.id.delete_item:
                        if (devices.size()!= 0) {
                            GizWifiSDK.sharedInstance().unbindDevice(mUid, mToken, devices.get(0).getDid());
                        }

                        break;
                }
                return true;

            }
        });

    }

    private void initData() {
        if (devices.size() != 0) {
            emptyList.setVisibility(View.GONE);
            rvDeviceList.setVisibility(View.VISIBLE);
            iotDeviceList = new ArrayList<IotDevice>();

            for (GizWifiDevice device : devices) {
                IotDevice iotDevice = new IotDevice();
                iotDevice
                        .setBind(BooleanTranslateUtil.translateString(device.isBind()))
                        .setDid(device.getDid())
                        .setDisable(BooleanTranslateUtil.translateString(device.isDisabled()))
                        .setLan(BooleanTranslateUtil.translateString(device.isLAN()))
                        .setMac(device.getMacAddress())
                        .setNetStatus(device.getNetStatus().toString())
                        .setSubscribed(BooleanTranslateUtil.translateString(device.isSubscribed()));
                iotDeviceList.add(iotDevice);
            }
        } else {
            emptyList.setVisibility(View.VISIBLE);
            rvDeviceList.setVisibility(View.GONE);
        }

    }


    private void initRecyclerView() {
        //recyclerView
        iotAdapter = new IotRecyckerViewAdapter(iotDeviceList);
        LinearLayoutManager linearLayoutManager = new LinearLayoutManager(this);
        rvDeviceList.setLayoutManager(linearLayoutManager);
        rvDeviceList.setAdapter(iotAdapter);
        iotAdapter.setOnItemClickLitener(new IotRecyckerViewAdapter.OnItemClickLitener() {
            @Override
            public void onItemClick(View view, int position) {
                mDevice = devices.get(position);
                mDevice.setListener(mDeviceListener);
                mDevice.setSubscribe(true);
                Intent intent = new Intent(MainActivity.this, DeviceActivity.class);
                Bundle bundle = new Bundle();
                bundle.putParcelable("mDevice", mDevice);
                intent.putExtras(bundle);
                startActivity(intent);
            }
        });
    }


    @Override
    protected void mDidDiscovered(GizWifiErrorCode result, List<GizWifiDevice> deviceList) {
        // 提示错误原因
        if (result != GizWifiErrorCode.GIZ_SDK_SUCCESS) {
            Log.d("", "result: " + result.name());
        }
        // 显示变化后的设备列表
        Log.d("", "discovered deviceList: " + deviceList);
        devices = deviceList;
        initData();
        initRecyclerView();
        Snackbar.make(rvDeviceList, "列表发生变化", Snackbar.LENGTH_SHORT).show();

    }
}


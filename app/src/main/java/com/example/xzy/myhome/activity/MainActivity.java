package com.example.xzy.myhome.activity;

import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AlertDialog;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.widget.TextView;

import com.example.xzy.myhome.R;
import com.example.xzy.myhome.Setting;
import com.example.xzy.myhome.adapter.IotRecyclerAdapter;
import com.example.xzy.myhome.model.bean.IotDevice;
import com.example.xzy.myhome.util.BooleanTranslateUtil;
import com.example.xzy.myhome.util.ToastUtil;
import com.gizwits.gizwifisdk.api.GizWifiDevice;
import com.gizwits.gizwifisdk.api.GizWifiSDK;
import com.gizwits.gizwifisdk.enumration.GizWifiDeviceNetStatus;
import com.gizwits.gizwifisdk.enumration.GizWifiErrorCode;

import java.util.ArrayList;
import java.util.List;

import butterknife.BindView;
import butterknife.ButterKnife;

public class MainActivity extends BaseActivity {
    GizWifiDevice mGizDevice = null;
    @BindView(R.id.tb_device_list)
    Toolbar tbDeviceList;
    @BindView(R.id.rv_device_list)
    RecyclerView rvDeviceList;
    @BindView(R.id.empty_list)
    TextView emptyList;

    private String mUid;
    private String mToken;
    private List<IotDevice> iotDeviceList= new ArrayList<>();
    private IotRecyclerAdapter mIotAdapter;

    List<GizWifiDevice> mGizDevicesList = GizWifiSDK.sharedInstance().getDeviceList();

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        ButterKnife.bind(this);
        Intent intent = getIntent();
        mUid = intent.getStringExtra("mUid");
        mToken = intent.getStringExtra("mToken");
        List<String> pks = new ArrayList<>();
        pks.add(getApplication().getString(R.string.product_key));
        GizWifiSDK.sharedInstance().getBoundDevices(mUid, mToken, pks);
        initToolbar();
        initRecyclerView();
    }


    private void initRecyclerView() {
        mIotAdapter = new IotRecyclerAdapter(iotDeviceList);
        LinearLayoutManager linearLayoutManager = new LinearLayoutManager(this);
        rvDeviceList.setLayoutManager(linearLayoutManager);
        rvDeviceList.setAdapter(mIotAdapter);
        mIotAdapter.setOnItemClickLitener(new IotRecyclerAdapter.OnItemClickLitener() {
            @Override
            public void onItemClick(View view, int position) {
                mGizDevice = mGizDevicesList.get(position);
                if (mGizDevice.getNetStatus() != GizWifiDeviceNetStatus.GizDeviceOffline|| Setting.TEST) {
                    mGizDevice.setListener(mDeviceListener);
                    mGizDevice.setSubscribe(true);
                    Intent intent = new Intent(MainActivity.this, Main2Activity.class);
                    intent.putExtra("gizDevice", mGizDevice);
                    startActivityForResult(intent, 0);
                    finish();
                } else {
                    ToastUtil.showToast(MainActivity.this, "当前设备处于离线状态");
                }

            }
        });
    }

    private void initToolbar() {
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
                        refresh();
                        break;
                    case R.id.delete_item:
                        if (mGizDevicesList.size() != 0) {
                            GizWifiSDK.sharedInstance().unbindDevice(mUid, mToken, mGizDevicesList.get(0).getDid());
                        }
                        break;
                }
                return true;

            }
        });
    }

    protected void refresh() {
        Log.i(TAG, "refresh: " + mGizDevicesList.size());
        if (mGizDevicesList.size() != 0 || iotDeviceList != null) {
            iotDeviceList.removeAll(iotDeviceList);
            for (GizWifiDevice device : mGizDevicesList) {
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
                Log.i(TAG, "列表数量" + iotDeviceList.size());
                mIotAdapter.notifyDataSetChanged();
            }
            emptyList.setVisibility(View.GONE);
            rvDeviceList.setVisibility(View.VISIBLE);
        }
    }

    @Override
    protected void mDidDiscovered(GizWifiErrorCode result, List<GizWifiDevice> deviceList) {
        if (result != GizWifiErrorCode.GIZ_SDK_SUCCESS) {
            Log.e("", "result: " + result.name());
        }
        mGizDevicesList = deviceList;
        refresh();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == RESULT_OK) {
            ToastUtil.showToast(MainActivity.this, "云端设备未响应，请稍后再试");
            Snackbar.make(tbDeviceList, "云端设备未响应", Snackbar.LENGTH_LONG)
                    .setAction("查看帮助", new View.OnClickListener() {
                        @Override
                        public void onClick(View v) {
                            showHelp();
                        }
                    }).show();
        }
    }

    private void showHelp() {
        AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
        builder.setTitle("云端设备未响应解决方法")
                .setMessage("1 检查WIFI网络状况" + "\n" +
                        "2 确认网关设备已成功连接网络" + "\n" +
                        "3 尝试再次连接")
                .setPositiveButton("再次连接", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        mGizDevice.setListener(mDeviceListener);
                        mGizDevice.setSubscribe(true);
                        Intent intent = new Intent(MainActivity.this, Main2Activity.class);
                        Bundle bundle = new Bundle();
                        bundle.putParcelable("sGizDevice", mGizDevice);
                        intent.putExtras(bundle);
                        startActivityForResult(intent, 0);
                    }
                })
                .show();
    }
}


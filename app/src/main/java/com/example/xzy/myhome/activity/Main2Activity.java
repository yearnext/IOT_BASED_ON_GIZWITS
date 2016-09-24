package com.example.xzy.myhome.activity;

import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AlertDialog;
import android.support.v7.widget.PopupMenu;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.MenuItem;
import android.view.Window;
import android.widget.ImageButton;
import android.widget.TimePicker;

import com.example.xzy.myhome.R;
import com.example.xzy.myhome.util.ToastUtil;
import com.gizwits.gizwifisdk.api.GizWifiDevice;
import com.gizwits.gizwifisdk.enumration.GizWifiErrorCode;

import java.util.concurrent.ConcurrentHashMap;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;

import static com.mxchip.helper.ProbeReqData.bytesToHex;

public class Main2Activity extends BaseActivity {
    GizWifiDevice mDevice;
    @BindView(R.id.tb_device_list1)
    Toolbar tbDeviceList;
    @BindView(R.id.button_lamp_set)
    ImageButton buttonLampSet;

    @Override
    public void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main2);
        ButterKnife.bind(this);
        initView();
        Intent intent = getIntent();


        //mDevice = intent.getParcelableExtra("mDevice");
        //mDevice.setListener(mDeviceListener);
        //mDevice.getDeviceStatus();
    }

    private void initView() {

        //toolbarList

        tbDeviceList.setTitle(R.string.toolbar1);
        tbDeviceList.setTitleTextColor(Color.WHITE);
        tbDeviceList.inflateMenu(R.menu.devicelist_toolbar_menu);
        tbDeviceList.setOnMenuItemClickListener(new Toolbar.OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(MenuItem item) {
                int itemId = item.getItemId();
                switch (itemId) {
                    case R.id.add_item:

                        break;

                    case R.id.refresh_item:
                        if (mDevice != null) {

                        }

                        break;

                    case R.id.delete_item:


                        break;
                }
                return true;

            }
        });

    }

    // 实现回调
    @Override
    public void mDidReceiveData(GizWifiErrorCode result, GizWifiDevice device, ConcurrentHashMap<String, Object> dataMap, int sn) {
        if (result == GizWifiErrorCode.GIZ_SDK_SUCCESS) {
            Log.i(TAG, "MdidReceiveData: 接收到云端数据");
            // 普通数据点类型，有布尔型、整形和枚举型数据，该种类型一般为可读写
            if (dataMap.get("data") != null) {
                ConcurrentHashMap<String, Object> map = (ConcurrentHashMap<String, Object>) dataMap.get("data");
                byte[] bytes = (byte[]) map.get("Packet");
                if (bytes == null) Log.e(TAG, "MdidReceiveData: " + "bytes为空");
                else {
                    for (byte a : bytes) {
                        Log.e(TAG, "MdidReceiveData: " + a);
                    }
                    String string = bytes.toString();
                    Log.e(TAG, "mDidReceiveData: " + string);
                }
            }
            //无定义数据
            if (dataMap.get("binary") != null) {
                byte[] binary = (byte[]) dataMap.get("binary");
                Log.e(TAG, "无定义数据 Binary data:"
                        + bytesToHex(binary));
            }
        } else {
            if (result == GizWifiErrorCode.GIZ_SDK_DEVICE_NOT_READY) {
                ToastUtil.showToast(Main2Activity.this, "设备处于未就绪状态");
            } else {
                Log.e(TAG, "MdidReceiveData: 数据点回调失败 result=" + result);
            }
        }


    }

    @OnClick(R.id.button_lamp_set)
    public void onClick() {
        PopupMenu popupMenu = new PopupMenu(Main2Activity.this,buttonLampSet);
        popupMenu.getMenuInflater().inflate(R.menu.curtain_set_menu, popupMenu.getMenu());
        popupMenu.setOnMenuItemClickListener(new PopupMenu.OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(MenuItem item) {
                int id = item.getItemId();
                switch (id) {
                    case R.id.time_set:
                        Intent i = new Intent(Main2Activity.this, AlarmActivity.class);
                       startActivity(i);
                        break;
                    case R.id.countdown_set:
                        final AlertDialog.Builder builder = new AlertDialog.Builder(Main2Activity.this);
                        builder.setItems(R.array.countdown_data, new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                                if (which == 5) {
                                    AlertDialog.Builder builder1 = new AlertDialog.Builder(Main2Activity.this,R.style.AlertDialog_AppCompat_mTime);
                                    AlertDialog alertDialog = builder1.setView(R.layout.custom_countdown_layout)
                                           .setPositiveButton("确定", new DialogInterface.OnClickListener() {
                                               @Override
                                               public void onClick(DialogInterface dialog, int which) {
                                                   ToastUtil.showToast(Main2Activity.this, "0k");
                                               }
                                           })
                                            .setNegativeButton("取消", new DialogInterface.OnClickListener() {
                                                @Override
                                                public void onClick(DialogInterface dialog, int which) {
                                                    ToastUtil.showToast(Main2Activity.this,"No");
                                                }
                                            }).setMessage("倒计时时间")
                                            .show();
                                    Window window = alertDialog.getWindow();
                                    TimePicker timePicker = (TimePicker) window.findViewById(R.id.timePicker);
                                    timePicker.setIs24HourView(true);
                                } else {

                                }
                            }
                        }).show();

                        break;
                    case R.id.humidity_set:
                        Snackbar.make(buttonLampSet, "humidity", Snackbar.LENGTH_SHORT).show();


                        break;
                }
                return true;
            }
        });
        popupMenu.show();
    }
}

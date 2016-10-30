package com.example.xzy.myhome.activity;

import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.support.v7.app.AlertDialog;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.Window;
import android.widget.TextView;
import android.widget.TimePicker;

import com.example.xzy.myhome.Config;
import com.example.xzy.myhome.R;
import com.example.xzy.myhome.adapter.DeviceItemRecycerViewAdapter;
import com.example.xzy.myhome.bean.Device;
import com.example.xzy.myhome.util.ParsePacket;
import com.example.xzy.myhome.util.ToastUtil;
import com.gizwits.gizwifisdk.api.GizWifiDevice;
import com.gizwits.gizwifisdk.enumration.GizWifiErrorCode;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;

import static com.example.xzy.myhome.util.ParsePacket.COMMAND.UPDATE_DEVICE_COUNT;
import static com.mxchip.helper.ProbeReqData.bytesToHex;

public class Main2Activity extends BaseActivity {

    private static final int ON = 0;
    private static final int OFF = 1;
    private static  String MAC;
    @BindView(R.id.tb_device_list1)
    Toolbar tbDeviceList;

    @BindView(R.id.text_view_logcat)
    TextView textViewLogcat;

    @BindView(R.id.rv_device_item)
    RecyclerView rvDeviceItem;


    GizWifiDevice mDevice;
    byte eventNumber = 0;
    String logcat;
    int i = 0;
    //private byte[] emptyacket = new byte[]{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    private String  emptyacket= "0000000000000000000000000000000000000000000000000000000000000000";
    List<Device> deviceList;
    DeviceItemRecycerViewAdapter deviceRVAdapter;
    static ConcurrentHashMap<String, Object> dataMap = new ConcurrentHashMap<String, Object>();
    byte count = 0;


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main2);
        ButterKnife.bind(this);
        Intent intent = getIntent();
        mDevice = intent.getParcelableExtra("mDevice");
        mDevice.setListener(mDeviceListener);
        mDevice.getDeviceStatus();
        deviceList = new ArrayList<Device>();
        initView();
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


        //RecyclerView
        deviceRVAdapter = new DeviceItemRecycerViewAdapter(deviceList);
        deviceRVAdapter.setDeviceSetListener(new DeviceItemRecycerViewAdapter.DeviceSetListener() {
            @Override
            public void onCountdownClickOn(int position) {
                final ParsePacket parsePacket = new ParsePacket();
                parsePacket.setDataTimeState((byte) 255);
                //// TODO: 2016/10/30 lamp state
                showCountdownDialog(position,parsePacket,ON);
            }

            @Override
            public void onCountdownClickOff(int position) {
                final ParsePacket parsePacket = new ParsePacket();
                parsePacket.setDataTimeState((byte) 0);
                showCountdownDialog(position,parsePacket,OFF);

            }

            @Override
            public void onNameClick(int position, View view) {
                //// TODO: 2016/10/26 名字修改 

            }

            @Override
            public void onSwtichClick(int position, View view, boolean switchState) {
                byte switchStateB;
                if (switchState) {
                    switchStateB = (byte) 255;
                } else {
                    switchStateB = 0;
                }
                Log.i(TAG, "点击开关，开关状态准备变成：" + switchState);
                ParsePacket parsePacket = new ParsePacket();
                parsePacket.setMac(deviceList.get(position).getMac())
                        .setDeviceType(deviceList.get(position).getDeviceType())
                        .setType(ParsePacket.TYPE.APP_REQUEST)
                        .setCommand(ParsePacket.COMMAND.STATE_WRITE)
                        .setDataState(switchStateB)
                        .setDataLength((byte) 1)
                        .sendPacket(mDevice);
            }
        });
        LinearLayoutManager linearLayoutManager = new LinearLayoutManager(this);
        rvDeviceItem.setLayoutManager(linearLayoutManager);
        rvDeviceItem.setAdapter(deviceRVAdapter);

    }

    @Override
    public void mDidReceiveData(GizWifiErrorCode result, GizWifiDevice device, ConcurrentHashMap<String, Object> dataMap, int sn) {
        if (result == GizWifiErrorCode.GIZ_SDK_SUCCESS) {
            Log.d(TAG, "mDidReceiveData:接收到云端数据");
            if (dataMap.get("data") != null) {
                Log.d(TAG, "mDidReceiveData:有data");
                ConcurrentHashMap<String, Object> map = (ConcurrentHashMap<String, Object>) dataMap.get("data");
                byte[] bytes = (byte[]) map.get(Config.DATA_NAME);
                if (bytes == null ) {
                    Log.i(TAG, "mDidReceiveData: " + "bytes为空");
                } else if (bytesToHex(bytes).equals(emptyacket)) {
                    Log.d(TAG, "mDidReceiveData: " + "全为0数据一条");
                } else {
                    updateLogcat(bytes);
                    assessDataType(bytes);
                }
            } else {
                logcat = logcat + "第" + i + "次:数据型数据点为空" + "\n";
                Log.i(TAG, "第" + i++ + "次:数据型数据点为空");
            }
            textViewLogcat.setText(logcat);
            //无定义数据
            if (dataMap.get("binary") != null) {
                byte[] binary = (byte[]) dataMap.get("binary");
                Log.i(TAG, "无定义数据 Binary data:" + bytesToHex(binary));
            }
        }


        if (result == GizWifiErrorCode.GIZ_SDK_DEVICE_NOT_READY) {
            ToastUtil.showToast(Main2Activity.this, "设备处于未就绪状态");
        }
    }


    private void updateLogcat(byte[] data) {
        int aa = 0;
        String a = bytesToHex(data);
        //// TODO: 2016/10/29 以后用 StringBuff 优化
        StringBuffer s = new StringBuffer();
        Log.i(TAG, "原始数据包"+bytesToHex(data));
        Log.w(TAG, "第" + i + "次:" +
                "通讯类型：=" + a.substring(0, 2) + "；" +
                "事件序号：=" + a.substring(2, 4) + "；" +
                "设备类型：=" + a.substring(4, 6) + "；" +
                "mac：=" + a.substring(6, 23) + "；" +
                "命令：=" + a.substring(22, 24) + "；" +
                "数据长度：=" + a.substring(24, 26) + "；" +
                "数据：=" + a.substring(26, 61) + "；" +
                "校验和：=" + a.substring(62, 64));
        //logcat = logcat + "第" + i++ + "次:" + a + "\n";
        logcat = logcat + "第" + i++ + "次:" +
                "通讯类型：=" + a.substring(0, 2) + "；" + "\n" +
                "事件序号：=" + a.substring(2, 4) + "；" + "\n" +
                "设备类型：=" + a.substring(4, 6) + "；" + "\n" +
                "mac：=" + a.substring(6, 23) + "；" + "\n" +
                "命令：=" + a.substring(22, 24) + "；" + "\n" +
                "数据长度：=" + a.substring(24, 26) + "；" + "\n" +
                "数据：=" + a.substring(26, 61) + "；" + "\n" +
                "校验和：=" + a.substring(62, 64)
                + "\n";

    }

    private void assessDataType(byte[] b) {
        //// TODO: 2016/10/25 接收数据解析 
        ParsePacket parsePacket = new ParsePacket(b);
        if (parsePacket.getType() == ParsePacket.TYPE.DEVICE_REQUEST||
                parsePacket.getType() == ParsePacket.TYPE.DEVICE_RESPONSE) {
            processReceiveData(parsePacket);

       /* } else if (parsePacket.getType() == ParsePacket.TYPE.DEVICE_RESPONSE) {
            Log.i(TAG, "收到设备回应");*/
        } else {
            Log.e(TAG, "assessDataType:通讯代号错误：" + parsePacket.getType());
        }
    }

    private void processReceiveData(ParsePacket parsePacket) {
        if (parsePacket.getDeviceType() == ParsePacket.DEVICE_TYPE.GATEWAY) {
            Log.i(TAG, "processingData: 网关");
            updateDeviceList(parsePacket);
        } else {
            switch (parsePacket.getCommand()) {
                case ParsePacket.COMMAND.STATE_WRITE:
                    int index = getListIndex(parsePacket.getMac());
                    deviceList.get(index).setSwitchState(parsePacket.getDataState());
                    Log.i(TAG, "接收到灯数据"+deviceList.get(index)+"    "+parsePacket.getDataState());
                    deviceRVAdapter.notifyDataSetChanged();
                    break;
                case ParsePacket.COMMAND.TIMING_WRITE:

                    break;
                case ParsePacket.COMMAND.COUNTDOWN_WRITE:

                    break;
            }
        }

    }

    private int getListIndex(byte[] mac) {
        for (int a = 0;a<deviceList.size(); a++) {

            if (deviceList.get(a).getMac().equals(mac))
                return a;

        }
        return -1;
    }

    private void updateDeviceList(ParsePacket parsePacket) {
        //// TODO: 控制命令
        //请求设备数量
        if (parsePacket.getCommand() == ParsePacket.COMMAND.UPDATE_DEVICE_COUNT||
                parsePacket.getCommand() == ParsePacket.COMMAND.DEVICE_RESPONSE_APP_COUNT) {
            /*//回应设备（暂时没有意义）
            ParsePacket mParsePacket = new ParsePacket();
            mParsePacket.setType(ParsePacket.TYPE.APP_RESPONSE);
            mParsePacket.sendPacket(mDevice);*/

            count = 0;
            byte[] data = parsePacket.getData();
            count = data[0];
            Log.i(TAG, "updateDeviceList: 接收到设备列表更新，有" + count + "台设备");
            deviceList.removeAll(deviceList);
            for (byte i = 0; i < count; i++) {
                new ParsePacket().requestDeviceList(mDevice, i);
            }
            //获取设备信息
        } else if (parsePacket.getCommand() == ParsePacket.COMMAND.UPDATE_DEVICE_MESSAGE) {
            Device device= new Device();
            int index=getListIndex(parsePacket.getMac());
            Log.w(TAG, "index:"+index );
            if (index != -1) {
                device = deviceList.get(index);
                device.setMac(parsePacket.getDataMac());
                device.setDeviceType(parsePacket.getDataDeviceType());
            } else {
                device.setMac(parsePacket.getDataMac());
                device.setDeviceType(parsePacket.getDataDeviceType());
                deviceList.add(device);
            }


            Log.i(TAG, "获取到的设备MAC：" + bytesToHex(parsePacket.getDataMac()) +
                    "类型" + parsePacket.getDataDeviceType());

                deviceRVAdapter.notifyDataSetChanged();

            /*if (MAC != null) {
                Log.i(TAG,"保存的MAC"+MAC+"\n"+
                        "当前的mac"+bytesToHex(parsePacket.getDataMac()));
            }
            if (MAC == null || !MAC .equals( bytesToHex(parsePacket.getDataMac()))) {
                MAC = bytesToHex(parsePacket.getDataMac());
                device.setMac(parsePacket.getDataMac());
                device.setDeviceType(parsePacket.getDataDeviceType());
                Log.i(TAG, "获取到的设备MAC：" + bytesToHex(parsePacket.getDataMac()) +
                        "类型" + parsePacket.getDataDeviceType());
                deviceList.add(device);
                if (count == parsePacket.getDataDeviceCount()) {
                    deviceRVAdapter.notifyDataSetChanged();
                }
            } else {
                Log.e(TAG, "updateDeviceList: "+MAC);
            }*/




        }
    }


    private void showCountdownDialog(int position, final ParsePacket parsePacket,int state) {

        Log.e(TAG, "position "+position);
        Device device = deviceList.get(position);
        parsePacket.setDeviceType(device.getDeviceType());
        parsePacket.setMac(device.getMac());

        final AlertDialog.Builder builder = new AlertDialog.Builder(Main2Activity.this);
        DialogInterface.OnClickListener clickListener = new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                switch (which) {
                    //// TODO: 2016/10/26 倒计时还是老的data对应
                    case 0:
                        parsePacket.setDataCountdown(00, 10);
                        parsePacket.sendPacket(mDevice);
                        break;
                    case 1:
                        parsePacket.setDataCountdown(00, 20);
                        parsePacket.sendPacket(mDevice);
                        break;
                    case 2:
                        parsePacket.setDataCountdown(00, 30);
                        parsePacket.sendPacket(mDevice);
                        break;
                    case 3:
                        parsePacket.setDataCountdown(00, 60);
                        parsePacket.sendPacket(mDevice);
                        break;
                    case 4:
                        parsePacket.setDataCountdown(00, 90);
                        parsePacket.sendPacket(mDevice);
                        break;
                    case 5:
                        AlertDialog.Builder builder1 = new AlertDialog.Builder(Main2Activity.this, R.style.AlertDialog_AppCompat_mTime);
                        LayoutInflater inflater = LayoutInflater.from(Main2Activity.this);
                        final View view = inflater.inflate(R.layout.custom_countdown_layout, null);
                        AlertDialog alertDialog = builder1.setView(view)
                                .setPositiveButton("确定", new DialogInterface.OnClickListener() {
                                    @Override
                                    public void onClick(DialogInterface dialog, int which) {
                                        TimePicker timePicker = (TimePicker) view.findViewById(R.id.timePicker);
                                        int i = timePicker.getCurrentHour();
                                        int i1 = timePicker.getCurrentMinute();
                                        parsePacket.setDataCountdown(i, i1);
                                        parsePacket.sendPacket(mDevice);

                                    }
                                })
                                .setNegativeButton("取消", new DialogInterface.OnClickListener() {
                                    @Override
                                    public void onClick(DialogInterface dialog, int which) {
                                        ToastUtil.showToast(Main2Activity.this, "No");
                                    }
                                }).setMessage("倒计时时间")
                                .show();

                        Window window = alertDialog.getWindow();
                        TimePicker timePicker = (TimePicker) window.findViewById(R.id.timePicker);
                        timePicker.setIs24HourView(true);
                        break;
                }

            }
        };
        if (state == ON) {
            builder.setItems(R.array.countdown_data_on, clickListener).show();

        } else {
            builder.setItems(R.array.countdown_data_off, clickListener).show();

        }
    }

    @OnClick({R.id.button_error, R.id.button_error1})
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.button_error:
                new ParsePacket().requestDeviceList(mDevice, (byte) 0);
                break;
            case R.id.button_error1:
                ParsePacket parsePacket = new ParsePacket();
                parsePacket.setType(ParsePacket.TYPE.APP_REQUEST);
                parsePacket.setCommand(UPDATE_DEVICE_COUNT);
                parsePacket.sendPacket(mDevice);
                break;
        }
    }
}
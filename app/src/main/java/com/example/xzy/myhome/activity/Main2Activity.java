package com.example.xzy.myhome.activity;

import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
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
import com.gizwits.gizwifisdk.enumration.GizWifiDeviceNetStatus;
import com.gizwits.gizwifisdk.enumration.GizWifiErrorCode;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;

import static com.example.xzy.myhome.util.ParsePacket.COMMAND.UPDATE_DEVICE_COUNT;
import static com.mxchip.helper.ProbeReqData.bytesToHex;

public class Main2Activity extends BaseActivity implements DeviceItemRecycerViewAdapter.DeviceSetListener {

    private static final int ON = 0;
    private static final int OFF = 1;
    private static  String MAC;
    private boolean controlled=false;
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
    private byte[] emptyacket = new byte[]{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    List<Device> deviceList;
    DeviceItemRecycerViewAdapter deviceRVAdapter;
    static ConcurrentHashMap<String, Object> dataMap = new ConcurrentHashMap<String, Object>();
    byte count = 0;
    byte lampuminance = (byte) 150;
    AlertDialog alertDialog;


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main2);
        ButterKnife.bind(this);
        AlertDialog.Builder builder = new AlertDialog.Builder(Main2Activity.this,R.style.MyAlertDialog);
        alertDialog = builder.setView(R.layout.logining)
                .setCancelable(false)
                .create();
        alertDialog.show();
        Intent intent = getIntent();
        mDevice = intent.getParcelableExtra("mDevice");
        mDevice.setListener(mDeviceListener);
        mDevice.getDeviceStatus();
        deviceList = new ArrayList<Device>();
        //initData();
        initView();

    }

    @Override
    protected void onStart() {
        super.onStart();
        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                if (controlled == false) {
                    finish();
                }
            }
        },20000);
    }

    @Override
    protected void onPause() {
        super.onPause();
        alertDialog.cancel();

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
        deviceRVAdapter.setDeviceSetListener(this);
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
                } else if (Arrays.equals(bytes,emptyacket)) {
                    //bytesToHex(bytes).equals(emptyacket)
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
        if (parsePacket.getType() == ParsePacket.TYPE.DEVICE_RESPONSE &&
                parsePacket.getDeviceType() == ParsePacket.DEVICE_TYPE.GATEWAY) {
            Log.d(TAG, "assessDataType: 收到网关数据，准备更新设备列表");
            updateDeviceList(parsePacket);
        } else if (parsePacket.getType() == ParsePacket.TYPE.DEVICE_REQUEST) {
            Log.d(TAG, "assessDataType: 收到设备请求，准备更新设备数据");
            updateDeviceData(parsePacket);
        } else {
            Log.e(TAG, "assessDataType:通讯代号错误：" + parsePacket.getType());
        }
    }

    private void updateDeviceData(ParsePacket parsePacket) {
        int index = getListIndex(parsePacket.getMac());
        if (index == -1) {
            Log.e(TAG, "processReceiveData: " + "收到未知设备的神秘请求");
            return;
        }
        if (parsePacket.getCommand() == ParsePacket.COMMAND.STATE_READ) {

            if (parsePacket.getDeviceType() == ParsePacket.DEVICE_TYPE.SENSOR_TEMPERATURE) {
                deviceList.get(index).setTemperture(parsePacket.getDataTemperature());
                deviceList.get(index).setHumidity(parsePacket.getDataHumidity());
                Log.i(TAG, "温度传感器数据" + deviceList.get(index) + "    " + parsePacket.getDataState());
                deviceRVAdapter.notifyDataSetChanged();
            } else {
                deviceList.get(index).setSwitchState(parsePacket.getDataState());
                Log.i(TAG, "接收到设备数据（温度传感器以外）" + deviceList.get(index) + "    " + parsePacket.getDataState());
                deviceRVAdapter.notifyDataSetChanged();

            }


        }

    }

    private int getListIndex(byte[] mac) {
        for (int a = 0;a<deviceList.size(); a++) {
            Log.i(TAG, "判断位  "+"deviceList.size():"+deviceList.size()+"\n"+
                    "deviceList.get(a).getMac()"+bytesToHex(deviceList.get(a).getMac())+"\n"+
                    "mac"+bytesToHex(mac)+"\n"+
                    "Arrays.equals(deviceList.get(a).getMac(),mac)"+Arrays.equals(deviceList.get(a).getMac(),mac)
            );

            if (Arrays.equals(deviceList.get(a).getMac(),mac))
                return a;

        }
        return -1;
    }

    private void updateDeviceList(ParsePacket parsePacket) {
        //// TODO: 控制命令
        //请求设备数量
        if (parsePacket.getCommand() == ParsePacket.COMMAND.UPDATE_DEVICE_COUNT||
                parsePacket.getCommand() == ParsePacket.COMMAND.DEVICE_RESPONSE_APP_COUNT) {
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
            int index=getListIndex(parsePacket.getDataMac());
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

        }
    }



    private void showCountdownDialog( final ParsePacket parsePacket) {
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

                                    }
                                }).setMessage("几小时几分后关闭？")
                                .show();

                        Window window = alertDialog.getWindow();
                        TimePicker timePicker = (TimePicker) window.findViewById(R.id.timePicker);
                        timePicker.setIs24HourView(true);
                        timePicker.setCurrentHour(0);
                        timePicker.setCurrentMinute(0);
                        break;
                }

            }
        };

            builder.setItems(R.array.countdown_data_off, clickListener).show();


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



    @Override
    public void onCountdownClick(int position) {
        ParsePacket parsePacket = new ParsePacket();

        Device device = deviceList.get(position);
        parsePacket.setType(ParsePacket.TYPE.APP_REQUEST)
                .setDeviceType(device.getDeviceType())
                .setMac(device.getMac())
                .setDataLength((byte) 8)
                .setCommand(ParsePacket.COMMAND.COUNTDOWN_WRITE);
        if (device.getDeviceType() == ParsePacket.DEVICE_TYPE.LAMP) {
            parsePacket.setDataTimeState(lampuminance);
        } else {
            parsePacket.setDataTimeState((byte) 1);
        }
        showCountdownDialog(parsePacket);
    }

    @Override
    public void onTimingClick(int position) {
        ParsePacket parsePacket = new ParsePacket();
        Device device = deviceList.get(position);
        parsePacket.setType(ParsePacket.TYPE.APP_REQUEST)
                .setDeviceType(device.getDeviceType())
                .setMac(device.getMac())
                .setDataLength((byte) 8)
                .setCommand(ParsePacket.COMMAND.TIMING_WRITE);
        if (device.getDeviceType() == ParsePacket.DEVICE_TYPE.LAMP) {
            parsePacket.setDataTimeState(lampuminance);
        } else {
            parsePacket.setDataTimeState((byte) 1);
        }
        Intent intent = new Intent(this, AlarmActivity.class);
        intent.putExtra("parsePacket", parsePacket);
        startActivity(intent);
    }

    @Override
    public void onNameClick(int position, View view) {
        //// TODO: 2016/10/26 名字修改

    }

    @Override
    public void onSwtichClick(int position, View view, byte switchState) {
        ParsePacket parsePacket = new ParsePacket();
        parsePacket.setMac(deviceList.get(position).getMac())
                .setDeviceType(deviceList.get(position).getDeviceType())
                .setType(ParsePacket.TYPE.APP_REQUEST)
                .setCommand(ParsePacket.COMMAND.STATE_WRITE)
                .setDataState(switchState)
                .setDataLength((byte) 1)
                .sendPacket(mDevice);
        lampuminance = switchState;
    }

    private void initData() {
        Device device1 = new Device();
        device1.setDeviceType(ParsePacket.DEVICE_TYPE.SENSOR_TEMPERATURE);
        device1.setMac(new byte[]{0,0,0,0,0,0,0,1});
        Device device2 = new Device();
        device2.setDeviceType(ParsePacket.DEVICE_TYPE.LAMP);
        device2.setMac(new byte[]{0,0,0,0,0,0,0,2});

        Device device3 = new Device();
        device3.setDeviceType(ParsePacket.DEVICE_TYPE.CURTAIN);
        device3.setMac(new byte[]{0,0,0,0,0,0,0,3});

        Device device4 = new Device();
        device4.setDeviceType(ParsePacket.DEVICE_TYPE.SOCKET);
        device4.setMac(new byte[]{0,0,0,0,0,0,0,4});

        Device device5 = new Device();
        device5.setDeviceType(ParsePacket.DEVICE_TYPE.SOCKET);
        device5.setMac(new byte[]{0,0,0,0,0,0,0,5});


        Collections.addAll(deviceList, device1,device2,device3,device4, device5);
    }

    @Override
    protected void mDidUpdateNetStatus(GizWifiDevice device, GizWifiDeviceNetStatus netStatus) {
        switch (netStatus) {
            case GizDeviceOnline:
                ToastUtil.showToast(Main2Activity.this, device.getProductName() +"设备状态变为:在线");
                break;
            case GizDeviceOffline:
                ToastUtil.showToast(Main2Activity.this, device.getProductName() +"设备状态变为:离线" );
                break;
            case GizDeviceControlled:
                alertDialog.cancel();
                controlled = true;
                ToastUtil.showToast(Main2Activity.this, device.getProductName() +"设备状态变为:可控");
                break;
            case GizDeviceUnavailable:
                ToastUtil.showToast(Main2Activity.this, device.getProductName() +"设备状态变为:难以获得的");
                break;
        }

    }
}
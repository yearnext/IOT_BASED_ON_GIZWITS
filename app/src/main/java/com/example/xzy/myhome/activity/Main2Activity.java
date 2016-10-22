package com.example.xzy.myhome.activity;

import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.support.v7.app.AlertDialog;
import android.support.v7.widget.PopupMenu;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.Window;
import android.widget.ImageButton;
import android.widget.TextView;
import android.widget.TimePicker;

import com.example.xzy.myhome.R;
import com.example.xzy.myhome.util.ParsePacket;
import com.example.xzy.myhome.util.ToastUtil;
import com.gizwits.gizwifisdk.api.GizWifiDevice;
import com.gizwits.gizwifisdk.enumration.GizWifiDeviceNetStatus;
import com.gizwits.gizwifisdk.enumration.GizWifiErrorCode;

import java.util.concurrent.ConcurrentHashMap;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;

import static com.example.xzy.myhome.util.ParsePacket.COMMAND;
import static com.example.xzy.myhome.util.ParsePacket.DATA;
import static com.example.xzy.myhome.util.ParsePacket.DATALENGTH;
import static com.example.xzy.myhome.util.ParsePacket.MAC;
import static com.example.xzy.myhome.util.ParsePacket.TYPE;
import static com.mxchip.helper.ProbeReqData.bytesToHex;

public class Main2Activity extends BaseActivity {
    GizWifiDevice mDevice;
    @BindView(R.id.tb_device_list1)
    Toolbar tbDeviceList;
    @BindView(R.id.tv_lamp_switch)
    TextView tvLampSwitch;
    @BindView(R.id.tv_lamp_timing)
    TextView tvLampTime;
    @BindView(R.id.tv_lamp_countdown)
    TextView tvLampCountdown;
    @BindView(R.id.tv_lamp_model)
    TextView tvLampModel;
    @BindView(R.id.tv_curtain_switch)
    TextView tvCurtainSwitch;
    @BindView(R.id.tv_curtain_timing)
    TextView tvCurtainTime;
    @BindView(R.id.tv_curtain_countdown)
    TextView tvCurtainCountdown;
    @BindView(R.id.tv_curtain_model)
    TextView tvCurtainModel;
    @BindView(R.id.tv_socket_switch)
    TextView tvSocketSwitch;
    @BindView(R.id.tv_socket_timing)
    TextView tvSocketTime;
    @BindView(R.id.tv_socket_countdown)
    TextView tvSocketCountdown;
    @BindView(R.id.tv_socket_model)
    TextView tvSocketModel;
    @BindView(R.id.button_lamp_switch)
    ImageButton buttonLampSwitch;
    @BindView(R.id.button_lamp_set)
    ImageButton buttonLampSet;
    @BindView(R.id.button_curtain_switch)
    ImageButton buttonCurtainSwitch;
    @BindView(R.id.button_curtain_set)
    ImageButton buttonCurtainSet;
    @BindView(R.id.button_socket_switch)
    ImageButton buttonSocketSwitch;
    @BindView(R.id.button_socket_set)
    ImageButton buttonSocketSet;
    @BindView(R.id.text_view_logcat)
    TextView textViewLogcat;
    byte eventNumber = 0;
    ParsePacket parsePacket;
    String logcat;
    int i = 0;
    static ConcurrentHashMap<String, Object> dataMap = new ConcurrentHashMap<String, Object>();


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main2);
        ButterKnife.bind(this);
        initView();
        Intent intent = getIntent();
        mDevice = intent.getParcelableExtra("mDevice");
        mDevice.setListener(mDeviceListener);
        mDevice.getDeviceStatus();
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
                            //// TODO: 2016/10/22 发送请求

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
                Log.i(TAG, "MdidReceiveData:有data");
                ConcurrentHashMap<String, Object> map = (ConcurrentHashMap<String, Object>) dataMap.get("data");
                byte[] bytes = (byte[]) map.get("Packet");
                if (bytes == null) {
                    Log.e(TAG, "MdidReceiveData: " + "bytes为空");
                    logcat = logcat + "第" + i++ + "次:" + "数据点 Packet 收到数据点数据为空" + "\n";
                } else {
                    StringBuilder stringBuilder = new StringBuilder();
                    for (byte a : bytes) {
                        Log.e(TAG, "MdidReceiveData: " + a);
                        stringBuilder.append(a);
                    }
                    logcat = logcat + "第" + i++ + "次:" + stringBuilder + "\n";
                    processingData(bytes);
                }
            } else {
                logcat = logcat + "第" + i++ + "次:数据型数据点为空" + "\n";
                Log.i(TAG, "MdidReceiveData:data为空");
            }
            textViewLogcat.setText(logcat);
            //无定义数据
            if (dataMap.get("binary") != null) {
                byte[] binary = (byte[]) dataMap.get("binary");
                Log.e(TAG, "无定义数据 Binary data:"
                        + bytesToHex(binary));
            }
        }
        if (result == GizWifiErrorCode.GIZ_SDK_DEVICE_NOT_READY) {
            ToastUtil.showToast(Main2Activity.this, "设备处于未就绪状态");
        } else {
            Log.e(TAG, "MdidReceiveData: 数据点回调失败 result=" + result);
        }


    }


    @OnClick({R.id.button_lamp_switch, R.id.button_lamp_set, R.id.button_curtain_switch,
            R.id.button_curtain_set, R.id.button_socket_switch, R.id.button_socket_set
            ,R.id.tv_error})
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.button_lamp_switch:
                ParsePacket lamp = new ParsePacket();
                if (tvLampSwitch.getText().equals("开"))
                    lamp.sendPacket(mDevice, TYPE.APP_WRITE, eventNumber, MAC.LAMP, DATALENGTH.SWITCH
                            , COMMAND.SWITCH, DATA.OFF);
                else
                    lamp.sendPacket(mDevice, TYPE.APP_WRITE, eventNumber, MAC.LAMP, DATALENGTH.SWITCH
                            , COMMAND.SWITCH, DATA.ON);
                break;
            case R.id.button_curtain_switch:
                ParsePacket curtain = new ParsePacket();
                if (tvLampSwitch.getText().equals("开"))
                    curtain.sendPacket(mDevice, TYPE.APP_WRITE, eventNumber, MAC.CURTAIN, DATALENGTH.SWITCH
                            , COMMAND.SWITCH, DATA.OFF);
                else
                    curtain.sendPacket(mDevice, TYPE.APP_WRITE, eventNumber, MAC.CURTAIN, DATALENGTH.SWITCH
                            , COMMAND.SWITCH, DATA.ON);
                break;
            case R.id.button_socket_switch:
                ParsePacket socket = new ParsePacket();
                if (tvLampSwitch.getText().equals("开"))
                    socket.sendPacket(mDevice, TYPE.APP_WRITE, eventNumber, MAC.SOCKET, DATALENGTH.SWITCH
                            , COMMAND.SWITCH, DATA.OFF);
                else
                    socket.sendPacket(mDevice, TYPE.APP_WRITE, eventNumber, MAC.SOCKET, DATALENGTH.SWITCH
                            , COMMAND.SWITCH, DATA.ON);
                break;
            case R.id.button_socket_set:
                parsePacket.setType(TYPE.APP_WRITE)
                        .setEventNumber(eventNumber)
                        .setMac(MAC.SOCKET);
                showSetMemu(buttonSocketSet);
                break;
            case R.id.button_curtain_set:
                parsePacket = new ParsePacket();
                parsePacket.setType(TYPE.APP_WRITE)
                        .setEventNumber(eventNumber)
                        .setMac(MAC.CURTAIN);
                showSetMemu(buttonCurtainSet);
                break;
            case R.id.button_lamp_set:
                parsePacket = new ParsePacket();
                parsePacket.setType(TYPE.APP_WRITE)
                        .setEventNumber(eventNumber)
                        .setMac(MAC.LAMP);
                showSetMemu(buttonLampSet);
                break;

            case R.id.tv_error:
                byte[] a = {11, 12, 12, 13, 13};
                dataMap.put("Packet",a );
                mDevice.write(dataMap, 0);
                break;


        }
    }


    private void showSetMemu(ImageButton buttonSet) {
        final PopupMenu popupMenu = new PopupMenu(Main2Activity.this, buttonSet, Gravity.END);
        popupMenu.getMenuInflater().inflate(R.menu.device_set_menu, popupMenu.getMenu());
        popupMenu.setOnMenuItemClickListener(new PopupMenu.OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(MenuItem item) {
                int id = item.getItemId();
                switch (id) {
                    case R.id.timing_set:
                        parsePacket.setCommand(COMMAND.TIMING);
                        parsePacket.setDataLength(DATALENGTH.TIMING);

                        Bundle bundle = new Bundle();
                        bundle.putSerializable("parsePacket", parsePacket);
                        bundle.putParcelable("mDevice", mDevice);
                        Intent i = new Intent(Main2Activity.this, AlarmActivity.class);
                        i.putExtras(bundle);
                        startActivity(i);
                        break;
                    case R.id.countdown_set:
                        parsePacket.setCommand(COMMAND.COUNTDOWN);
                        parsePacket.setDataLength(DATALENGTH.COUNTDOWN);
                        showCountdownDialog();
                        break;

                }
                return true;
            }
        });
        popupMenu.show();
    }

    private void showCountdownDialog() {
        final AlertDialog.Builder builder = new AlertDialog.Builder(Main2Activity.this);
        builder.setItems(R.array.countdown_data, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                switch (which) {
                    case 0:
                        parsePacket.setData(new byte[]{00, 10});
                        parsePacket.sendPacket(mDevice);
                        break;
                    case 1:
                        parsePacket.setData(new byte[]{00, 20});
                        parsePacket.sendPacket(mDevice);
                        break;
                    case 2:
                        parsePacket.setData(new byte[]{00, 30});
                        parsePacket.sendPacket(mDevice);
                        break;
                    case 3:
                        parsePacket.setData(new byte[]{00, 60});
                        parsePacket.sendPacket(mDevice);
                        break;
                    case 4:
                        parsePacket.setData(new byte[]{00, 90});
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
                                        byte[] bytes = new byte[2];
                                        int i = timePicker.getCurrentHour();
                                        int i1 = timePicker.getCurrentMinute();
                                        bytes[0] = (byte) i;
                                        bytes[1] = (byte) i1;
                                        parsePacket.setData(bytes);
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
        }).show();
    }

    private void processingData(byte[] b) {
        ParsePacket recevedData = new ParsePacket(b);
        switch (recevedData.getType()) {
            case TYPE.DEVICE_RESPONSE_APP_WRITE:
                if (recevedData.getCommand() == COMMAND.TIMING) {

                    recevedData.setType(TYPE.END);
                    uopdateTimingUI(recevedData.getMac(), recevedData);
                } else if (recevedData.getCommand() == COMMAND.COUNTDOWN) {

                    recevedData.setType(TYPE.END);
                    uopdateCountdownUI(recevedData.getMac(), recevedData);

                }
            case TYPE.DEVICE_RESPONSE_APP_READ:
                if (recevedData.getCommand() == COMMAND.SWITCH) {
                    recevedData.setType(TYPE.END);
                    uopdateSwitchUI(recevedData.getMac(), recevedData);
                }

                break;
            case TYPE.DEVICE_WRITE:
                if (recevedData.getCommand() == COMMAND.SWITCH) {
                    recevedData.setType(TYPE.APP_RESPONSE_DEVICE_WRITE);
                    uopdateSwitchUI(recevedData.getMac(), recevedData);
                }


                break;

        }
    }

    private void uopdateSwitchUI(byte[] mac, ParsePacket recevedData) {
        byte[] b = recevedData.getData();
        if (mac.equals(MAC.LAMP)) {

            if (b[0] == 1) {
                tvLampSwitch.setText("开");
            } else {
                tvLampSwitch.setText("关");
            }
        } else if (mac.equals(MAC.CURTAIN)) {

            if (b[0] == 1) {
                tvCurtainSwitch.setText("开");
            } else {
                tvCurtainSwitch.setText("关");
            }


        } else if (mac.equals(MAC.SOCKET)) {

            if (b[0] == 1) {
                tvSocketSwitch.setText("开");
            } else {
                tvSocketSwitch.setText("关");
            }

        }

    }

    private void uopdateTimingUI(byte[] mac, ParsePacket recevedData) {
        byte[] b = recevedData.getData();
        byte startHour = b[0];
        byte startMin = b[1];
        byte endHour = b[2];
        byte endMin = b[3];
        if (mac.equals(MAC.LAMP)) {


            tvLampTime.setText(startHour + ":" + startMin + "-" + endHour + ":" + endMin);

        } else if (mac.equals(MAC.CURTAIN)) {


            tvCurtainTime.setText(startHour + ":" + startMin + "-" + endHour + ":" + endMin);


        } else if (mac.equals(MAC.SOCKET)) {


            tvSocketTime.setText(startHour + ":" + startMin + "-" + endHour + ":" + endMin);


        }

    }

    private void uopdateCountdownUI(byte[] mac, ParsePacket recevedData) {
        byte[] b = recevedData.getData();
        byte hour = b[0];
        byte min = b[1];
        if (mac.equals(MAC.LAMP)) {

            tvLampCountdown.setText(hour + ":" + min);
        } else if (mac.equals(MAC.CURTAIN)) {

            tvCurtainCountdown.setText(hour + ":" + min);


        } else if (mac.equals(MAC.SOCKET)) {


            tvSocketCountdown.setText(hour + ":" + min);


        }

    }

    //设备状态回调
    @Override
    protected void mDidUpdateNetStatus(GizWifiDevice device, GizWifiDeviceNetStatus netStatus) {
        ToastUtil.showToast(Main2Activity.this, "设备状态变为:" + netStatus);
        switch (netStatus) {
            case GizDeviceOnline:
                ToastUtil.showToast(Main2Activity.this, device.getProductName() + "设备状态变为:在线");
                break;
            case GizDeviceOffline:
                ToastUtil.showToast(Main2Activity.this, device.getProductName() + "设备状态变为:离线");
                break;
            case GizDeviceControlled:
                ToastUtil.showToast(Main2Activity.this, device.getProductName() + "设备状态变为:可控");
                break;
            case GizDeviceUnavailable:
                ToastUtil.showToast(Main2Activity.this, device.getProductName() + "设备状态变为:难以获得的");
                break;

        }


        Log.e(TAG, "mDidUpdateNetStatus: " + device.getProductName() + "   " + netStatus);
    }



}

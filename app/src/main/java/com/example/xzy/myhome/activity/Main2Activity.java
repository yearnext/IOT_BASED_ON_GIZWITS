package com.example.xzy.myhome.activity;

import android.app.Notification;
import android.app.NotificationManager;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.support.design.widget.Snackbar;
import android.support.v7.app.NotificationCompat;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ScrollView;
import android.widget.TextView;

import com.example.xzy.myhome.R;
import com.example.xzy.myhome.Setting;
import com.example.xzy.myhome.adapter.DeviceItemRecycleViewAdapter;
import com.example.xzy.myhome.model.bean.Device;
import com.example.xzy.myhome.model.bean.PacketBean;
import com.example.xzy.myhome.util.ToastUtil;
import com.gizwits.gizwifisdk.api.GizWifiDevice;
import com.gizwits.gizwifisdk.enumration.GizWifiDeviceNetStatus;
import com.yanzhenjie.recyclerview.swipe.Closeable;
import com.yanzhenjie.recyclerview.swipe.OnSwipeMenuItemClickListener;
import com.yanzhenjie.recyclerview.swipe.SwipeMenu;
import com.yanzhenjie.recyclerview.swipe.SwipeMenuCreator;
import com.yanzhenjie.recyclerview.swipe.SwipeMenuItem;
import com.yanzhenjie.recyclerview.swipe.SwipeMenuRecyclerView;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;

import static android.app.Notification.DEFAULT_ALL;
import static com.example.xzy.myhome.model.bean.PacketBean.COMMAND.UPDATE_DEVICE_COUNT;
import static com.mxchip.helper.ProbeReqData.bytesToHex;

public class Main2Activity extends BaseActivity implements DeviceItemRecycleViewAdapter.DeviceSetListener {

    @BindView(R.id.tb_device_list1)
    Toolbar tbDeviceList;
    @BindView(R.id.text_view_logcat)
    TextView textViewLogcat;
    @BindView(R.id.rv_device_item)
    SwipeMenuRecyclerView rvDeviceItem;
    @BindView(R.id.button_error1)
    Button buttonError1;
    @BindView(R.id.layout_debug)
    ScrollView layoutDebug;

    static int sLampLuminance = 150;

    private GizWifiDevice mGizDevice;
    private List<Device> mDeviceList;
    private DeviceItemRecycleViewAdapter deviceRVAdapter;
    SharedPreferences preferences;
    int mLuminance1;
    int mLuminance2;
    static int b;
    byte count;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main2);
        ButterKnife.bind(this);
        Intent intent = getIntent();
        mGizDevice = intent.getParcelableExtra("gizDevice");
        mGizDevice.setListener(mDeviceListener);
        mGizDevice.getDeviceStatus();
        mDeviceList = new ArrayList<>();
        SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(this);
        mLuminance1 = preferences.getInt(Setting.PREF_LUMINANCE1, 60);
        mLuminance2 = preferences.getInt(Setting.PREF_LUMINANCE2, 80);
        if (Setting.TEST) testInitData();
        if (!PreferenceManager.getDefaultSharedPreferences(this)
                .getBoolean(Setting.PREF_DEBUG, false))
            layoutDebug.setVisibility(View.GONE);
        //iniToolbarList();
        initRecycleView();

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mGizDevice.setSubscribe(false);
    }

    private void iniToolbarList() {
        tbDeviceList.inflateMenu(R.menu.devicelist_toolbar_menu);
        tbDeviceList.setOnMenuItemClickListener(new Toolbar.OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(MenuItem item) {
                int itemId = item.getItemId();
                switch (itemId) {
                    case R.id.add_item:
                        break;
                    case R.id.refresh_item:
                        break;
                    case R.id.delete_item:
                        break;
                }
                return true;
            }
        });
    }

    private void initRecycleView() {
        final int _width = getResources().getDimensionPixelSize(R.dimen.item_width);
        final int _height = ViewGroup.LayoutParams.MATCH_PARENT;
        deviceRVAdapter = new DeviceItemRecycleViewAdapter(mDeviceList);
        deviceRVAdapter.setDeviceSetListener(this);
        LinearLayoutManager linearLayoutManager = new LinearLayoutManager(this);
        rvDeviceItem.setLayoutManager(linearLayoutManager);
        rvDeviceItem.setAdapter(deviceRVAdapter);
        rvDeviceItem.setSwipeMenuCreator(new SwipeMenuCreator() {
            @Override
            public void onCreateMenu(SwipeMenu swipeLeftMenu, SwipeMenu swipeRightMenu, int viewType) {
                SwipeMenuItem setItem = new SwipeMenuItem(Main2Activity.this)
                        .setBackgroundColor(getResources().getColor(R.color.colorPrimary))
                        .setImage(R.drawable.ic_settings_white_24dp)
                        .setHeight(_height)
                        .setWidth(_width);
                swipeLeftMenu.addMenuItem(setItem);
            }
        });
        rvDeviceItem.setSwipeMenuItemClickListener(new OnSwipeMenuItemClickListener() {
            @Override
            public void onItemClick(Closeable closeable, int adapterPosition, int menuPosition, int direction) {
                closeable.smoothCloseMenu();
                Intent intent = new Intent(Main2Activity.this, SettingsActivity.class);
                intent.putExtra("device", mDeviceList.get(adapterPosition));
                intent.putExtra("gizDevice", mGizDevice);
                if (mDeviceList.get(adapterPosition).getDeviceType() == PacketBean.DEVICE_TYPE.SENSOR_TEMPERATURE) {
                    Snackbar.make(rvDeviceItem, "暂无相关设置", Snackbar.LENGTH_LONG).show();
                } else {
                    startActivity(intent);

                }
            }
        });
    }

    @Override
    protected void receiveSucceedData(byte[] b) {
        textViewLogcat.setText(sLogcat);
        PacketBean packetBean = new PacketBean(b);
        switch (PacketBean.receiveDataType(packetBean)) {
            case PacketBean.RECEIVE_DEVICE_TYPE.DEVICE:
                updateDeviceData(packetBean);
                break;
            case PacketBean.RECEIVE_DEVICE_TYPE.GATEWAY:
                updateDeviceList(packetBean);
                break;

        }
    }

    @Override
    protected void onStart() {
        super.onStart();
        if (sLogcat != null) {
            textViewLogcat.setText(sLogcat);
        }
    }

    private void updateDeviceData(PacketBean packetBean) {
        int index = getListIndex(packetBean.getMac());
        if (index == -1) {
            Log.e(TAG, "processReceiveData: " + "收到未知设备的神秘请求");
            return;
        }
        switch (packetBean.getCommand()) {
            case PacketBean.COMMAND.STATE_READ:
                updateDeviceState(index, packetBean);
                break;
            case PacketBean.COMMAND.TIME_READ:
                byte[] _mac = mDeviceList.get(index).getMac();
                byte _deviceType = mDeviceList.get(index).getDeviceType();
                PacketBean.updateDeviceTime(_mac, _deviceType, mGizDevice);
                break;
            case (byte) 0x18:
                NotificationManager notificationManager =
                        (NotificationManager) this.getSystemService(Context.NOTIFICATION_SERVICE);
                Notification notification =
                        new NotificationCompat.Builder(this)
                                .setSmallIcon(R.mipmap.ic_launcher)
                                .setContentTitle("警告")
                                .setContentText("下雨了")
                                .setDefaults(DEFAULT_ALL)
                                .build();
                notificationManager.notify(0,notification);
                break;
        }
    }

    private void updateDeviceState(int index, PacketBean packetBean) {
        if (packetBean.getDeviceType() == PacketBean.DEVICE_TYPE.SENSOR_TEMPERATURE) {
            mDeviceList.get(index).setTemperature(packetBean.getDataTemperature());
            mDeviceList.get(index).setHumidity(packetBean.getDataHumidity());
            Log.i(TAG, "温度传感器数据" + mDeviceList.get(index) + "    " + packetBean.getDataState());
            deviceRVAdapter.notifyItemChanged(index);
        } else {
            mDeviceList.get(index).setSwitchState(packetBean.getDataState());
            Log.i(TAG, "接收到设备状态" + mDeviceList.get(index) + "    " + packetBean.getDataState());
            deviceRVAdapter.notifyDataSetChanged();
        }
    }

    private int getListIndex(byte[] mac) {
        for (int a = 0; a < mDeviceList.size(); a++) {
            if (Arrays.equals(mDeviceList.get(a).getMac(), mac))
                return a;
        }
        return -1;
    }

    private void updateDeviceList(PacketBean packetBean) {
        //请求设备数
        if (packetBean.getCommand() == PacketBean.COMMAND.UPDATE_DEVICE_COUNT ||
                packetBean.getCommand() == PacketBean.COMMAND.DEVICE_RESPONSE_APP_COUNT) {

            byte[] data = packetBean.getData();
            if (count==data[0])
            {mDeviceList.removeAll(mDeviceList);
                return;}
            count = data[0];
            Log.i(TAG, "updateDeviceList: 接收到设备列表更新，有" + count + "台设备");

                new Thread(new Runnable() {
                    @Override
                    public void run() {

                        for (byte i = 0; i < count; i++) {
                            try {
                                Thread.sleep(1000);
                                new PacketBean().requestDeviceList(mGizDevice, i);
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                        }
                    }
                }).start();

            //获取设备信息
        } else if (packetBean.getCommand() == PacketBean.COMMAND.UPDATE_DEVICE_MESSAGE) {
            Device device = new Device();
            int index = getListIndex(packetBean.getDataMac());
            Log.w(TAG, "index:" + index);
            if (index != -1) {
                device = mDeviceList.get(index);
                device.setMac(packetBean.getDataMac());
                device.setDeviceType(packetBean.getDataDeviceType());
            } else {
                device.setMac(packetBean.getDataMac());
                device.setDeviceType(packetBean.getDataDeviceType());
                mDeviceList.add(device);
                appRequstDeviceData(device);
                deviceRVAdapter.notifyDataSetChanged();

            }
            Log.i(TAG, "获取到的设备MAC：" + bytesToHex(packetBean.getDataMac()) +
                    "类型" + packetBean.getDataDeviceType());
        }
    }

    @OnClick({R.id.button_1,R.id.button_2,R.id.button_3,R.id.button_4, R.id.button_error1, R.id.button_clear, R.id.button_update})
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.button_1:
                new PacketBean().requestDeviceList(mGizDevice, (byte) 0);
                break;
            case R.id.button_2:
                new PacketBean().requestDeviceList(mGizDevice, (byte) 1);
                break;
            case R.id.button_3:
                new PacketBean().requestDeviceList(mGizDevice, (byte) 2);
                break;
            case R.id.button_4:
                new PacketBean().requestDeviceList(mGizDevice, (byte) 3);
                break;
            case R.id.button_error1:
                PacketBean packetBean = new PacketBean();
                packetBean.setType(PacketBean.TYPE.APP_REQUEST);
                packetBean.setCommand(UPDATE_DEVICE_COUNT);
                packetBean.sendPacket(mGizDevice);
                break;
            case R.id.button_clear:
                sLogcat = "";
                textViewLogcat.setText(sLogcat);
                break;
            case R.id.button_update:
                if (sLogcat != null)
                    textViewLogcat.setText(sLogcat);
                break;

        }
    }

    @Override
    public void onSwitchClick(int position, byte switchState) {
        PacketBean packetBean = new PacketBean();
        packetBean.setMac(mDeviceList.get(position).getMac())
                .setDeviceType(mDeviceList.get(position).getDeviceType())
                .setType(PacketBean.TYPE.APP_REQUEST)
                .setCommand(PacketBean.COMMAND.STATE_WRITE)
                .setDataState(switchState)
                .setDataLength((byte) 1)
                .sendPacket(mGizDevice);
        sLampLuminance = switchState;

    }

    private void testInitData() {
        Device device1 = new Device();
        device1.setDeviceType(PacketBean.DEVICE_TYPE.SENSOR_TEMPERATURE);
        device1.setMac(new byte[]{0, 0, 0, 0, 0, 0, 0, 1});
        Device device2 = new Device();
        device2.setDeviceType(PacketBean.DEVICE_TYPE.LAMP);
        device2.setDeviceName("灯");
        device2.setMac(new byte[]{0, 0, 0, 0, 0, 0, 0, 2});

        Device device3 = new Device();
        device3.setDeviceType(PacketBean.DEVICE_TYPE.CURTAIN);
        device3.setDeviceName("窗帘");

        device3.setMac(new byte[]{0, 0, 0, 0, 0, 0, 0, 3});

        Device device4 = new Device();
        device4.setDeviceType(PacketBean.DEVICE_TYPE.SOCKET);
        device4.setDeviceName("插座1");
        device4.setMac(new byte[]{0, 0, 0, 0, 0, 0, 0, 4});

        Device device5 = new Device();
        device5.setDeviceType(PacketBean.DEVICE_TYPE.SOCKET);
        device4.setDeviceName("插座2");
        device5.setMac(new byte[]{0, 0, 0, 0, 0, 0, 0, 5});

        Collections.addAll(mDeviceList, device1, device2, device3, device4, device5);
    }

    @Override
    protected void mDidUpdateNetStatus(GizWifiDevice device, GizWifiDeviceNetStatus netStatus) {
        switch (netStatus) {
            case GizDeviceOnline:
                ToastUtil.showToast(Main2Activity.this, device.getProductName() + "设备上线");
                break;
            case GizDeviceOffline:
                Intent intent = new Intent("com.example.xzy.myhome.FORCE_OFFLINE");
                sendBroadcast(intent);
                ToastUtil.showToast(Main2Activity.this, device.getProductName() + "设备状态变为:离线");
                break;
            case GizDeviceControlled:
                PacketBean packetBean = new PacketBean();
                packetBean.setType(PacketBean.TYPE.APP_REQUEST);
                packetBean.setCommand(UPDATE_DEVICE_COUNT);
                packetBean.sendPacket(mGizDevice);
                ToastUtil.showToast(Main2Activity.this, device.getProductName() + "设备状态变为:可控");
                break;
            case GizDeviceUnavailable:

                ToastUtil.showToast(Main2Activity.this, device.getProductName() + "设备状态变为:难以获得的");
                break;
        }
    }

    private void appRequstDeviceData(Device device) {
        PacketBean packetBean = new PacketBean();
        packetBean.setMac(device.getMac())
                .setType(PacketBean.TYPE.APP_REQUEST)
                .setDeviceType(device.getDeviceType())
                .setCommand(PacketBean.COMMAND.STATE_READ)
                .setDataLength((byte) 0)
                .sendPacket(mGizDevice);
    }


}
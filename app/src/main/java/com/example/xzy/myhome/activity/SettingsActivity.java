package com.example.xzy.myhome.activity;

import android.content.DialogInterface;
import android.os.Bundle;
import android.support.v7.app.AlertDialog;
import android.support.v7.widget.CardView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.Window;
import android.widget.TimePicker;

import com.example.xzy.myhome.R;
import com.example.xzy.myhome.Setting;
import com.example.xzy.myhome.model.bean.Device;
import com.example.xzy.myhome.model.bean.PacketBean;
import com.gizwits.gizwifisdk.api.GizWifiDevice;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;

public class SettingsActivity extends BaseActivity {
    @BindView(R.id.card_curtain)
    CardView cardCurtain;
    private Device mDevice;
    private GizWifiDevice mGizDevice;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_settings);
        ButterKnife.bind(this);
        mDevice = (Device) getIntent().getSerializableExtra("device");
        mGizDevice = getIntent().getParcelableExtra("gizDevice");
        Log.e(TAG, "onCreate: "+mDevice.getDeviceType() );
        if (mDevice.getDeviceType() != PacketBean.DEVICE_TYPE.CURTAIN) {
            cardCurtain.setVisibility(View.GONE);
        }
    }

    public void onCountdownClick() {
        PacketBean packetBean = new PacketBean();
        packetBean.setType(PacketBean.TYPE.APP_REQUEST)
                .setDeviceType(mDevice.getDeviceType())
                .setMac(mDevice.getMac())
                .setDataLength((byte) 8)
                .setCommand(PacketBean.COMMAND.COUNTDOWN_WRITE);
        if (mDevice.getDeviceType() == PacketBean.DEVICE_TYPE.LAMP) {
            packetBean.setDataTimeState((byte) Main2Activity.sLampLuminance);
        } else {
            packetBean.setDataTimeState((byte) 1);
        }
    }

    private void showCountdownDialog(final PacketBean packetBean) {
        final AlertDialog.Builder builder = new AlertDialog.Builder(SettingsActivity.this);
        DialogInterface.OnClickListener clickListener = new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                switch (which) {
                    case 0:
                        packetBean.setDataCountdown(0, 10);
                        packetBean.sendPacket(mGizDevice);
                        break;
                    case 1:
                        packetBean.setDataCountdown(0, 20);
                        packetBean.sendPacket(mGizDevice);
                        break;
                    case 2:
                        packetBean.setDataCountdown(0, 30);
                        packetBean.sendPacket(mGizDevice);
                        break;
                    case 3:
                        packetBean.setDataCountdown(0, 60);
                        packetBean.sendPacket(mGizDevice);
                        break;
                    case 4:
                        packetBean.setDataCountdown(0, 90);
                        packetBean.sendPacket(mGizDevice);
                        break;
                    case 5:
                        AlertDialog.Builder builder1 = new AlertDialog.Builder(SettingsActivity.this, R.style.AlertDialog_AppCompat_mTime);
                        LayoutInflater inflater = LayoutInflater.from(SettingsActivity.this);
                        final View view = inflater.inflate(R.layout.custom_countdown_layout, null);
                        AlertDialog alertDialog = builder1.setView(view)
                                .setPositiveButton("确定", new DialogInterface.OnClickListener() {
                                    @Override
                                    public void onClick(DialogInterface dialog, int which) {
                                        TimePicker timePicker = (TimePicker) view.findViewById(R.id.timePicker);
                                        int i = timePicker.getCurrentHour();
                                        int i1 = timePicker.getCurrentMinute();
                                        packetBean.setDataCountdown(i, i1);
                                        packetBean.sendPacket(mGizDevice);

                                    }
                                })
                                .setNegativeButton("取消", new DialogInterface.OnClickListener() {
                                    @Override
                                    public void onClick(DialogInterface dialog, int which) {

                                    }
                                }).setMessage("几小时几分后关闭？")
                                .show();

                        Window window = alertDialog.getWindow();
                        assert window != null;
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

    public void onTimingClick() {
        PacketBean packetBean = new PacketBean();
        packetBean.setType(PacketBean.TYPE.APP_REQUEST)
                .setDeviceType(mDevice.getDeviceType())
                .setMac(mDevice.getMac())
                .setDataLength((byte) 8)
                .setCommand(PacketBean.COMMAND.TIMING_WRITE);
        if (mDevice.getDeviceType() == PacketBean.DEVICE_TYPE.LAMP) {
            packetBean.setDataTimeState(Setting.LAMP_LUMINANCE);
        } else {
            packetBean.setDataTimeState((byte) 1);
        }
    }

    @OnClick(R.id.button_setting_right)
    public void onClick() {
    }
}

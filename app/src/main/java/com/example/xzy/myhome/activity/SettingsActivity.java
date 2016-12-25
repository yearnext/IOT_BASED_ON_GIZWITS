package com.example.xzy.myhome.activity;

import android.app.ActivityOptions;
import android.app.TimePickerDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AlertDialog;
import android.support.v7.widget.CardView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.Window;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.TimePicker;

import com.example.xzy.myhome.R;
import com.example.xzy.myhome.Setting;
import com.example.xzy.myhome.model.bean.Device;
import com.example.xzy.myhome.model.bean.PacketBean;
import com.example.xzy.myhome.util.ToastUtil;
import com.gizwits.gizwifisdk.api.GizWifiDevice;

import java.util.Calendar;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;

public class SettingsActivity extends BaseActivity {
    @BindView(R.id.card_curtain)
    CardView cardCurtain;
    @BindView(R.id.checkbox_mon)
    CheckBox checkboxMon;
    @BindView(R.id.checkbox_tue)
    CheckBox checkboxTue;
    @BindView(R.id.checkbox_wed)
    CheckBox checkboxWed;
    @BindView(R.id.checkbox_thu)
    CheckBox checkboxThu;
    @BindView(R.id.checkbox_fri)
    CheckBox checkboxFri;
    @BindView(R.id.checkbox_sat)
    CheckBox checkboxSat;
    @BindView(R.id.checkbox_sun)
    CheckBox checkboxSun;
    @BindView(R.id.float_button_setting_right)
    FloatingActionButton floatButtonSettingRight;
    @BindView(R.id.text_curtain_luminance)
    TextView textCurtainLuminance;
    @BindView(R.id.text_countdown)
    TextView textCountdown;
    @BindView(R.id.text_timing_start)
    TextView textTimingStart;
    @BindView(R.id.text_timing_end)
    TextView textTimingEnd;
    @BindView(R.id.text_lamp_luminance1)
    TextView textLampLuminance1;
    @BindView(R.id.text_lamp_luminance2)
    TextView textLampLuminance2;
    @BindView(R.id.card_lamp)
    CardView cardLamp;

    public static final int START_TIMING = 0;
    public static final int END_TIMING = 1;

    final PacketBean countdownPacketBean = new PacketBean();
    @BindView(R.id.card_count_down)
    CardView cardCountDown;
    @BindView(R.id.card_timing)
    CardView cardTiming;
    @BindView(R.id.switch_curtain)
    Switch switchCurtain;
    @BindView(R.id.checkbox_re)
    CheckBox checkboxRe;
    private Device mDevice;
    private GizWifiDevice mGizDevice;
    byte[] mTimeData = {2, 0, 0, 0, 0, 0, 0, 0};
    int mLuminance1;
    int mLuminance2;
    SharedPreferences preferences;
    boolean isCountdown = false;
    boolean isTiming = false;
    boolean isCurtain = false;
    boolean isCurtainClose = false;
    byte mCurtainLuminance = -1;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_settings);
        ButterKnife.bind(this);
        mDevice = (Device) getIntent().getSerializableExtra("device");
        mGizDevice = getIntent().getParcelableExtra("gizDevice");
        Log.e(TAG, "onCreate: " + mDevice.getDeviceType());
        switch (mDevice.getDeviceType()) {
            case PacketBean.DEVICE_TYPE.CURTAIN:
                cardLamp.setVisibility(View.GONE);
                break;
            case PacketBean.DEVICE_TYPE.LAMP:
                cardCurtain.setVisibility(View.GONE);
                break;
            case PacketBean.DEVICE_TYPE.SOCKET:
                cardCurtain.setVisibility(View.GONE);
                cardLamp.setVisibility(View.GONE);
                break;
            case PacketBean.DEVICE_TYPE.SENSOR_TEMPERATURE:
                ToastUtil.showToast(this, "暂无相关设置设置");
                break;

        }

        preferences = PreferenceManager.getDefaultSharedPreferences(this);
        mLuminance1 = preferences.getInt(Setting.PREF_LUMINANCE1, 60);
        mLuminance2 = preferences.getInt(Setting.PREF_LUMINANCE2, 80);
        updateLuminanceUI(mLuminance1, mLuminance2);
    }

    @Override
    protected void onStop() {
        super.onStop();
        SharedPreferences.Editor e = preferences.edit();
        e.putInt(Setting.PREF_LUMINANCE1, mLuminance1);
        e.putInt(Setting.PREF_LUMINANCE2, mLuminance2);
        e.apply();
    }

    private void updateLuminanceUI(int luminance1, int luminance2) {
        textLampLuminance1.setText(String.valueOf(luminance1));
        textLampLuminance2.setText(String.valueOf(luminance2));
    }


    @OnClick({R.id.float_button_setting_right, R.id.switch_curtain, R.id.text_curtain_luminance,
            R.id.text_countdown, R.id.text_timing_start, R.id.text_timing_end,
            R.id.text_lamp_luminance1, R.id.text_lamp_luminance2})
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.float_button_setting_right:
                commitData();
                break;
            case R.id.switch_curtain:
                isCurtain = true;
                switchCurtain.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        isCurtainClose = true;
                    }
                });
                break;
            case R.id.text_curtain_luminance:
                showDialogCurtainLuminance();
                break;
            case R.id.text_countdown:
                showDialogCountdown();
                break;
            case R.id.text_timing_start:
                showDialogOpenTiming();
                break;
            case R.id.text_timing_end:
                showDialogCloseTiming();
                break;
            case R.id.text_lamp_luminance1:
                showDialogLuminance(1);
                break;
            case R.id.text_lamp_luminance2:
                showDialogLuminance(2);
                break;
        }
    }

    private void showDialogCurtainLuminance() {
        final View _view = LayoutInflater.from(this).inflate(R.layout.dialog_curtain_luninance, null);
        final AlertDialog alertDialog =
                new AlertDialog.Builder(SettingsActivity.this)
                        .setView(_view)
                        .setTitle("请输入一个0-100的数字")
                        .setPositiveButton("确定", new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialogInterface, int i) {
                                EditText editText = (EditText) _view.findViewById(R.id.et_curtain_luminance);
                                String l = (editText.getText().toString());
                                byte dl = (byte) Integer.parseInt(l);
                                if (dl >= 0 && dl < 101) {
                                    isCurtain = true;
                                    mCurtainLuminance = dl;
                                    textCurtainLuminance.setText(l);
                                } else {
                                    Snackbar.make(cardCurtain, "输入数值范围不正确", Snackbar.LENGTH_LONG).show();
                                }

                            }
                        }).create();
        alertDialog.show();
    }

    private void showDialogLuminance(final int luminanceMode) {
        LayoutInflater inflater = LayoutInflater.from(SettingsActivity.this);
        final View view = inflater.inflate(R.layout.dialog_luminance, null);
        new AlertDialog.Builder(SettingsActivity.this)
                .setView(view)
                .setTitle("请输入亮度值  0~99")
                .setPositiveButton("确定", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialogInterface, int i) {
                        EditText editText = (EditText) view.findViewById(R.id.edit_text_luminance);
                        i = Integer.parseInt(editText.getText().toString());
                        if (luminanceMode == 1) {
                            mLuminance1 = i;
                            textLampLuminance1.setText(String.valueOf(mLuminance1));
                        } else {
                            mLuminance2 = i;
                            textLampLuminance2.setText(String.valueOf(mLuminance2));
                        }
                    }
                }).show();
    }

    private void showDialogOpenTiming() {
        if (mDevice.getDeviceType() == PacketBean.DEVICE_TYPE.LAMP)
            mTimeData[6] = (byte) mLuminance2;
        else
            mTimeData[6] = (byte) 1;
        TimePickerDialog dialog = new TimePickerDialog(
                SettingsActivity.this,
                R.style.AlertDialog_AppCompat_mTime,
                new TimePickerDialog.OnTimeSetListener() {
                    @Override
                    public void onTimeSet(TimePicker view, int hourOfDay, int minute) {
                        mTimeData[2] = (byte) hourOfDay;
                        mTimeData[3] = (byte) minute;
                        String h = String.format("%02d", hourOfDay);
                        String m = String.format("%02d", minute);
                        textTimingStart.setText(h + ":" + m);
                        isTiming = true;
                    }
                }, 0, 0, true);
        dialog.show();
    }

    private void showDialogCloseTiming() {
        TimePickerDialog dialog = new TimePickerDialog(
                SettingsActivity.this,
                R.style.AlertDialog_AppCompat_mTime,
                new TimePickerDialog.OnTimeSetListener() {
                    @Override
                    public void onTimeSet(TimePicker view, int hourOfDay, int minute) {
                        mTimeData[4] = (byte) hourOfDay;
                        mTimeData[5] = (byte) minute;
                        String h = String.format("%02d", hourOfDay);
                        String m = String.format("%02d", minute);

                        textTimingEnd.setText(h + ":" + m);
                        isTiming = true;

                    }
                }, 0, 0, true);
        dialog.show();
    }


    private void commitData() {
        if (isCountdown) {
            isCountdown = false;
            countdownPacketBean.sendPacket(mGizDevice);
        }
        if (isTiming) {
            isTiming = false;
            updateCheckBox();
            if (checkboxRe.isChecked()) {
                mTimeData[0] = 0x03;
            }
            PacketBean p = new PacketBean();
            p.setType(PacketBean.TYPE.APP_REQUEST)
                    .setDeviceType(mDevice.getDeviceType())
                    .setMac(mDevice.getMac())
                    .setDataLength((byte) 8)
                    .setCommand(PacketBean.COMMAND.TIMING_WRITE)
                    .setDataTiming(mTimeData)
                    .sendPacket(mGizDevice);
        }
        if (isCurtain) {
            isCurtain = false;
            PacketBean p1 = new PacketBean();
            p1.setType(PacketBean.TYPE.APP_REQUEST)
                    .setDeviceType(mDevice.getDeviceType())
                    .setMac(mDevice.getMac())
                    .setDataLength((byte) 2)
                    .setCommand((byte) 0x17)
                    .setcloseLuminance(mCurtainLuminance)
                    .sendPacket(mGizDevice);
        }
        if (isCurtainClose) {
            isCurtainClose = false;
            byte i = 0;
            if (switchCurtain.isChecked())
                i = 1;
            switchCurtain.isChecked();
            PacketBean p2 = new PacketBean();
            p2.setType(PacketBean.TYPE.APP_REQUEST)
                    .setDeviceType(mDevice.getDeviceType())
                    .setMac(mDevice.getMac())
                    .setDataLength((byte) 1)
                    .setCommand((byte) 0x19)
                    .setDataState(i)
                    .sendPacket(mGizDevice);
        }
        Intent i = new Intent(this, DialogActivity.class);
        startActivity(i, ActivityOptions.makeSceneTransitionAnimation(this,
                floatButtonSettingRight, "float").toBundle());

    }

    private void updateCheckBox() {
        byte i = 0;
        if (checkboxMon.isChecked()) i = (byte) (i + 2);
        if (checkboxTue.isChecked()) i = (byte) (i + 4);
        if (checkboxWed.isChecked()) i = (byte) (i + 8);
        if (checkboxThu.isChecked()) i = (byte) (i + 16);
        if (checkboxFri.isChecked()) i = (byte) (i + 32);
        if (checkboxSat.isChecked()) i = (byte) (i + 64);
        if (checkboxSun.isChecked()) i = (byte) (i + 128);
        mTimeData[1] = i;
    }

    private void updateCountdown(int hour, int minute) {
        Calendar calendar = Calendar.getInstance();
        calendar.add(Calendar.HOUR_OF_DAY, hour);
        calendar.add(Calendar.MINUTE, minute);
        String h = String.format("%02d", calendar.get(Calendar.HOUR_OF_DAY));
        String m = String.format("%02d", calendar.get(Calendar.MINUTE));
        textCountdown.setText(h + ":" + m + "时自动关闭");
    }

    @Override
    public void onBackPressed() {
        AlertDialog.Builder _builder = new AlertDialog.Builder(this);
        _builder.setMessage("当前数据未保存，要直接退出么？")
                .setPositiveButton("保存", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialogInterface, int i) {
                        commitData();
                    }
                })
                .setNegativeButton("继续退出", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialogInterface, int i) {
                        finish();
                    }
                }).show();
    }

    private void showDialogCountdown() {

        countdownPacketBean.setType(PacketBean.TYPE.APP_REQUEST)
                .setDeviceType(mDevice.getDeviceType())
                .setMac(mDevice.getMac())
                .setDataLength((byte) 8)
                .setCommand(PacketBean.COMMAND.COUNTDOWN_WRITE);
        if (mDevice.getDeviceType() == PacketBean.DEVICE_TYPE.LAMP) {
            countdownPacketBean.setDataTimeState((byte) Main2Activity.sLampLuminance);
        } else {
            countdownPacketBean.setDataTimeState((byte) 1);
        }
        final AlertDialog.Builder builder = new AlertDialog.Builder(SettingsActivity.this);
        DialogInterface.OnClickListener clickListener = new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                switch (which) {
                    case 0:
                        updateCountdown(0, 2);
                        countdownPacketBean.setDataCountdown(0, 2);
                        isCountdown = true;
                        break;
                    case 1:
                        updateCountdown(0, 20);
                        countdownPacketBean.setDataCountdown(0, 20);
                        isCountdown = true;
                        break;
                    case 2:
                        updateCountdown(0, 30);

                        countdownPacketBean.setDataCountdown(0, 30);
                        isCountdown = true;
                        break;
                    case 3:
                        updateCountdown(0, 60);

                        countdownPacketBean.setDataCountdown(0, 60);
                        isCountdown = true;
                        break;
                    case 4:
                        updateCountdown(0, 90);

                        countdownPacketBean.setDataCountdown(0, 90);
                        isCountdown = true;
                        break;
                    case 5:
                        AlertDialog.Builder builder1 = new AlertDialog.Builder(SettingsActivity.this, R.style.AlertDialog_AppCompat_mTime);
                        LayoutInflater inflater = LayoutInflater.from(SettingsActivity.this);
                        final View view = inflater.inflate(R.layout.dialog_countdown, null);
                        AlertDialog alertDialog = builder1.setView(view)
                                .setPositiveButton("确定", new DialogInterface.OnClickListener() {
                                    @Override
                                    public void onClick(DialogInterface dialog, int which) {
                                        TimePicker timePicker = (TimePicker) view.findViewById(R.id.timePicker);
                                        int h = timePicker.getCurrentHour();
                                        int m = timePicker.getCurrentMinute();
                                        updateCountdown(h, m);
                                        countdownPacketBean.setDataCountdown(h, m);
                                        isCountdown = true;

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
}

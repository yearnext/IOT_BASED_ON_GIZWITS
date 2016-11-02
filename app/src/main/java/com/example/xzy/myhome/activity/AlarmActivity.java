package com.example.xzy.myhome.activity;

import android.app.TimePickerDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.LinearLayout;
import android.widget.TimePicker;

import com.example.xzy.myhome.R;
import com.example.xzy.myhome.util.ParsePacket;
import com.gizwits.gizwifisdk.api.GizWifiDevice;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;

public class AlarmActivity extends AppCompatActivity {

    @BindView(R.id.toolbar_alarm)
    Toolbar toolbarAlarm;
    @BindView(R.id.start_time_button)
    Button startTimeButton;
    @BindView(R.id.close_time_button)
    Button closeTimeButton;
    @BindView(R.id.button_ok)
    Button buttonOk;
    @BindView(R.id.rg_custom)
    LinearLayout rgCustom;
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

    byte[] timeData = {3, 0, 0, 0, 0, 0, 0, 0};
    ParsePacket parsePacket;
    GizWifiDevice mDevice;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_alarm);
        ButterKnife.bind(this);

        Intent intent = getIntent();
        parsePacket = (ParsePacket) intent.getSerializableExtra("parsePacket");
        mDevice = intent.getParcelableExtra("mDevice");
        toolbarAlarm.setTitle("定时设置");
        toolbarAlarm.setTitleTextColor(Color.WHITE);
        toolbarAlarm.setNavigationIcon(R.drawable.ic_close_white_24dp);
        toolbarAlarm.setNavigationOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                AlertDialog.Builder builder = new AlertDialog.Builder(AlarmActivity.this, R.style.AlertDialog_AppCompat_TimePicker);
                builder.setMessage("要舍弃更改么")
                        .setNegativeButton("取消", new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                            }
                        })
                        .setPositiveButton("确定", new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                                finish();
                            }
                        }).show();

            }
        });

    }


    @OnClick({R.id.start_time_button, R.id.close_time_button, R.id.button_ok})
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.start_time_button:
                TimePickerDialog start = new TimePickerDialog(AlarmActivity.this, R.style.AlertDialog_AppCompat_mTime,
                        new TimePickerDialog.OnTimeSetListener() {
                            @Override
                            public void onTimeSet(TimePicker view, int hourOfDay, int minute) {
                                timeData[2] = (byte) hourOfDay;
                                timeData[3] = (byte) minute;
                                startTimeButton.setText(hourOfDay+":"+minute);
                            }
                        }, 0, 0, true);
                start.show();
                start.onClick(start, 1);
                break;
            case R.id.close_time_button:
                TimePickerDialog close = new TimePickerDialog(AlarmActivity.this, R.style.AlertDialog_AppCompat_mTime, new TimePickerDialog.OnTimeSetListener() {
                    @Override
                    public void onTimeSet(TimePicker view, int hourOfDay, int minute) {
                        timeData[4] = (byte) hourOfDay;
                        timeData[5] = (byte) minute;
                        closeTimeButton.setText(hourOfDay+":"+minute);

                    }
                }, 0, 0, true);
                close.show();
                break;
            case R.id.button_ok:
                if (checkboxMon.isChecked()) timeData[1] = 1;
                if (checkboxTue.isChecked()) timeData[1] += 2;
                if (checkboxWed.isChecked()) timeData[1] += 4;
                if (checkboxThu.isChecked()) timeData[1] += 8;
                if (checkboxFri.isChecked()) timeData[1] += 16;
                if (checkboxSat.isChecked()) timeData[1] += 32;
                if (checkboxSun.isChecked()) timeData[1] += 64;
                parsePacket.setDataTiming(timeData);
                parsePacket.sendPacket(mDevice);
                finish();
                break;
                }

        }
    }



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
import android.widget.FrameLayout;
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
    byte[] timeData = {100, 100, 100, 100, 0, 0, 0, 0};
    ParsePacket parsePacket;
    GizWifiDevice mDevice;
    @BindView(R.id.button_ok)
    Button buttonOk;
    @BindView(R.id.fr_custom)
    FrameLayout frCustom;
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
                        .setNegativeButton("不舍弃", new DialogInterface.OnClickListener() {
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


    @OnClick({R.id.start_time_button, R.id.close_time_button, R.id.button_ok, R.id.radio_button_every_day, R.id.radio_button_one, R.id.radio_button_custom})
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.start_time_button:
                TimePickerDialog start = new TimePickerDialog(AlarmActivity.this, R.style.AlertDialog_AppCompat_mTime,
                        new TimePickerDialog.OnTimeSetListener() {
                            @Override
                            public void onTimeSet(TimePicker view, int hourOfDay, int minute) {
                                timeData[0] = (byte) hourOfDay;
                                timeData[1] = (byte) minute;
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
                        timeData[2] = (byte) hourOfDay;
                        timeData[3] = (byte) minute;
                        closeTimeButton.setText(hourOfDay+":"+minute);

                    }
                }, 0, 0, true);
                close.show();
                break;
            case R.id.button_ok:
                if (checkboxMon.isSelected()) {
                    timeData[7] = 1;
                } else if (checkboxTue.isSelected()) {
                    timeData[7] += 2;
                } else if (checkboxWed.isSelected()) {
                    timeData[7] += 4;
                } else if (checkboxThu.isSelected()) {
                    timeData[7] += 8;
                } else if (checkboxFri.isSelected()) {
                    timeData[7] += 16;
                } else if (checkboxSat.isSelected()) {
                    timeData[7] += 32;
                } else if (checkboxSun.isSelected()){
                    timeData[7] += 64;
                }
                parsePacket.setData(timeData);
                parsePacket.sendPacket(mDevice);
                break;
            case R.id.radio_button_every_day:
                rgCustom.setVisibility(View.GONE);
                frCustom.setVisibility(View.GONE);
                timeData[6] = 1;
                break;
            case R.id.radio_button_one:
                rgCustom.setVisibility(View.GONE);
                frCustom.setVisibility(View.GONE);
                timeData[6] = 0;
                break;
            case R.id.radio_button_custom:
                timeData[6] = 2;
                rgCustom.setVisibility(View.VISIBLE);
                frCustom.setVisibility(View.VISIBLE);
                break;
        }
    }


}

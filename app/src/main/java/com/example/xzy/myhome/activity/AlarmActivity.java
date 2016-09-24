package com.example.xzy.myhome.activity;

import android.app.TimePickerDialog;
import android.content.DialogInterface;
import android.graphics.Color;
import android.os.Bundle;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.View;

import com.example.xzy.myhome.R;

import butterknife.BindView;
import butterknife.ButterKnife;

public class AlarmActivity extends AppCompatActivity {

    @BindView(R.id.toolbar_alarm)
    Toolbar toolbarAlarm;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_alarm);
        ButterKnife.bind(this);
        toolbarAlarm.setTitle("定时设置");
        toolbarAlarm.setTitleTextColor(Color.WHITE);
        toolbarAlarm.setNavigationIcon(R.drawable.ic_close_white_24dp);
        toolbarAlarm.setNavigationOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                AlertDialog.Builder builder = new AlertDialog.Builder(AlarmActivity.this,R.style.AlertDialog_AppCompat_TimePicker);
                builder.setMessage("要舍弃更改么")
                        .setNegativeButton("不舍弃", new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {

                            }
                        })
                        .setPositiveButton("确定", new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {

                            }
                        }).show();

            }
        });

    }


    void alarmShow() {
        TimePickerDialog t = new TimePickerDialog(AlarmActivity.this, R.style.AlertDialog_AppCompat_mTime, null, 10, 12, true);
        t.show();
    }
}

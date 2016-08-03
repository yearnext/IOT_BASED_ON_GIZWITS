package com.example.xzy.myhome.activity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import com.example.xzy.myhome.R;
import com.gizwits.gizwifisdk.api.GizWifiDevice;
import com.gizwits.gizwifisdk.api.GizWifiSDK;
import com.gizwits.gizwifisdk.enumration.GizWifiErrorCode;

import java.util.concurrent.ConcurrentHashMap;

import static com.example.xzy.myhome.R.id.toggleButton_motor;

public class DeviceActivity extends BaseActivity implements View.OnClickListener {
    GizWifiDevice mDevice;
    int sn;
    ConcurrentHashMap<String, Object> command = new ConcurrentHashMap<String, Object>();
    TextView textViewLED;
    TextView textViewDoorbell;
    TextView textViewMotor;
    TextView textViewSocket;
    ToggleButton toggleButtonLED;
    ToggleButton toggleButtonMotor;
    ToggleButton toggleButtonSocket;
    SeekBar seekBarMotor;
    int toggleMotorData;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_device);
        initViews();
        Intent intent = getIntent();
        GizWifiSDK.sharedInstance().setListener(mListener);
        mDevice = intent.getParcelableExtra("mDevice");
        mDevice.setListener(mDeviceListener);
        mDevice.getDeviceStatus();
    }

    private void initViews() {
        toggleButtonLED = (ToggleButton) findViewById(R.id.toggleButton_LED);
        toggleButtonLED.setOnClickListener(this);
        toggleButtonMotor = (ToggleButton) findViewById(toggleButton_motor);
        toggleButtonMotor.setOnClickListener(this);
        toggleButtonSocket = (ToggleButton) findViewById(R.id.toggleButton_socket);
        toggleButtonSocket.setOnClickListener(this);
        textViewLED = (TextView) findViewById(R.id.textView_LED);
        textViewDoorbell = (TextView) findViewById(R.id.textView_doorbell);
        textViewMotor = (TextView) findViewById(R.id.textView_motor);
        textViewSocket = (TextView) findViewById(R.id.textView_socket);
        seekBarMotor = (SeekBar) findViewById(R.id.seekBar_motor);

        seekBarMotor.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                toggleMotorData = i;
                textViewMotor.setText(toggleMotorData+"");
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                command.put("Motor_data",toggleMotorData);
                mDevice.write(command, sn);

            }
        });
    }

    // 实现回调
        @Override
        public void MdidReceiveData(GizWifiErrorCode result, GizWifiDevice device, ConcurrentHashMap<String, Object> dataMap, int sn) {

            // 已定义的设备数据点，有布尔、数值和枚举型数据
            if (dataMap.get("data") != null) {
                ConcurrentHashMap<String, Object> map = (ConcurrentHashMap<String, Object>) dataMap.get("data");

                // 普通数据点，打印对应的key和value
                for (String key : map.keySet()) {
                    String data= map.get(key).toString();
                   Toast.makeText(DeviceActivity.this,
                            key+data, Toast.LENGTH_SHORT).show();
                    //判断数据
                    if (key.equals("Motor_toggle")) {
                        toggleButtonMotor.setChecked(data.equals("true"));


                    } else if (key.equals("Socket_toggle")){
                        toggleButtonSocket.setChecked(data.equals("true"));
                        if (data.equals("true"))
                            textViewSocket.setText("开");
                        else{
                            textViewSocket.setText("关");
                        }


                    } else if (key.equals("LED_toggle")) {
                        toggleButtonLED.setChecked(data.equals("true"));
                        if (data.equals("true"))
                            textViewLED.setText("开");
                        else{
                            textViewLED.setText("关");
                        }

                    } else if (key.equals("Motor_data")) {
                        toggleMotorData = Integer.parseInt(data);
                        seekBarMotor.setProgress(Integer.parseInt(data));
                        textViewMotor.setText(toggleMotorData+"");



                    } else if (key.equals("Doorbell_toggle")) {
                        if (data.equals("true"))
                            textViewDoorbell.setText("有人按门铃");
                        else{
                            textViewDoorbell.setText("没有人按门铃");
                        }

                    } else {
                        Toast.makeText(DeviceActivity.this, "有未定义数据点", Toast.LENGTH_SHORT).show();
                    }
                }

            }
        }




    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.toggleButton_LED:
                if (toggleButtonLED.isChecked()) {
                    textViewLED.setText("开");
                    command.put("LED_toggle", true);
                    mDevice.write(command, sn);
                } else {
                    textViewLED.setText("关");
                    command.put("LED_toggle", false);
                    mDevice.write(command, sn);
                }
                break;
            case R.id.toggleButton_motor:
                if (toggleButtonMotor.isChecked()) {
                    command.put("Motor_toggle", true);
                    mDevice.write(command, sn);

                } else {
                    command.put("Motor_toggle", false);
                    mDevice.write(command, sn);

                }
                break;
            case R.id.toggleButton_socket:
                if (toggleButtonSocket.isChecked()) {
                    textViewSocket.setText("开");
                    command.put("Socket_toggle", true);
                    mDevice.write(command, sn);

                } else {
                    textViewSocket.setText("关");
                    command.put("Socket_toggle", false);
                    mDevice.write(command, sn);
                }
                break;
        }
    }
}

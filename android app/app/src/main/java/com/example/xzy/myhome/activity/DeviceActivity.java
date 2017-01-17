package com.example.xzy.myhome.activity;

import android.content.Intent;
import android.os.Bundle;
import android.text.Editable;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;

import com.example.xzy.myhome.R;
import com.gizwits.gizwifisdk.api.GizWifiDevice;

import java.util.concurrent.ConcurrentHashMap;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;

public class DeviceActivity extends BaseActivity {
    GizWifiDevice mDevice;
    @BindView(R.id.textView_data)
    TextView textViewData;
    @BindView(R.id.editText_data)
    EditText editTextData;

    int count;

    public static byte[] HexString2Bytes(String src) {
        byte[] tmp = src.getBytes();
        byte[] ret = new byte[src.length() / 2 + 1];
        int i;
        for (i = 0; i < src.length() / 2; i++) {
            ret[i] = uniteBytes(tmp[i * 2], tmp[i * 2 + 1]);
        }
        if (src.length() % 2 != 0) {
            ret[i] = uniteBytes(tmp[i * 2], (byte) 48);
        }
        return ret;
    }

    public static byte uniteBytes(byte src0, byte src1) {
        byte _b0 = Byte.decode("0x" + new String(new byte[]{src0})).byteValue();
        _b0 = (byte) (_b0 << 4);
        byte _b1 = Byte.decode("0x" + new String(new byte[]{src1})).byteValue();
        byte ret = (byte) (_b0 ^ _b1);
        return ret;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_device);
        ButterKnife.bind(this);
        Intent intent = getIntent();
        mDevice = intent.getParcelableExtra("sGizDevice");
        mDevice.setListener(mDeviceListener);
        mDevice.getDeviceStatus();
    }



    private void test() {
        byte[] input1 = {11, 11, 11, 6, 15, 16, 17};
        ConcurrentHashMap<String, Object> dataMap = new ConcurrentHashMap<String, Object>();
        dataMap.put("Packet", input1);
        mDevice.write(dataMap, 0);
    }

    @OnClick({R.id.button_data, R.id.button_data_test})
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.button_data:
                Editable editText = editTextData.getText();
                String s = editText.toString();
                Log.e(TAG, "onClick: " + s);
                byte[] b = HexString2Bytes(s);
                ConcurrentHashMap<String, Object> dataMap = new ConcurrentHashMap<String, Object>();
                dataMap.put("Packet", b);
                mDevice.write(dataMap, 0);
                break;
            case R.id.button_data_test:
                test();
                break;
        }
    }
}


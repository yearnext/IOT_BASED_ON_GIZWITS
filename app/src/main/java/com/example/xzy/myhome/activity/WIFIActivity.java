package com.example.xzy.myhome.activity;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import com.example.xzy.myhome.R;
import com.gizwits.gizwifisdk.api.GizWifiSDK;
import com.gizwits.gizwifisdk.enumration.GizWifiConfigureMode;
import com.gizwits.gizwifisdk.enumration.GizWifiGAgentType;

import java.util.ArrayList;
import java.util.List;

public class WIFIActivity extends BaseActivity {

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_wifi);
        GizWifiSDK.sharedInstance().setListener(mListener);
        final EditText editTextWifiSsd = (EditText) findViewById(R.id.editText_wifi_ssd);
        final EditText editTextWifiPassword = (EditText) findViewById(R.id.editText_wifi_password);
        final EditText editTextAPSsd = (EditText) findViewById(R.id.editText_AP_ssd);
        final EditText editTextAPPassword = (EditText) findViewById(R.id.editText_AP_password);
        Button WIFIlogin = (Button) findViewById(R.id.button_wifi_login);
        WIFIlogin.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String WifiSSD = editTextWifiSsd.getText().toString();
                String WifiPassword = editTextWifiPassword.getText().toString();
                List<GizWifiGAgentType> types = new ArrayList<GizWifiGAgentType>();
                types.add(GizWifiGAgentType.GizGAgentESP); GizWifiSDK.sharedInstance().setDeviceOnboarding(WifiSSD, WifiPassword, GizWifiConfigureMode.GizWifiAirLink, null, 60, types);

            }
        });
        Button APlogin = (Button) findViewById(R.id.button_AP_login);
        APlogin.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String APSSD = editTextAPSsd.getText().toString();
                String APPassword = editTextAPPassword.getText().toString();
                //// TODO: 16/9/1 热点密码设置
                GizWifiSDK.sharedInstance().setDeviceOnboarding("CMCC-506", "abc123456", GizWifiConfigureMode.GizWifiSoftAP, null, 60, null);

            }
        });
    }



}

package com.example.xzy.myhome.activity;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import com.example.xzy.myhome.R;
import com.gizwits.gizwifisdk.api.GizWifiSDK;
import com.gizwits.gizwifisdk.enumration.GizWifiConfigureMode;
import com.gizwits.gizwifisdk.enumration.GizWifiErrorCode;
import com.gizwits.gizwifisdk.enumration.GizWifiGAgentType;
import com.gizwits.gizwifisdk.listener.GizWifiSDKListener;

import java.util.ArrayList;
import java.util.List;

public class WIFIActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
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
                GizWifiSDK.sharedInstance().setDeviceOnboarding(APSSD, APPassword, GizWifiConfigureMode.GizWifiSoftAP, "your_gagent_hotspot_prefix", 60, null);

            }
        });
    }


    GizWifiSDKListener mListener = new GizWifiSDKListener() {
        //等待配置完成或超时，回调配置完成接口
        @Override
        public  void didSetDeviceOnboarding (GizWifiErrorCode result, String mac, String did, String productKey) {
            if (result == GizWifiErrorCode.GIZ_SDK_SUCCESS) {
                Toast.makeText(WIFIActivity.this, "配置成功", Toast.LENGTH_SHORT).show();
                Log.d("WIFI", "didSetDeviceOnboarding:配置成功 ");
                // 配置成功
            } else {
                Toast.makeText(WIFIActivity.this, "配置失败", Toast.LENGTH_SHORT).show();
                Log.d("WIFI", "didSetDeviceOnboarding:配置失败 ");
                // 配置失败
            }
        }

    };
}

package com.example.xzy.myhome.activity;

import android.content.Intent;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.support.v7.app.AlertDialog;
import android.util.Log;
import android.widget.EditText;
import android.widget.Toast;

import com.example.xzy.myhome.R;
import com.gizwits.gizwifisdk.api.GizWifiSDK;
import com.gizwits.gizwifisdk.enumration.GizWifiConfigureMode;
import com.gizwits.gizwifisdk.enumration.GizWifiErrorCode;
import com.gizwits.gizwifisdk.enumration.GizWifiGAgentType;

import java.util.ArrayList;
import java.util.List;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;

public class WIFIActivity extends BaseActivity {

    @BindView(R.id.editText_wifi_ssd)
    EditText editTextWifiSsd;
    @BindView(R.id.editText_wifi_password)
    EditText editTextWifiPassword;

    AlertDialog alertDialog;
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_wifi);
        ButterKnife.bind(this);
        WifiManager wifiManager = (WifiManager) getSystemService(WIFI_SERVICE);
        WifiInfo wifiInfo = wifiManager.getConnectionInfo();
        String ssid = wifiInfo.getSSID().replaceAll("\"","");
        editTextWifiSsd.setText(ssid);
        editTextWifiSsd.setSelection(ssid.length());
        /*Button APlogin = (Button) findViewById(R.id.button_AP_login);
        APlogin.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String APSSD = editTextAPSsd.getText().toString();
                String APPassword = editTextAPPassword.getText().toString();
                //// TODO: 16/9/1 热点密码设置
                GizWifiSDK.sharedInstance().setDeviceOnboarding("CMCC-506", "abc123456", GizWifiConfigureMode.GizWifiSoftAP, null, 60, null);

            }
        });*/
    }


    @OnClick(R.id.button_wifi_login)
    public void onClick() {
        String WifiSSD = editTextWifiSsd.getText().toString();
        String WifiPassword = editTextWifiPassword.getText().toString();
        List<GizWifiGAgentType> types = new ArrayList<GizWifiGAgentType>();
        types.add(GizWifiGAgentType.GizGAgentESP);
        GizWifiSDK.sharedInstance().setDeviceOnboarding(WifiSSD, WifiPassword, GizWifiConfigureMode.GizWifiAirLink, null, 30, types);
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        alertDialog=builder.setTitle(R.string.AirLinkTitle)
                .setMessage(R.string.AirLinkMessage)
                .setCancelable(false).show();

    }

    @Override
    protected  void mDidSetDeviceOnboarding (GizWifiErrorCode result, String mac, String did, String productKey) {
        if (result == GizWifiErrorCode.GIZ_SDK_SUCCESS) {
            Toast.makeText(WIFIActivity.this, "配置成功", Toast.LENGTH_SHORT).show();
            Log.d("WIFI", "didSetDeviceOnboarding:UDP广播发送成功 ");
            Intent intent = new Intent(WIFIActivity.this, MainActivity.class);
            startActivity(intent);
        } else {
            Toast.makeText(WIFIActivity.this, "配置失败", Toast.LENGTH_SHORT).show();
            Log.e("WIFI", "didSetDeviceOnboarding:UDP广播发送失败   "+result);
            alertDialog.dismiss();

        }
    }


}

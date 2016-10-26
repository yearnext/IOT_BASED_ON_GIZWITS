package com.example.xzy.myhome.activity;

import android.content.Intent;
import android.content.SharedPreferences;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.support.v7.app.AlertDialog;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;
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
    @BindView(R.id.textView1)
    TextView textView1;
    private SharedPreferences preferences;
    private SharedPreferences.Editor editor;
    String ssid;

    String wifiSSD;
    String wifiPassword;
    List<GizWifiGAgentType> types;
    AlertDialog.Builder builder;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_wifi);
        ButterKnife.bind(this);
        preferences = PreferenceManager.getDefaultSharedPreferences(this);

        WifiManager wifiManager = (WifiManager) getSystemService(WIFI_SERVICE);
        WifiInfo wifiInfo = wifiManager.getConnectionInfo();
        ssid = wifiInfo.getSSID().replaceAll("\"", "");
        String wifiPassword = preferences.getString(ssid, null);
        if (!ssid.equals("<unknown ssid>")) {
            editTextWifiSsd.setText(ssid);
            editTextWifiSsd.setSelection(ssid.length());
        }


        if (wifiPassword != null) {
            editTextWifiPassword.setText(wifiPassword);
        }

    }


  /*  @OnClick(R.id.button_wifi_login)
    public void onClick() {
        String wifiSSD = editTextWifiSsd.getText().toString();
        String wifiPassword = editTextWifiPassword.getText().toString();
        List<GizWifiGAgentType> types = new ArrayList<GizWifiGAgentType>();
        types.add(GizWifiGAgentType.GizGAgentESP);
        GizWifiSDK.sharedInstance().setDeviceOnboarding(wifiSSD, wifiPassword, GizWifiConfigureMode.GizWifiAirLink, null, 30, types);
        //GizWifiSDK.sharedInstance().setDeviceOnboarding(wifiSSD, wifiPassword, GizWifiConfigureMode.GizWifiSoftAP, "XPG-GAgent-", 60, null);
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        alertDialog = builder.setTitle(R.string.AirLinkTitle)
                .setMessage(R.string.AirLinkMessage)
                .setCancelable(false).show();
        editor = preferences.edit();
        editor.putString(ssid, wifiPassword);
        editor.commit();

    }*/

    @Override
    protected void mDidSetDeviceOnboarding(GizWifiErrorCode result, String mac, String did, String productKey) {
        if (result == GizWifiErrorCode.GIZ_SDK_SUCCESS) {
            Toast.makeText(WIFIActivity.this, "配置成功", Toast.LENGTH_SHORT).show();
            Log.d("WIFI", "didSetDeviceOnboarding:UDP广播发送成功 ");
            Intent intent = new Intent(WIFIActivity.this, MainActivity.class);
            startActivity(intent);
        } else {
            Toast.makeText(WIFIActivity.this, "配置失败", Toast.LENGTH_SHORT).show();
            Log.e("WIFI", "didSetDeviceOnboarding:UDP广播发送失败   " + result);
            alertDialog.dismiss();

        }
    }


    @OnClick({R.id.button_wifi_login, R.id.button_wifi_login1})
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.button_wifi_login:
               String wifiSSD = editTextWifiSsd.getText().toString();
               String wifiPassword = editTextWifiPassword.getText().toString();
                List<GizWifiGAgentType> types = new ArrayList<GizWifiGAgentType>();
                types.add(GizWifiGAgentType.GizGAgentESP);
                GizWifiSDK.sharedInstance().setDeviceOnboarding(wifiSSD, wifiPassword, GizWifiConfigureMode.GizWifiAirLink, null, 30, types);
                //GizWifiSDK.sharedInstance().setDeviceOnboarding(wifiSSD, wifiPassword, GizWifiConfigureMode.GizWifiSoftAP, "XPG-GAgent-", 60, null);
                AlertDialog.Builder builder = new AlertDialog.Builder(this);
                alertDialog = builder.setTitle(R.string.AirLinkTitle)
                        .setMessage(R.string.AirLinkMessage)
                        .setCancelable(false).show();
                editor = preferences.edit();
                editor.putString(ssid, wifiPassword);
                editor.commit();
                break;
            case R.id.button_wifi_login1:
                wifiSSD = editTextWifiSsd.getText().toString();
                wifiPassword = editTextWifiPassword.getText().toString();
                types = new ArrayList<GizWifiGAgentType>();
                types.add(GizWifiGAgentType.GizGAgentESP);
                //GizWifiSDK.sharedInstance().setDeviceOnboarding(wifiSSD, wifiPassword, GizWifiConfigureMode.GizWifiAirLink, null, 30, types);
                GizWifiSDK.sharedInstance().setDeviceOnboarding(wifiSSD, wifiPassword, GizWifiConfigureMode.GizWifiSoftAP, "XPG-GAgent-", 60, null);
                builder = new AlertDialog.Builder(this);
                alertDialog = builder.setTitle(R.string.AirLinkTitle)
                        .setMessage(R.string.AirLinkMessage)
                        .setCancelable(false).show();
                editor = preferences.edit();
                editor.putString(ssid, wifiPassword);
                editor.commit();
                break;
        }
    }
}

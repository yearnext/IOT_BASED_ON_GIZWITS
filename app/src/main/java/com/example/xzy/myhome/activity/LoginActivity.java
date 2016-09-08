package com.example.xzy.myhome.activity;

import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.support.v7.app.AlertDialog;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.example.xzy.myhome.Config;
import com.example.xzy.myhome.R;
import com.example.xzy.myhome.util.ExceptionUtil;
import com.example.xzy.myhome.util.ToastUtil;
import com.gizwits.gizwifisdk.api.GizWifiSDK;
import com.gizwits.gizwifisdk.enumration.GizWifiErrorCode;

import java.util.ArrayList;
import java.util.List;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;

public class LoginActivity extends BaseActivity {
    @BindView(R.id.login_email)
    EditText eTLoginEmail;
    @BindView(R.id.login_password)
    EditText eTLoginPassword;
    @BindView(R.id.login_button)
    Button loginButton;
    @BindView(R.id.register_start)
    TextView registerStart;
    @BindView(R.id.login_anonymity)
    TextView loginAnonymity;
    private String loginEmail;
    private String loginPassword;
    AlertDialog ad;
    private SharedPreferences preferences;
    private SharedPreferences.Editor editor;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);
        ButterKnife.bind(this);
        preferences = PreferenceManager.getDefaultSharedPreferences(this);
        AlertDialog.Builder builder = new AlertDialog.Builder(this, R.style.MyAlertDialog);
        builder.setView(R.layout.logining);
        ad = builder.create();
        String account = preferences.getString("account", "");
        String password = preferences.getString("password", "");
        eTLoginEmail.setText(account);
        eTLoginPassword.setText(password);
    }



    @OnClick({R.id.login_button, R.id.register_start, R.id.login_anonymity})
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.login_button://登录
                loginEmail = eTLoginEmail.getText().toString();
                loginPassword = eTLoginPassword.getText().toString();
                if (ExceptionUtil.isException(loginEmail, loginPassword, LoginActivity.this)) break;
                ad.show();
                GizWifiSDK.sharedInstance().userLogin(loginEmail, loginPassword);
                editor = preferences.edit();
                editor.putString("account",loginEmail);
                editor.putString("password",loginPassword);
                editor.commit();
                break;

            case R.id.login_anonymity:
                GizWifiSDK.sharedInstance().userLoginAnonymous();
                ad.show();
                break;

            case R.id.register_start:
                startActivity(new Intent(LoginActivity.this, RegisterActivity.class));
                break;
        }
    }


    //登录回调
    @Override
    protected void mDidUserLogin(GizWifiErrorCode result, String uid, String token) {
        Log.e(TAG, "didUserLogin: 准备判断result");
        if (result == GizWifiErrorCode.GIZ_SDK_SUCCESS) {
            // 主动刷新绑定设备列表、指定筛选的设备productKey
            List<String> pks = new ArrayList<String>();
            pks.add(Config.PRODUCT_KEY);
            GizWifiSDK.sharedInstance().getBoundDevices(uid, token, pks);
        } else {
            // 登录失败
            ad.dismiss();
            ToastUtil.showToast(LoginActivity.this, "登录失败");

        }
    }
    //设备绑定结果回调
    @Override
    protected void MdidDiscovered(GizWifiErrorCode result){
        if (devices.size()==0){
            ToastUtil.showToast(LoginActivity.this, "登录成功"+devices.size());
            Intent intent = new Intent(LoginActivity.this, WIFIActivity.class);
            startActivity(intent);
            finish();
        }else {

            //ToastUtil.showToast(LoginActivity.this, "登录成功"+devices.size());
            Intent intent = new Intent(LoginActivity.this, MainActivity.class);
            startActivity(intent);
            finish();
        }


    };
}

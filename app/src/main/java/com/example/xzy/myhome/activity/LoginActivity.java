package com.example.xzy.myhome.activity;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AlertDialog;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.example.xzy.myhome.R;
import com.example.xzy.myhome.util.ExceptionUtil;
import com.example.xzy.myhome.util.ToastUtil;
import com.gizwits.gizwifisdk.api.GizWifiSDK;
import com.gizwits.gizwifisdk.enumration.GizWifiErrorCode;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;

public class LoginActivity extends BaseActivity {
    @BindView(R.id.login_email)
    EditText loginEmail;
    @BindView(R.id.login_password)
    EditText loginPassword;
    @BindView(R.id.login_button)
    Button loginButton;
    @BindView(R.id.register_start)
    TextView registerStart;
    @BindView(R.id.login_anonymity)
    TextView loginAnonymity;
    private String LoginEmail;
    private String LoginPassword;
    AlertDialog ad;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);
        ButterKnife.bind(this);
        AlertDialog.Builder builder = new AlertDialog.Builder(this,R.style.MyAlertDialog);
        builder.setView(R.layout.logining);
        ad = builder.create();
    }



    @OnClick({R.id.login_button, R.id.register_start, R.id.login_anonymity})
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.login_button://登录
                LoginEmail = loginEmail.getText().toString();
                LoginPassword = loginPassword.getText().toString();
                if (ExceptionUtil.isException(LoginEmail, LoginPassword, LoginActivity.this)) break;
                ad.show();
                GizWifiSDK.sharedInstance().userLogin(LoginEmail, LoginPassword);
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
    protected void mDidUserLogin(GizWifiErrorCode result) {
        Log.e(TAG, "didUserLogin: 准备判断result");
        ToastUtil.showToast(LoginActivity.this, "即将完成");
        if (result == GizWifiErrorCode.GIZ_SDK_SUCCESS) {
            // 登录成功
            ToastUtil.showToast(LoginActivity.this, "登录成功");
            Intent intent = new Intent(LoginActivity.this, MainActivity.class);
            startActivity(intent);
            finish();
        } else {
            // 登录失败
            ad.dismiss();
            ToastUtil.showToast(LoginActivity.this, "登录失败");

        }
    }
}

package com.example.xzy.myhome.activity;

import android.content.Intent;
import android.os.Bundle;
import android.support.design.widget.Snackbar;
import android.widget.EditText;
import android.widget.Toast;

import com.example.xzy.myhome.R;
import com.example.xzy.myhome.util.ExceptionUtil;
import com.example.xzy.myhome.util.ToastUtil;
import com.gizwits.gizwifisdk.api.GizWifiSDK;
import com.gizwits.gizwifisdk.enumration.GizUserAccountType;
import com.gizwits.gizwifisdk.enumration.GizWifiErrorCode;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;

public class RegisterActivity extends BaseActivity {
    @BindView(R.id.register_email)
    EditText registerEmail;
    @BindView(R.id.register_password)
    EditText registerPassword;
    String RegisterEmail;
    String RegisterPassword;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_register);
        ButterKnife.bind(this);
    }

    @Override
    protected void mDidRegisterUser(GizWifiErrorCode result) {
        if (result == GizWifiErrorCode.GIZ_SDK_SUCCESS) {
            Toast.makeText(RegisterActivity.this, "邮箱注册成功，自动登录", Toast.LENGTH_SHORT).show();
            GizWifiSDK.sharedInstance().userLogin(RegisterEmail, RegisterPassword);

        } else {
            Snackbar.make(registerEmail, "注册失败", Snackbar.LENGTH_SHORT).show();
        }
    }

    @OnClick(R.id.register_button)
    public void onClick() {
        RegisterEmail = registerEmail.getText().toString();
        RegisterPassword = registerPassword.getText().toString();
        if (ExceptionUtil.validateAccount(RegisterEmail, RegisterPassword, RegisterActivity.this))
            return;
        GizWifiSDK.sharedInstance().registerUser(RegisterEmail, RegisterPassword, null, GizUserAccountType.GizUserEmail);
    }

    @Override
    protected void mDidUserLogin(GizWifiErrorCode result, String uid, String token) {
        if (result == GizWifiErrorCode.GIZ_SDK_SUCCESS) {
            Intent intent = new Intent(RegisterActivity.this, WIFIActivity.class);
            startActivity(intent);
            finish();
        } else {

            ToastUtil.showToast(RegisterActivity.this, "登录失败");

        }
    }
}




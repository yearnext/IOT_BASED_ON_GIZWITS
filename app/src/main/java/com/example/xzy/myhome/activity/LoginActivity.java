package com.example.xzy.myhome.activity;

import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AlertDialog;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
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

import static com.example.xzy.myhome.Setting.PREF_ACCOUNT;
import static com.example.xzy.myhome.Setting.PREF_DEBUG;
import static com.example.xzy.myhome.Setting.PREF_PASSWORD;

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
    AlertDialog ad;
    @BindView(R.id.checkbox_debug)
    CheckBox checkboxDebug;

    private SharedPreferences mPreferences;


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);
        ButterKnife.bind(this);
        mPreferences = PreferenceManager.getDefaultSharedPreferences(this);
        AlertDialog.Builder builder = new AlertDialog.Builder(this, R.style.MyAlertDialog);
        builder.setView(R.layout.logining);
        ad = builder.create();
        String account = mPreferences.getString(PREF_ACCOUNT, "");
        String password = mPreferences.getString(PREF_PASSWORD, "");
        checkboxDebug.setChecked(mPreferences.getBoolean(PREF_DEBUG, false));
        eTLoginEmail.setText(account);
        eTLoginPassword.setText(password);
    }


    @OnClick({R.id.login_button, R.id.register_start, R.id.login_anonymity})
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.login_button://登录
                login();
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

    private void login() {
        String mLoginEmail = eTLoginEmail.getText().toString();
        String mLoginPassword = eTLoginPassword.getText().toString();
        if (ExceptionUtil.validateAccount(mLoginEmail, mLoginPassword, LoginActivity.this))
            return;
        ad.show();
        GizWifiSDK.sharedInstance().userLogin(mLoginEmail, mLoginPassword);
        SharedPreferences.Editor mEditor = mPreferences.edit();
        mEditor.putBoolean(PREF_DEBUG, checkboxDebug.isChecked());
        mEditor.putString("account", mLoginEmail);
        mEditor.putString("password", mLoginPassword);
        mEditor.putBoolean("loginState", true);
        mEditor.apply();
    }


    //登录回调
    @Override
    protected void mDidUserLogin(GizWifiErrorCode result, String uid, String token) {
        if (result == GizWifiErrorCode.GIZ_SDK_SUCCESS) {
            Intent intent = new Intent(LoginActivity.this, MainActivity.class);
            intent.putExtra("mUid", uid);
            intent.putExtra("mToken", token);
            startActivity(intent);
            ToastUtil.showToast(LoginActivity.this, "登录成功");
            ad.dismiss();
            finish();
        } else {
            ad.dismiss();
            Log.e(TAG, "mDidUserLogin: " + result);
            Snackbar.make(eTLoginEmail, "登录失败", Snackbar.LENGTH_LONG).show();
        }
}


}

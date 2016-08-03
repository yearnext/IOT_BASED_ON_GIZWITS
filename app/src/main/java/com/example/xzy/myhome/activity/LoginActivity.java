package com.example.xzy.myhome.activity;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.example.xzy.myhome.R;
import com.example.xzy.myhome.util.CircularAnimUtil;
import com.example.xzy.myhome.util.ExceptionUtil;
import com.example.xzy.myhome.util.ToastUtil;
import com.gizwits.gizwifisdk.api.GizWifiSDK;

public class LoginActivity extends BaseActivity implements View.OnClickListener{
    private EditText etLoginEmail;
    private EditText etLoginPassword;
    private Button loginButton;
    private ProgressBar mProgressBar;
    private String LoginEmail;
    private String LoginPassword;


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);
        GizWifiSDK.sharedInstance().setListener(mListener);
        initViews();

    }

    //控件初始化
    private void initViews() {
        etLoginEmail = (EditText) findViewById(R.id.login_email);
        etLoginPassword = (EditText) findViewById(R.id.login_password);
        loginButton = (Button) findViewById(R.id.login_button);
        TextView registerStart = (TextView) findViewById(R.id.register_start);
        TextView loginAnonymity = (TextView) findViewById(R.id.login_anonymity);
        mProgressBar = (ProgressBar) findViewById(R.id.progressBar);
        loginButton.setOnClickListener(this);
        registerStart.setOnClickListener(this);
        loginAnonymity.setOnClickListener(this);
        mProgressBar.setOnClickListener(this);
    }





    @Override
    public void onClick(View view) {
        switch (view.getId()) {

            case R.id.login_button:     //登录
                LoginEmail = etLoginEmail.getText().toString();
                LoginPassword = etLoginPassword.getText().toString();
                if(ExceptionUtil.isException(LoginEmail,LoginPassword,LoginActivity.this)) break;

                login();
                break;

            case R.id.login_anonymity:

                GizWifiSDK.sharedInstance().userLoginAnonymous();
                break;

            case R.id.register_start:

                startActivity(new Intent(LoginActivity.this,RegisterActivity.class));
                break;

            case R.id.progressBar:

                mProgressBar.setVisibility(View.GONE);
                loginButton.setEnabled(true);
                // 伸展按钮
                CircularAnimUtil.show(loginButton);
                break;
        }

    }



    //登录实现
    private void login() {
        loginButton.setEnabled(false);
        mProgressBar.setVisibility(View.VISIBLE);
        CircularAnimUtil.hide(loginButton);    // 收缩按钮
/*        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                GizWifiSDK.sharedInstance().userLogin(LoginEmail, LoginPassword);
            }
        },1000);*/
        GizWifiSDK.sharedInstance().userLogin(LoginEmail, LoginPassword);


    }


    // 登录成功按钮
    @Override
    protected View loginSucceedButton() {
        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                finish();
            }
        },1000);
        return loginButton;

    }


    //登录失败
    @Override
    protected void loginFail() {
        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                ToastUtil.showToast(LoginActivity.this, "登录失败");
                mProgressBar.setVisibility(View.GONE);
                loginButton.setEnabled(true);
                // 伸展按钮
                CircularAnimUtil.show(loginButton);
            }
        },1000);



    }



}

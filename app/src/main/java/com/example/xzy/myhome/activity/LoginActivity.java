package com.example.xzy.myhome.activity;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import com.example.xzy.myhome.R;
import com.gizwits.gizwifisdk.api.GizWifiSDK;
import com.gizwits.gizwifisdk.enumration.GizUserAccountType;
import com.gizwits.gizwifisdk.enumration.GizWifiErrorCode;
import com.gizwits.gizwifisdk.listener.GizWifiSDKListener;

public class LoginActivity extends AppCompatActivity implements View.OnClickListener{
    private EditText loginEmail;
    private EditText loginPassword;
    private EditText registerPassword;
    private EditText registerEmail;
    String LoginEmail;
    String LoginPassword;
    String RegisterEmail;
    String RegisterPassword;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);
        GizWifiSDK.sharedInstance().setListener(mListener);    //SDK初始化
        initViews();
        Button login = (Button) findViewById(R.id.login_button);
        Button register = (Button) findViewById(R.id.register_button);
        login.setOnClickListener(this);
        register.setOnClickListener(this);
    }

    //控件初始化
    private void initViews() {
        loginEmail = (EditText) findViewById(R.id.login_email);
        loginPassword = (EditText) findViewById(R.id.login_password);
        registerEmail = (EditText) findViewById(R.id.register_email);
        registerPassword = (EditText) findViewById(R.id.register_password);
    }


    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.login_button:
                login();
                finish();
                break;
            case R.id.register_button:
                register();
                break;
        }

    }

    //注册实现
    private void register() {
        RegisterEmail = registerEmail.getText().toString();
        RegisterPassword = registerPassword.getText().toString();
        GizWifiSDK.sharedInstance().registerUser(RegisterEmail, RegisterPassword, null, GizUserAccountType.GizUserEmail);
    }

    //登录实现
    private void login() {
        LoginEmail = loginEmail.getText().toString();
        LoginPassword = loginPassword.getText().toString();
        GizWifiSDK.sharedInstance().userLogin(LoginEmail, LoginPassword);


    }
    //SDK初始化
    GizWifiSDKListener mListener = new GizWifiSDKListener() {
        @Override
        public  void didUserLogin(GizWifiErrorCode result, String uid, String token) {
            if (result == GizWifiErrorCode.GIZ_SDK_SUCCESS) {
                // 登录成功
                Toast.makeText(LoginActivity.this, "登录成功", Toast.LENGTH_SHORT).show();
                Intent intent = new Intent(LoginActivity.this, MainActivity.class);
                intent.putExtra("uid", uid);
                intent.putExtra("token", token);
                startActivity(intent);


            } else {
                // 登录失败
                Toast.makeText(LoginActivity.this, "登录失败", Toast.LENGTH_SHORT).show();

            }
        }
        @Override
        public void didRegisterUser(GizWifiErrorCode result, String uid,  String token) {
            if (result == GizWifiErrorCode.GIZ_SDK_SUCCESS) {
                // 注册成功
                Toast.makeText(LoginActivity.this, "邮箱注册成功，自动登录", Toast.LENGTH_SHORT).show();
                GizWifiSDK.sharedInstance().userLogin(RegisterEmail, RegisterPassword);
            } else {
                // 注册失败
                Toast.makeText(LoginActivity.this, "邮箱注册失败"+"\n"+
                        "注册邮箱"+RegisterEmail+"\n"+"注册密码"+RegisterPassword, Toast.LENGTH_SHORT).show();
                Log.e("LoginActivity","邮箱注册失败"+"\n"+ "注册邮箱"+RegisterEmail+"\n"+"注册密码");
            }
        }
    };

}
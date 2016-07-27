package com.example.xzy.myhome.activity;

import android.os.Bundle;
import android.os.Handler;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ProgressBar;
import android.widget.Toast;

import com.example.xzy.myhome.R;
import com.example.xzy.myhome.util.CircularAnimUtil;
import com.example.xzy.myhome.util.ExceptionUtil;
import com.gizwits.gizwifisdk.api.GizWifiSDK;
import com.gizwits.gizwifisdk.enumration.GizUserAccountType;

public class RegisterActivity extends BaseActivity {
    private EditText registerPassword;
    private EditText registerEmail;
    Button register;
    ProgressBar mProgressBar;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_register);
        registerEmail = (EditText) findViewById(R.id.register_email);
        registerPassword = (EditText) findViewById(R.id.register_password);
        mProgressBar = (ProgressBar) findViewById(R.id.progressBar1);

        register = (Button) findViewById(R.id.register_button);
        register.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                RegisterEmail = registerEmail.getText().toString();
                RegisterPassword = registerPassword.getText().toString();
                if(ExceptionUtil.isException(RegisterEmail,RegisterPassword,RegisterActivity.this)) return;

                register();
            }
        });
    }

    //注册实现
    private void register() {
        register.setEnabled(false);
        mProgressBar.setVisibility(View.VISIBLE);
        CircularAnimUtil.hide(register);    // 收缩按钮
        GizWifiSDK.sharedInstance().registerUser(RegisterEmail, RegisterPassword, null, GizUserAccountType.GizUserEmail);



    }
    //注册成功
    @Override
    protected void registerSucceed() {
        Toast.makeText(RegisterActivity.this, "邮箱注册成功，自动登录", Toast.LENGTH_SHORT).show();
        GizWifiSDK.sharedInstance().userLogin(RegisterEmail, RegisterPassword);
    }
    //注册失败
    protected void registerFail()  {
        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                mProgressBar.setVisibility(View.GONE);
                register.setEnabled(true);
                // 伸展按钮
                CircularAnimUtil.show(register);
            }
        },1000);


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
        return register;

    }

}

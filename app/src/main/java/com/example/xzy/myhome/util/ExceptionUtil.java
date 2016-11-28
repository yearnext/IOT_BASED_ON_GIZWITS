package com.example.xzy.myhome.util;

import android.content.Context;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;

/**
 * Created by xzy on 16/7/27.
 */

public class ExceptionUtil {
    //判断账号问题
    public static boolean isException(String Email, String Password, Context context) {
        if ((Email.isEmpty()) || (Password.isEmpty())) {
            ToastUtil.showToast(context, "账号或密码为空");
            return true;
        }
        if (!isNetworkConnected(context)) {
            ToastUtil.showToast(context, "网络不可用");
            return true;
        }

        return false;
    }

    //判断网络
    public static boolean isNetworkConnected(Context context) {
        if (context != null) {
            ConnectivityManager mConnectivityManager = (ConnectivityManager) context
                    .getSystemService(Context.CONNECTIVITY_SERVICE);
            NetworkInfo mNetworkInfo = mConnectivityManager.getActiveNetworkInfo();
            if (mNetworkInfo != null) {
                return mNetworkInfo.isAvailable();
            }
        }
        return false;
    }
}

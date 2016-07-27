package com.example.xzy.myhome.util;

import android.content.Context;
import android.widget.Toast;

/**
 * Created by xzy on 16/7/26.
 */
public class ToastUtil {
    private static Toast toast;

    public static void showToast(Context context, String content) {
        if (toast == null) {
            toast = Toast.makeText(context, content, Toast.LENGTH_SHORT);
        } else {
            toast.setText(content);
        }
        toast.show();
    }
}

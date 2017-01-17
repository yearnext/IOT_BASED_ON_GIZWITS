package com.example.xzy.myhome.model;

import android.util.Log;

import static com.mxchip.helper.ProbeReqData.bytesToHex;

/**
 * Created by xzy on 2016/12/5.
 */

public class GizLog {
    private static int i;
    public static String updateLogcat(byte[] data) {
        String mData = bytesToHex(data);
        String logcat = "第" + i++ + "次:" +
                "通讯类型：=" + mData.substring(0, 2) + "；" + "\n" +
                "事件序号：=" + mData.substring(2, 4) + "；" + "\n" +
                "设备类型：=" + mData.substring(4, 6) + "；" + "\n" +
                "mac：=" + mData.substring(6, 22) + "；" + "\n" +
                "命令：=" + mData.substring(22, 24) + "；" + "\n" +
                "数据长度：=" + mData.substring(24, 26) + "；" + "\n" +
                "数据：=" + mData.substring(26, 61) + "；" + "\n" +
                "校验和：=" + mData.substring(62, 64)
                + "\n";
        Log.i("接收数据", "原始数据包" + bytesToHex(data));
        Log.i("接收数据", logcat);
        return logcat;
    }
    public static String updateAppSendLogcat(byte[] data) {
        String mData = bytesToHex(data);
        String logcat ="app发送数据："+
                "通讯类型：=" + mData.substring(0, 2) + "；" + "\n" +
                "事件序号：=" + mData.substring(2, 4) + "；" + "\n" +
                "设备类型：=" + mData.substring(4, 6) + "；" + "\n" +
                "mac：=" + mData.substring(6, 22) + "；" + "\n" +
                "命令：=" + mData.substring(22, 24) + "；" + "\n" +
                "数据长度：=" + mData.substring(24, 26) + "；" + "\n" +
                "数据：=" + mData.substring(26, 61) + "；" + "\n" +
                "校验和：=" + mData.substring(62, 64)
                + "\n";
        Log.i("发送数据", "原始数据包" + bytesToHex(data));
        Log.i("发送数据", logcat);
        return logcat;
    }
}

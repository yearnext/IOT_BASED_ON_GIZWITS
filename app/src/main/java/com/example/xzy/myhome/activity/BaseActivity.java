package com.example.xzy.myhome.activity;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.widget.Toast;

import com.example.xzy.myhome.util.ToastUtil;
import com.gizwits.gizwifisdk.api.GizWifiDevice;
import com.gizwits.gizwifisdk.api.GizWifiSDK;
import com.gizwits.gizwifisdk.api.GizWifiSSID;
import com.gizwits.gizwifisdk.enumration.GizEventType;
import com.gizwits.gizwifisdk.enumration.GizWifiErrorCode;
import com.gizwits.gizwifisdk.listener.GizWifiDeviceListener;
import com.gizwits.gizwifisdk.listener.GizWifiSDKListener;

import java.util.List;
import java.util.concurrent.ConcurrentHashMap;


public abstract class BaseActivity extends AppCompatActivity {
    String RegisterEmail;
    String RegisterPassword;
    String mUid;
    String mToken;
    String mDid;
    protected final String TAG = getClass().getSimpleName();
    // 使用缓存的设备列表刷新UI
    List<GizWifiDevice> devices = GizWifiSDK.sharedInstance().getDeviceList();

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        GizWifiSDK.sharedInstance().setListener(mListener);
    }

    GizWifiSDKListener mListener = new GizWifiSDKListener() {


        @Override
        public void didNotifyEvent(GizEventType eventType, Object eventSource, GizWifiErrorCode eventID, String eventMessage) {
            if (eventType == GizEventType.GizEventSDK) {
                // SDK的事件通知
                Log.i("GizWifiSDK", "SDK event happened: " + eventID + ", " + eventMessage);
            } else if (eventType == GizEventType.GizEventDevice) {
                // 设备连接断开时可能产生的通知
                GizWifiDevice mDevice = (GizWifiDevice)eventSource;
                Log.i("GizWifiSDK", "device mac: " + mDevice.getMacAddress() + " disconnect caused by eventID: " + eventID + ", eventMessage: " + eventMessage);
            } else if (eventType == GizEventType.GizEventM2MService) {
                // M2M服务返回的异常通知
                Log.i("GizWifiSDK", "M2M domain " + eventSource + " exception happened, eventID: " + eventID + ", eventMessage: " + eventMessage);
            } else if (eventType == GizEventType.GizEventToken) {
                // token失效通知
                Log.i("GizWifiSDK", "token " + eventSource + " expired: " + eventMessage);
            }

        }

        @Override    //登录回调
        public  void didUserLogin(GizWifiErrorCode result, String uid, String token) {
            mUid = uid;
            mToken = token;
            mDidUserLogin(result,uid,token);
        }
        @Override   //注册回调
        public void didRegisterUser(GizWifiErrorCode result, String uid,  String token) {
            MDidRegisterUser(result);
        }
        @Override   //绑定结果回调
        public void didDiscovered(GizWifiErrorCode result, List<GizWifiDevice> deviceList) {

            devices = deviceList;
            ToastUtil.showToast(BaseActivity.this,"size="+devices.size()+"   "+devices);
            MdidDiscovered( result);
        }

        @Override
        public void didBindDevice(GizWifiErrorCode result, String did) {
            mDid=did;
            if (result == GizWifiErrorCode.GIZ_SDK_SUCCESS) {
                Toast.makeText(BaseActivity.this,"绑定成功" , Toast.LENGTH_SHORT).show();

                // 绑定成功
            } else {
                Toast.makeText(BaseActivity.this,"绑定失败" , Toast.LENGTH_SHORT).show();

                // 绑定失败
            }
        }
        //
        @Override
        public  void didSetDeviceOnboarding (GizWifiErrorCode result, String mac, String did, String productKey) {
            mDidSetDeviceOnboarding(result, mac, did, productKey);

        }

        @Override
        public void didGetSSIDList(GizWifiErrorCode result, List<GizWifiSSID> ssids) {
            mDidGetSSIDList(result, ssids);
        }
    };




    GizWifiDeviceListener mDeviceListener = new GizWifiDeviceListener() {
        @Override
        public  void didSetSubscribe(GizWifiErrorCode result, GizWifiDevice device, boolean isSubscribed) {
            if (result == GizWifiErrorCode.GIZ_SDK_SUCCESS) {
                Toast.makeText(BaseActivity.this,"订阅或解除订阅成功" , Toast.LENGTH_SHORT).show();

                // 订阅或解除订阅成功
            } else {
                // 失败
                Toast.makeText(BaseActivity.this,"订阅或解除订阅失败" , Toast.LENGTH_SHORT).show();


            }
        }

        public void didReceiveData(GizWifiErrorCode result, GizWifiDevice device, ConcurrentHashMap<String, Object> dataMap, int sn){
            MdidReceiveData(result,device,dataMap,sn);
        }
    };

        protected void MdidReceiveData(GizWifiErrorCode result, GizWifiDevice device, ConcurrentHashMap<String, Object> dataMap, int sn){};
        protected void MdidDiscovered(GizWifiErrorCode result){};

    //登录回调
    protected  void mDidUserLogin(GizWifiErrorCode result, String uid, String token){};
    //注册回调
    protected  void MDidRegisterUser(GizWifiErrorCode result){};
    //WIFI配置回调
    protected  void mDidSetDeviceOnboarding (GizWifiErrorCode result, String mac, String did, String productKey) {}
    //在AP模式下获取WIFI列表
    protected  void mDidGetSSIDList(GizWifiErrorCode result, List<GizWifiSSID> ssids){}
}

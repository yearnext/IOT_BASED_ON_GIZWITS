package com.example.xzy.myhome.activity;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.widget.Toast;

import com.example.xzy.myhome.R;
import com.example.xzy.myhome.util.ActivityCollector;
import com.example.xzy.myhome.util.ToastUtil;
import com.gizwits.gizwifisdk.api.GizWifiDevice;
import com.gizwits.gizwifisdk.api.GizWifiSDK;
import com.gizwits.gizwifisdk.api.GizWifiSSID;
import com.gizwits.gizwifisdk.enumration.GizEventType;
import com.gizwits.gizwifisdk.enumration.GizWifiDeviceNetStatus;
import com.gizwits.gizwifisdk.enumration.GizWifiErrorCode;
import com.gizwits.gizwifisdk.listener.GizWifiDeviceListener;
import com.gizwits.gizwifisdk.listener.GizWifiSDKListener;

import java.util.Arrays;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;

import static com.example.xzy.myhome.model.GizLog.updateLogcat;
import static com.mxchip.helper.ProbeReqData.bytesToHex;


public abstract class BaseActivity extends AppCompatActivity {


    protected final String TAG = getClass().getSimpleName();
    private byte[] emptyData = new byte[]{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    protected String mLogcat;

    GizWifiSDKListener mListener = new GizWifiSDKListener() {


        @Override
        public void didNotifyEvent(GizEventType eventType, Object eventSource, GizWifiErrorCode eventID, String eventMessage) {
            if (eventType == GizEventType.GizEventSDK) {
                // SDK的事件通知
                Log.i("GizWifiSDK", "SDK event happened: " + eventID + ", " + eventMessage);
            } else if (eventType == GizEventType.GizEventDevice) {
                // 设备连接断开时可能产生的通知
                GizWifiDevice mDevice = (GizWifiDevice) eventSource;
                Log.i("GizWifiSDK", "device mac: " + mDevice.getMacAddress() +
                        " disconnect caused by eventID: " + eventID + ", eventMessage: " + eventMessage);
            } else if (eventType == GizEventType.GizEventM2MService) {
                // M2M服务返回的异常通知
                Log.i("GizWifiSDK", "M2M domain " + eventSource + " exception happened, eventID: "
                        + eventID + ", eventMessage: " + eventMessage);
            } else if (eventType == GizEventType.GizEventToken) {
                // token失效通知
                Log.i("GizWifiSDK", "token " + eventSource + " expired: " + eventMessage);
            }

        }

        @Override    //登录回调
        public void didUserLogin(GizWifiErrorCode result, String uid, String token) {
            mDidUserLogin(result, uid, token);
        }

        @Override   //注册回调
        public void didRegisterUser(GizWifiErrorCode result, String uid, String token) {
            mDidRegisterUser(result);
        }

        @Override   //绑定结果回调
        public void didDiscovered(GizWifiErrorCode result, List<GizWifiDevice> deviceList) {
            Log.i(TAG, "设备列表 " + deviceList);
            mDidDiscovered(result, deviceList);
        }

        @Override
        public void didBindDevice(GizWifiErrorCode result, String did) {
            if (result == GizWifiErrorCode.GIZ_SDK_SUCCESS) {
                Toast.makeText(BaseActivity.this, "绑定成功", Toast.LENGTH_SHORT).show();

                // 绑定成功
            } else {
                Toast.makeText(BaseActivity.this, "绑定失败", Toast.LENGTH_SHORT).show();

                // 绑定失败
            }
        }

        //
        @Override
        public void didSetDeviceOnboarding(GizWifiErrorCode result, String mac, String did, String productKey) {
            mDidSetDeviceOnboarding(result, mac, did, productKey);

        }

        @Override
        public void didGetSSIDList(GizWifiErrorCode result, List<GizWifiSSID> ssids) {
            mDidGetSSIDList(result, ssids);
        }

        //设备解绑
        @Override
        public void didUnbindDevice(GizWifiErrorCode result, String did) {
            if (result == GizWifiErrorCode.GIZ_SDK_SUCCESS) {
                Toast.makeText(BaseActivity.this, "解绑成功", Toast.LENGTH_SHORT).show();
                // 解绑成功
            } else {
                Toast.makeText(BaseActivity.this, "解绑失败", Toast.LENGTH_SHORT).show();
                // 解绑失败
            }
        }
    };
    GizWifiDeviceListener mDeviceListener = new GizWifiDeviceListener() {
        @Override
        public void didSetSubscribe(GizWifiErrorCode result, GizWifiDevice device, boolean isSubscribed) {
            if (result == GizWifiErrorCode.GIZ_SDK_SUCCESS) {
                Toast.makeText(BaseActivity.this, "订阅成功", Toast.LENGTH_SHORT).show();
                Log.i(TAG, "didSetSubscribe: 订阅成功");
            } else {
                // 失败
                Toast.makeText(BaseActivity.this, "订阅失败", Toast.LENGTH_SHORT).show();
                Log.e(TAG, "didSetSubscribe: 订阅失败 result=" + result);

            }
        }

        /**接收到云端数据回调
         * 根据接收到的数据判断，并更新日志
         * */
        @Override
        public void didReceiveData(GizWifiErrorCode result, GizWifiDevice device, ConcurrentHashMap<String, Object> dataMap, int sn) {

            if (result == GizWifiErrorCode.GIZ_SDK_SUCCESS) {
                Log.d(TAG, "mDidReceiveData:接收到云端数据");
                if (result == GizWifiErrorCode.GIZ_SDK_DEVICE_NOT_READY) {
                    ToastUtil.showToast(BaseActivity.this, "设备处于未就绪状态");
                    return;
                }
                if (dataMap.get("data") != null) {
                    Log.d(TAG, "mDidReceiveData:有data");
                    ConcurrentHashMap<String, Object> map = (ConcurrentHashMap<String, Object>) dataMap.get("data");
                    byte[] bytes = (byte[]) map.get(getApplication().getString(R.string.data_name));
                    if (bytes == null) {
                        Log.e(TAG, "mDidReceiveData: " + "bytes为空");
                    } else if (Arrays.equals(bytes, emptyData)) {
                        Log.w(TAG, "mDidReceiveData: " + "全为0数据一条");
                    } else {
                        mLogcat = mLogcat +updateLogcat(bytes);
                        receiveSucceedData(bytes);
                    }
                } else {
                    Log.w(TAG, "数据型数据点为空");
                }

                //无定义数据
                if (dataMap.get("binary") != null) {
                    byte[] binary = (byte[]) dataMap.get("binary");
                    Log.e(TAG, "无定义数据 Binary data:" + bytesToHex(binary));
                }
            }
        }

        @Override
        public void didUpdateNetStatus(GizWifiDevice device, GizWifiDeviceNetStatus netStatus) {
            mDidUpdateNetStatus(device, netStatus);
            Toast.makeText(BaseActivity.this, "设备状态变为:" + netStatus, Toast.LENGTH_SHORT).show();
        }
    };



    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        ActivityCollector.addActivity(this);
        GizWifiSDK.sharedInstance().setListener(mListener);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        ActivityCollector.removeActivity(this);
    }

    /**
     * 该回调主动上报设备状态回调备的网络状态变化，当设备重上电、断电或可控时会触发该回调
     *
     * @param device    回调的GizWifiDevice对象
     * @param netStatus 设备的状态
     */
    protected void mDidUpdateNetStatus(GizWifiDevice device, GizWifiDeviceNetStatus netStatus) {
        Toast.makeText(this, "设备状态变为:" + netStatus, Toast.LENGTH_SHORT).show();
        switch (netStatus) {
            case GizDeviceOnline:
                ToastUtil.showToast(BaseActivity.this, device.getProductName() + "设备状态变为:在线");
                break;
            case GizDeviceOffline:
                ToastUtil.showToast(BaseActivity.this, device.getProductName() + "设备状态变为:离线");
                break;
            case GizDeviceControlled:
                ToastUtil.showToast(BaseActivity.this, device.getProductName() + "设备状态变为:可控");
                break;
            case GizDeviceUnavailable:
                ToastUtil.showToast(BaseActivity.this, device.getProductName() + "设备状态变为:难以获得的");
                break;
        }
        Log.i(TAG, "设备状态回调:" + device.getProductName() + "   " + netStatus);
    }



    //登录回调
    protected void mDidUserLogin(GizWifiErrorCode result, String uid, String token) {
    }


    //注册回调
    protected void mDidRegisterUser(GizWifiErrorCode result) {
    }


    //WIFI配置回调
    protected void mDidSetDeviceOnboarding(GizWifiErrorCode result, String mac, String did, String productKey) {
    }

    //在AP模式下获取WIFI列表
    protected void mDidGetSSIDList(GizWifiErrorCode result, List<GizWifiSSID> ssids) {
    }

    //设备发现回调
    protected void mDidDiscovered(GizWifiErrorCode result, List<GizWifiDevice> deviceList) {
    }

    protected void receiveSucceedData(byte[] bytes){

    }
}

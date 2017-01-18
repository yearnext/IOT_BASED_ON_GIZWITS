package com.example.xzy.myhome.adapter;

import android.content.Context;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.TextView;

import com.example.xzy.myhome.R;
import com.example.xzy.myhome.Setting;
import com.example.xzy.myhome.model.bean.Device;
import com.example.xzy.myhome.model.bean.PacketBean;
import com.rm.rmswitch.RMSwitch;
import com.rm.rmswitch.RMTristateSwitch;
import com.yanzhenjie.recyclerview.swipe.SwipeMenuAdapter;

import java.util.List;

import butterknife.BindView;
import butterknife.ButterKnife;


/**
 * Created by xzy on 2016/10/24.
 */

public class DeviceItemRecycleViewAdapter extends SwipeMenuAdapter<RecyclerView.ViewHolder> {
    protected static final char[] hexArray = "0123456789ABCDEF".toCharArray();
    public static int sSocketCount = 0;
    public static int sCurtainCount = 0;
    public static int sLampCount = 0;
    private byte[] cuartainByte={1,0,2,0};
    int cuartainByteIndex = 0;
    static int b;
    static boolean isclick = true;
    List<Device> devices;
    Context context;
    DeviceSetListener deviceSetListener;
    byte switchState;
    SharedPreferences s;
    boolean isOpenCurtain = true;

    public DeviceItemRecycleViewAdapter(List<Device> devices) {
        this.devices = devices;
    }
    @Override
    public View onCreateContentView(ViewGroup parent, int viewType) {
        context = parent.getContext();
        s = PreferenceManager.getDefaultSharedPreferences(context);
        LayoutInflater inflater = LayoutInflater.from(parent.getContext());
        View deviceView = null;
        switch (viewType) {
            case PacketBean.DEVICE_TYPE.LAMP:
                deviceView = inflater.inflate(R.layout.item_lampt, parent, false);
                break;
            case PacketBean.DEVICE_TYPE.CURTAIN:
                deviceView = inflater.inflate(R.layout.item_curtain, parent, false);
                break;
            case PacketBean.DEVICE_TYPE.SENSOR_TEMPERATURE:
                deviceView = inflater.inflate(R.layout.item_temperature, parent, false);
                break;
            case PacketBean.DEVICE_TYPE.SOCKET:
                deviceView = inflater.inflate(R.layout.item_socket, parent, false);
                break;
        }
        return deviceView;
    }

    @Override
    public RecyclerView.ViewHolder onCompatCreateViewHolder(View realContentView, int viewType) {

        switch (viewType) {
            case PacketBean.DEVICE_TYPE.LAMP:
                return new LampHolder(realContentView);

            case PacketBean.DEVICE_TYPE.CURTAIN:
                return new CurtainHolder(realContentView);


            case PacketBean.DEVICE_TYPE.SENSOR_TEMPERATURE:
                return new TemperatureHolder(realContentView);


            case PacketBean.DEVICE_TYPE.SOCKET:
                return new MyViewHolder(realContentView);
        }
        return null;
    }


    @Override
    public void onBindViewHolder(final RecyclerView.ViewHolder holder, final int position) {
        Device device = devices.get(position);
        byte deviceState = device.getSwitchState();
        if (holder instanceof MyViewHolder) {
            //更新插座
            MyViewHolder holder1 = (MyViewHolder) holder;
                if (deviceState == 0)
                    holder1.switchDeviceItem.setChecked(false);
                else
                    holder1.switchDeviceItem.setChecked(true);
            holder1.switchDeviceItem.addSwitchObserver(new RMSwitch.RMSwitchObserver() {
                @Override
                public void onCheckStateChange(RMSwitch switchView, boolean isChecked) {
                    if (isChecked == true) {
                        switchState = 1;
                    } else {
                        switchState = 0;
                    }
                    deviceSetListener.onSwitchClick(position, switchState);
                }
            });
        } else if (holder instanceof LampHolder) {
            LampHolder lampHolder = (LampHolder) holder;
            if (deviceState ==s.getInt(Setting.PREF_LUMINANCE1, 60)) {
                lampHolder.mSwitch.setState(RMTristateSwitch.STATE_MIDDLE);
            } else if (deviceState==s.getInt(Setting.PREF_LUMINANCE2,80)){
                lampHolder.mSwitch.setState(RMTristateSwitch.STATE_RIGHT);
            } else if (deviceState > 0) {
                lampHolder.mSwitch.setState(RMTristateSwitch.STATE_MIDDLE);
            } else {
                lampHolder.mSwitch.setState(RMTristateSwitch.STATE_LEFT);
            }
                lampHolder.mSwitch.addSwitchObserver(new RMTristateSwitch.RMTristateSwitchObserver() {
                @Override
                public void onCheckStateChange(RMTristateSwitch switchView, int state) {
                    if (isclick) {
                        isclick = false;
                        Thread thread = new Mythread();
                        thread.start();
                        byte a = 0;
                        if (state == RMTristateSwitch.STATE_LEFT) {
                            a = 0;
                        } else if (state == RMTristateSwitch.STATE_RIGHT) {
                            a = (byte) s.getInt(Setting.PREF_LUMINANCE2, 80);
                        } else if (state == RMTristateSwitch.STATE_MIDDLE) {
                            a = (byte) s.getInt(Setting.PREF_LUMINANCE1, 60);
                        }
                        deviceSetListener.onSwitchClick(position, a);
                    }
                }
            });
        } else if (holder instanceof TemperatureHolder) {
            final TemperatureHolder temperatureHolder = (TemperatureHolder) holder;
            temperatureHolder.buttonTemperatureRefresh.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    deviceSetListener.onSwitchClick(position, (byte) 0);
                }
            });
            temperatureHolder.tvHumidity.setText((device.getHumidity() & 0xFF) + "");
            temperatureHolder.tvTemperature.setText((device.getTemperature() & 0xFF) + "");
        } else if (holder instanceof CurtainHolder) {
            switch (deviceState) {
                case 1:
                    ((CurtainHolder) holder).curtainStateing.setText("正在打开");

                    break;
                case 2:
                    ((CurtainHolder) holder).curtainStateing.setText("完成开启");
                    break;
                case 3:
                    ((CurtainHolder) holder).curtainStateing.setText("完全开启");
                    break;
                case 4:
                    ((CurtainHolder) holder).curtainStateing.setText("正在关闭");
                    break;
                case 5:
                    ((CurtainHolder) holder).curtainStateing.setText("完成关闭");
                    break;
                case 6:
                    ((CurtainHolder) holder).curtainStateing.setText("完全关闭");
                    break;

            }

            ((CurtainHolder) holder).curtainSwitch.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    if (cuartainByteIndex==4) cuartainByteIndex = 0;
                        deviceSetListener.onSwitchClick(position, cuartainByte[cuartainByteIndex++]);

                }
            });
        }
    }

    @Override
    public int getItemCount() {
        if (devices == null)
            return 0;
        return devices.size();
    }

    @Override
    public int getItemViewType(int position) {
        return devices.get(position).getDeviceType();
    }

    public void setDeviceSetListener(DeviceSetListener deviceSetListener) {
        this.deviceSetListener = deviceSetListener;
    }

    public interface DeviceSetListener {
        int a = 0;


        void onSwitchClick(int position, byte switchState);


    }

    public static class MyViewHolder extends RecyclerView.ViewHolder {
        @BindView(R.id.tv_device_item_name)
        TextView tvDeviceItemName;
        @BindView(R.id.image_device_item)
        ImageView imageDeviceItem;
        @BindView(R.id.switch_device_item)
        RMSwitch switchDeviceItem;


        public MyViewHolder(View itemView) {
            super(itemView);
            ButterKnife.bind(this, itemView);

        }
    }

    public static class LampHolder extends RecyclerView.ViewHolder {
        @BindView(R.id.tv_item_name_lamp)
        TextView tvDeviceItemNameLamp;
        @BindView(R.id.switch_device)
        RMTristateSwitch mSwitch;

        public LampHolder(View itemView) {
            super(itemView);
            ButterKnife.bind(this, itemView);

        }
    }

    public static class TemperatureHolder extends RecyclerView.ViewHolder {
        @BindView(R.id.tv_temperature)
        TextView tvTemperature;
        @BindView(R.id.tv_humidity)
        TextView tvHumidity;
        @BindView(R.id.button_temperature_refresh)
        ImageButton buttonTemperatureRefresh;

        public TemperatureHolder(View itemView) {
            super(itemView);
            ButterKnife.bind(this, itemView);
        }
    }

    public static class CurtainHolder extends RecyclerView.ViewHolder {
        @BindView(R.id.tv_curtain_stateing)
        TextView curtainStateing;
        @BindView(R.id.switch_curtain_item)
        ImageView curtainSwitch;

        public CurtainHolder(View itemView) {
            super(itemView);
            ButterKnife.bind(this, itemView);
        }
    }

    class Mythread extends Thread {
        @Override
        public void run() {
            super.run();
            try {
                sleep(200);
                isclick = true;
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

        }
    }


}

package com.example.xzy.myhome.adapter;

import android.content.Context;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CompoundButton;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.SeekBar;
import android.widget.Switch;
import android.widget.TextView;

import com.example.xzy.myhome.R;
import com.example.xzy.myhome.bean.Device;
import com.example.xzy.myhome.util.ParsePacket;

import java.util.List;

import butterknife.BindView;
import butterknife.ButterKnife;

import static android.content.ContentValues.TAG;
import static com.example.xzy.myhome.R.drawable.curtain;
import static com.example.xzy.myhome.R.drawable.lamp;
import static com.example.xzy.myhome.R.drawable.socket;


/**
 * Created by xzy on 2016/10/24.
 */

public class DeviceItemRecycerViewAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder> {
    protected static final char[] hexArray = "0123456789ABCDEF".toCharArray();
    List<Device> devices;
    Context context;
    DeviceSetListener deviceSetListener;
    byte switchState;
    byte lampLuminance;


    public DeviceItemRecycerViewAdapter(List<Device> devices) {
        this.devices = devices;
    }


    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        context = parent.getContext();
        LayoutInflater inflater = LayoutInflater.from(parent.getContext());
        if (viewType == ParsePacket.DEVICE_TYPE.LAMP) {
            View lampView = inflater.inflate(R.layout.item_lampt, parent, false);
            return new LampHolder(lampView);
        } else if (viewType == ParsePacket.DEVICE_TYPE.SENSOR_TEMPERATURE) {
            View lampView = inflater.inflate(R.layout.item_temperature, parent, false);
            return new TemperatureHolder(lampView);
        }
        View deviceView = inflater.inflate(R.layout.item_devicet, parent, false);
        return new MyViewHolder(deviceView);


    }


    @Override
    public void onBindViewHolder(final RecyclerView.ViewHolder holder, final int position) {
        Device device = devices.get(position);
        byte deviceType = device.getDeviceType();
        byte deviceState = device.getSwitchState();
        if (holder instanceof MyViewHolder) {
            MyViewHolder holder1 = (MyViewHolder) holder;


            if (deviceState == 0)

                holder1.switchDeviceItem.setChecked(false);
            else
                holder1.switchDeviceItem.setChecked(true);


            if (deviceType == ParsePacket.DEVICE_TYPE.SOCKET)
                holder1.imageDeviceItem.setImageResource(socket);
            else if (deviceType == ParsePacket.DEVICE_TYPE.LAMP) {
                holder1.imageDeviceItem.setImageResource(lamp);
            } else if (deviceType == ParsePacket.DEVICE_TYPE.CURTAIN) {
                holder1.imageDeviceItem.setImageResource(curtain);
            }

            holder1.switchDeviceItem.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
                @Override
                public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                    if (isChecked == true) {
                        switchState = 1;
                    } else {
                        switchState = 0;
                    }
                }
            });
            holder1.switchDeviceItem.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    Log.i("AG", "onCheckedChanged: 2");
                    deviceSetListener.onSwtichClick(position, v, switchState);
                }
            });
            holder1.tvDeviceItemName.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    deviceSetListener.onNameClick(position, v);

                }
            });
            holder1.tvDeviceItemCountdown.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    deviceSetListener.onCountdownClick(position);
                }
            });
            holder1.tvDeviceItemTiming.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    deviceSetListener.onTimingClick(position);


                }
            });
        } else if (holder instanceof LampHolder) {
            LampHolder lampHolder = (LampHolder) holder;
            lampHolder.seekBarLamp.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
                @Override
                public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                    lampLuminance = (byte) progress;
                }

                @Override
                public void onStartTrackingTouch(SeekBar seekBar) {

                }

                @Override
                public void onStopTrackingTouch(SeekBar seekBar) {
                    deviceSetListener.onSwtichClick(position, seekBar, lampLuminance);
                }
            });
            //// TODO: 2016/11/3 灯的亮度
            lampHolder.seekBarLamp.setProgress(deviceState & 0xFF);
            Log.i(TAG, "onBindViewHolder: " + deviceState);
            lampHolder.tvDeviceItemNameLamp.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {

                }
            });
            lampHolder.tvDeviceItemCountdownLamp.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    deviceSetListener.onCountdownClick(position);

                }
            });
            lampHolder.tvDeviceItemTimingLamp.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    deviceSetListener.onTimingClick(position);

                }
            });

        } else if (holder instanceof TemperatureHolder) {
            final TemperatureHolder temperatureHolder = (TemperatureHolder) holder;
            temperatureHolder.buttonTemperatureRefresh.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    deviceSetListener.onSwtichClick(position, null, (byte) 0);
                }
            });
            temperatureHolder.tvHumidity.setText(device.getHumidity() + "");
            temperatureHolder.tvTemperature.setText(device.getTemperture() + "");

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

        void onCountdownClick(int position);

        void onTimingClick(int position);

        void onNameClick(int position, View view);

        void onSwtichClick(int position, View view, byte switchState);


    }

    public static class MyViewHolder extends RecyclerView.ViewHolder {
        @BindView(R.id.tv_device_item_name)
        TextView tvDeviceItemName;
        @BindView(R.id.image_device_item)
        ImageView imageDeviceItem;
        @BindView(R.id.tv_device_item_timing)
        TextView tvDeviceItemTiming;
        @BindView(R.id.tv_device_item_countdown)
        TextView tvDeviceItemCountdown;
        @BindView(R.id.switch_device_item)
        Switch switchDeviceItem;


        public MyViewHolder(View itemView) {
            super(itemView);
            ButterKnife.bind(this, itemView);

        }
    }

    public static class LampHolder extends RecyclerView.ViewHolder {
        @BindView(R.id.tv_item_name_lamp)
        TextView tvDeviceItemNameLamp;
        @BindView(R.id.tv_item_timing_lamp)
        TextView tvDeviceItemTimingLamp;
        @BindView(R.id.tv_item_countdown_lamp)
        TextView tvDeviceItemCountdownLamp;
        @BindView(R.id.seekBar_lamp)
        SeekBar seekBarLamp;


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


}

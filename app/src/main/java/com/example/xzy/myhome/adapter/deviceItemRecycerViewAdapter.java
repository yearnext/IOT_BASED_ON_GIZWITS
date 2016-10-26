package com.example.xzy.myhome.adapter;

import android.content.Context;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.Switch;
import android.widget.TextView;

import com.example.xzy.myhome.R;
import com.example.xzy.myhome.bean.Device;
import com.example.xzy.myhome.util.ParsePacket;

import java.util.List;

import butterknife.BindView;
import butterknife.ButterKnife;

import static com.example.xzy.myhome.R.drawable.curtain;
import static com.example.xzy.myhome.R.drawable.lamp;
import static com.example.xzy.myhome.R.drawable.socket;


/**
 * Created by xzy on 2016/10/24.
 */

public class DeviceItemRecycerViewAdapter extends RecyclerView.Adapter<DeviceItemRecycerViewAdapter.MyViewHolder> {
    List<Device> devices;
    Context context;
    DeviceSetListener deviceSetListener;
    boolean switchState=false;


    public DeviceItemRecycerViewAdapter(List<Device> devices) {
        this.devices = devices;
    }


    @Override
    public MyViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        context = parent.getContext();
        LayoutInflater inflater = LayoutInflater.from(parent.getContext());
        View deviceView = inflater.inflate(R.layout.device_list, parent, false);
        MyViewHolder viewHolder = new MyViewHolder(deviceView);
        return viewHolder;
    }

    @Override
    public void onBindViewHolder(final MyViewHolder holder, final int position) {
        Device device = devices.get(position);
        byte deviceType = device.getDeviceType();
        byte deviceState = device.getSwitchState();
        if (deviceType == ParsePacket.DEVICE_TYPE.LAMP) {
            //// TODO: 2016/10/25 灯调节用

        } else {
            if (deviceState == 0)

                holder.switchDeviceItem.setChecked(false);
            else
                holder.switchDeviceItem.setChecked(true);
        }


        if (deviceType == ParsePacket.DEVICE_TYPE.SOCKET)
            holder.imageDeviceItem.setImageResource(socket);
        else if (deviceType == ParsePacket.DEVICE_TYPE.LAMP) {
            holder.imageDeviceItem.setImageResource(lamp);
        } else if (deviceType == ParsePacket.DEVICE_TYPE.CURTAIN) {
            holder.imageDeviceItem.setImageResource(curtain);
        }

        holder.switchDeviceItem.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                switchState = isChecked;
            }
        });
        holder.switchDeviceItem.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.i("AG", "onCheckedChanged: 2");
                deviceSetListener.onSwtichClick(position, holder.switchDeviceItem,switchState);
            }
        });
        holder.tvDeviceItemName.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                deviceSetListener.onNameClick(position, holder.tvDeviceItemName);

            }
        });
        holder.tvDeviceItemTiming.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                deviceSetListener.onTimingClick(position);
            }
        });
        holder.tvDeviceItemCountdown.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                deviceSetListener.onCountdownClick(position);

            }
        });
    }

    @Override
    public int getItemCount() {
        if (devices == null)
            return 0;
        return devices.size();
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

    public void setDeviceSetListener(DeviceSetListener deviceSetListener) {
        this.deviceSetListener = deviceSetListener;
    }

    ;

    public interface DeviceSetListener {
        int a = 0;

        void onCountdownClick(int position);

        void onTimingClick(int position);

        void onNameClick(int position, View view);

        void onSwtichClick(int position, View view, boolean switchState);


    }


}

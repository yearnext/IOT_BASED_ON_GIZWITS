package com.example.xzy.myhome.adapter;

import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.example.xzy.myhome.R;
import com.example.xzy.myhome.model.bean.IotDevice;

import java.util.List;

import butterknife.BindView;
import butterknife.ButterKnife;

/**
 * Created by xzy on 16/9/22.
 */

public class IotRecyclerAdapter extends RecyclerView.Adapter<IotRecyclerAdapter.MyViewHodler> {
    List<IotDevice> iotDeviceList;
    private OnItemClickLitener mOnItemClickLitener;

    public IotRecyclerAdapter(List<IotDevice> iotDeviceList) {
        this.iotDeviceList = iotDeviceList;
    }

    @Override
    public MyViewHodler onCreateViewHolder(ViewGroup parent, int viewType) {
        LayoutInflater inflater = LayoutInflater.from(parent.getContext());
        View childView = inflater.inflate(R.layout.item_iot_device, parent, false);
        MyViewHodler viewHodler = new MyViewHodler(childView);
        return viewHodler;
    }

    @Override
    public void onBindViewHolder(final MyViewHodler holder, int position) {
        IotDevice iotDevice = iotDeviceList.get(position);
        holder.tv_bind.setText(iotDevice.getBind());
        holder.tv_did.setText(iotDevice.getDid());
        holder.tv_disable.setText(iotDevice.getDisable());
        holder.tv_lan.setText(iotDevice.getLan());
        holder.tv_net_status.setText(iotDevice.getNetStatus());
        holder.tv_mac.setText(iotDevice.getMac());
        holder.tv_subscribed.setText(iotDevice.getSubscribed());
        if (mOnItemClickLitener != null) {
            holder.itemView.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    int pos = holder.getLayoutPosition();
                    mOnItemClickLitener.onItemClick(holder.itemView, pos);
                }
            });

        }
    }

    @Override
    public int getItemCount() {
        if (iotDeviceList == null)
            return 0;
        return iotDeviceList.size();
    }

    public void setOnItemClickLitener(OnItemClickLitener mOnItemClickLitener) {
        this.mOnItemClickLitener = mOnItemClickLitener;
    }

    public interface OnItemClickLitener {
        void onItemClick(View view, int position);
    }

    public static class MyViewHodler extends RecyclerView.ViewHolder {
        @BindView(R.id.tv_bind)
        TextView tv_bind;
        @BindView(R.id.tv_did)
        TextView tv_did;
        @BindView(R.id.tv_disable)
        TextView tv_disable;
        @BindView(R.id.tv_lan)
        TextView tv_lan;
        @BindView(R.id.tv_mac)
        TextView tv_mac;
        @BindView(R.id.tv_net_status)
        TextView tv_net_status;
        @BindView(R.id.tv_subscribed)
        TextView tv_subscribed;


        public MyViewHodler(View itemView) {
            super(itemView);
            ButterKnife.bind(this, itemView);
        }
    }
}

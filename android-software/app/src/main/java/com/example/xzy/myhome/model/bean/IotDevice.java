package com.example.xzy.myhome.model.bean;

/**
 * Created by xzy on 16/9/22.
 */

public class IotDevice {
    private String mDid;
    private String mMac;
    private String mSubscribed;
    private String mLan;
    private String mBind;
    private String mDisable;
    private String mNetStatus;

    public String getDisable() {
        return mDisable;
    }

    public IotDevice setDisable(String mDisable) {
        this.mDisable = mDisable;
        return this;
    }

    public String getDid() {
        return mDid;
    }

    public IotDevice setDid(String mDid) {
        this.mDid = mDid;
        return this;
    }

    public String getMac() {
        return mMac;
    }

    public IotDevice setMac(String mMac) {
        this.mMac = mMac;
        return this;
    }

    public String getSubscribed() {
        return mSubscribed;
    }

    public IotDevice setSubscribed(String mSubscribed) {
        this.mSubscribed = mSubscribed;
        return this;
    }

    public String getLan() {
        return mLan;
    }

    public IotDevice setLan(String mLan) {
        this.mLan = mLan;
        return this;
    }

    public String getBind() {
        return mBind;
    }

    public IotDevice setBind(String mBind) {
        this.mBind = mBind;
        return this;
    }

    public String getNetStatus() {
        return mNetStatus;
    }

    public IotDevice setNetStatus(String mNetStatus) {
        this.mNetStatus = mNetStatus;
        return this;
    }




}

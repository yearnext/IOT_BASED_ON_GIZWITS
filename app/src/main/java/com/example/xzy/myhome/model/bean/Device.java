package com.example.xzy.myhome.model.bean;

/**
 * Created by xzy on 2016/10/24.
 */

public class Device {


    byte deviceType;
    String deviceName;
    String timingState;
    String countdownState;
    byte[] mac;
    byte switchState;
    byte temperture;

    public byte getTemperture() {
        return temperture;
    }

    public void setTemperture(byte temperture) {
        this.temperture = temperture;
    }

    public byte getHumidity() {
        return humidity;
    }

    public void setHumidity(byte humidity) {
        this.humidity = humidity;
    }

    byte humidity;

    public byte getSwitchState() {
        return switchState;
    }

    public void setSwitchState(byte switchState) {
        this.switchState = switchState;
    }





    public byte[] getMac() {
        return mac;
    }

    public void setMac(byte[] mac) {
        this.mac = mac;
    }

    public byte getDeviceType() {
        return deviceType;
    }

    public void setDeviceType(byte deviceType) {
        this.deviceType = deviceType;
    }

    public String getDeviceName() {
        return deviceName;
    }

    public void setDeviceName(String deviceName) {
        this.deviceName = deviceName;
    }

    public String getTimingState() {
        return timingState;
    }

    public void setTimingState(String timingState) {
        this.timingState = timingState;
    }

    public String getCountdownState() {
        return countdownState;
    }

    public void setCountdownState(String countdownState) {
        this.countdownState = countdownState;
    }

}

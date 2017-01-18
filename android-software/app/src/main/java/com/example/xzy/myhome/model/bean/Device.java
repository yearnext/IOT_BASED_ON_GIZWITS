package com.example.xzy.myhome.model.bean;

import java.io.Serializable;

/**
 * Created by xzy on 2016/10/24.
 */

public class Device implements Serializable{


    byte deviceType;
    String deviceName;
    String timingState;
    String countdownState;
    byte[] mac;
    byte switchState;
    byte temperature;

    public byte getTemperature() {
        return temperature;
    }

    public void setTemperature(byte temperature) {
        this.temperature = temperature;
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

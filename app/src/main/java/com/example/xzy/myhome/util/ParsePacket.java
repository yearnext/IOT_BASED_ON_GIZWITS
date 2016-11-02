package com.example.xzy.myhome.util;

import android.util.Log;

import com.gizwits.gizwifisdk.api.GizWifiDevice;

import java.io.Serializable;
import java.util.Calendar;
import java.util.concurrent.ConcurrentHashMap;

import static com.mxchip.helper.ProbeReqData.bytesToHex;

/**
 * Created by xzy on 16/9/18.
 */

public class ParsePacket implements Serializable {
    public ParsePacket() {

    }

    static ConcurrentHashMap<String, Object> dataMap = new ConcurrentHashMap<String, Object>();

    private byte[] packet = new byte[]{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    private byte type;
    private byte eventNumber;
    private byte deviceType;
    private byte[] mac = {0, 0, 0, 0, 0, 0, 0, 0};
    private  byte dataLength;
    private  byte command;
    private  byte[] data = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    public interface TYPE {
        byte APP_REQUEST = 1;
        byte DEVICE_RESPONSE = 2;
        byte DEVICE_REQUEST = 3;
        byte APP_RESPONSE = 4;
    }

    public interface DEVICE_TYPE {
        byte GATEWAY = 0;
        byte LAMP = 1;
        byte SOCKET = 2;
        byte CURTAIN = 3;
        byte SENSOR_TEMPERATURE = 4;
    }

    public interface MAC {

    }

    public interface COMMAND {
        byte RESPONSE = 0;
        byte STATE_READ = 1;
        byte STATE_WRITE = 2;
        byte TIMING_READ = 5;
        byte TIMING_WRITE = 6;
        byte COUNTDOWN_READ = 3;
        byte COUNTDOWN_WRITE = 4;
        byte CURTAIN_STATE_READ = 7;
        byte CURTAIN_STATE_WRITE = 8;
        byte DEVICE_RESPONSE_APP_COUNT = 0;
        byte UPDATE_DEVICE_COUNT = 1;
        byte UPDATE_DEVICE_MESSAGE = 2;


    }

    public interface DATA_LENGTH {

    }

    public interface DATA {

    }




    public ParsePacket(byte[] packet) {
        type = packet[0];
        eventNumber = packet[1];
        deviceType = packet[2];

        for (int i = 3; i < 11; i++) {
            mac[i - 3] = packet[i];
        }

        command = packet[11];
        dataLength = packet[12];

        for (int i = 13; i < 31; i++) {
            data[i - 13] = packet[i];
        }

        checkSum = packet[31];
    }


    byte checkSum;


    public void sendPacket(GizWifiDevice mDevice) {
        packet[0] = type;
        packet[1] = eventNumber;
        packet[2] = deviceType;

        for (int i = 0; i < 8; i++) {
            Log.e("", "sendPacket: "+i );
            packet[3 + i] = mac[i];
        }
        Log.e("Main2Activity", "sendPacket: " + bytesToHex(mac));

        packet[11] = command;
        packet[12] = dataLength;

        for (int i = 0; i < data.length; i++) {
            packet[13 + i] = data[i];
        }

        packet[31] = getCheckSum();
        dataMap.put("Packet", packet);
        mDevice.write(dataMap, 0);
        Log.e("Main2Activity", "sendPacket: " + bytesToHex(packet));

    }

    public void requestDeviceList(GizWifiDevice mDevice, byte count) {
        packet[0] = TYPE.APP_REQUEST;
        packet[2] = DEVICE_TYPE.GATEWAY;
        packet[11] = COMMAND.UPDATE_DEVICE_MESSAGE;
        packet[12] = 1;
        packet[13] = ++count;
        packet[31] = getCheckSum();
        dataMap.put("Packet", packet);
        mDevice.write(dataMap, 0);
        Log.e("Main2Activity", "sendPacket: " + bytesToHex(packet));
    }

    //各种get set方法
    public byte[] getDataMac() {
        byte[] a=new byte[8];
        for (int i = 2; i < 10; i++) {
            a[i - 2] = data[i];
        }
        return a;
    }

    public byte getDataDeviceType() {
        return data[1];
    }
    public byte getDataDeviceCount() {
        return data[0];
    }


    public byte getDataState() {
        return data[0];

    }
    public ParsePacket setDataState(byte dataState) {
        data[0] = dataState;
        return this;

    }

    public ParsePacket setDataCountdown(int hour, int minute) {
        long time =System.currentTimeMillis();
        Calendar calendar = Calendar.getInstance();
        calendar.setTimeInMillis(time);
        int startHour = calendar.get(Calendar.HOUR_OF_DAY);
        int startMinute = calendar.get(Calendar.MINUTE);
        int endMinute = (startMinute + minute) % 60;
        int endHour = (startHour + hour+(startMinute + minute)/60)%24;
        data[0] = 1;
        data[2] = (byte) startHour;
        data[3] = (byte) startMinute;
        data[4] = (byte) endHour;
        data[5] = (byte) endMinute;
        data[7] = (byte) 0;
        //// TODO: 2016/10/30 7  灯状态记录 
        return this;
    }
    public ParsePacket setDataTimeState(byte state) {
        data[6] = state;
        return this;
    }






    public byte getType() {
        return type;
    }

    public ParsePacket setType(byte type) {
        this.type = type;
        return this;
    }

    public byte getEventNumber() {
        return eventNumber;
    }

    public ParsePacket setEventNumber(byte eventNumber) {
        this.eventNumber = eventNumber;
        return this;

    }

    public byte getCommand() {
        return command;
    }

    public ParsePacket setCommand(byte command) {
        this.command = command;
        return this;

    }

    public byte[] getMac() {
        return mac;
    }

    public ParsePacket setMac(byte[] mac) {
        if (mac!=null)
        this.mac = mac;
        return this;

    }

    public byte getDataLength() {
        return dataLength;
    }

    public ParsePacket setDataLength(byte dataLength) {
        this.dataLength = dataLength;
        return this;

    }

    public byte getCheckSum() {
        int sum = 0;
        for (int i = 0; i < 31; i++) {
            sum += (int) packet[i];
        }
        sum = (byte) (sum % 256);
        return (byte) sum;
    }


    public byte[] getData() {
        return data;
    }

    public ParsePacket setData(byte[] data) {
        this.data = data;
        return this;

    }

    public byte getDeviceType() {
        return deviceType;
    }

    public ParsePacket setDeviceType(byte deviceType) {
        this.deviceType = deviceType;
        return this;
    }
    public ParsePacket setDataTiming(byte[] data) {
        for (int i = 0; i < 8; i++) {
            if (i==6) i++;
            this.data[i] = data[i];
        }
        return this;

    }
    public byte getDataTemperature() {
        return data[0];

    }
    public byte getDataHumidity() {
        return data[1];

    }



}

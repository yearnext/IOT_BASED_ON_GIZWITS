package com.example.xzy.myhome.model.bean;

import android.util.Log;

import com.gizwits.gizwifisdk.api.GizWifiDevice;

import java.io.Serializable;
import java.util.Calendar;
import java.util.concurrent.ConcurrentHashMap;

import static android.content.ContentValues.TAG;
import static com.mxchip.helper.ProbeReqData.bytesToHex;

/**
 * Created by xzy on 16/9/18.
 */

public class PacketBean implements Serializable {
    static ConcurrentHashMap<String, Object> dataMap = new ConcurrentHashMap<String, Object>();
    byte checkSum;
    private byte[] packet = new byte[]{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    private byte type;
    private byte eventNumber;
    private byte deviceType;
    private byte[] mac = {0, 0, 0, 0, 0, 0, 0, 0};
    private byte dataLength;
    private byte command;
    private byte[] data = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    public PacketBean() {

    }

    public PacketBean(byte[] packet) {
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

    public static int receiveDataType(PacketBean packetBean) {
        if ((packetBean.getType() == PacketBean.TYPE.DEVICE_RESPONSE ||
                packetBean.getType() == PacketBean.TYPE.DEVICE_REQUEST)
                && packetBean.getDeviceType() == PacketBean.DEVICE_TYPE.GATEWAY) {
            Log.d(TAG, "receiveSucceedData: 收到网关数据，准备更新设备列表");
                return RECEIVE_DEVICE_TYPE.GATEWAY;
        } else if (packetBean.getType() == PacketBean.TYPE.DEVICE_REQUEST) {
            Log.d(TAG, "receiveSucceedData: 收到设备请求，准备更新设备数据");
            return RECEIVE_DEVICE_TYPE.DEVICE;
        } else {
            Log.e(TAG, "receiveSucceedData:通讯代号错误：" + packetBean.getType());
            return RECEIVE_DEVICE_TYPE.UNKNOWN;
        }
    }

    public static byte[] getSystemTime() {
        Calendar _calendar = Calendar.getInstance();
        byte[] _date = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        _date[0]= (byte)_calendar.get(Calendar.YEAR);
        _date[1]= (byte) (_calendar.get(Calendar.MONTH)+1);
        _date[2]= (byte) _calendar.get(Calendar.DAY_OF_MONTH);
        _date[3]= (byte) _calendar.get(Calendar.DAY_OF_WEEK);
        _date[4]= (byte) _calendar.get(Calendar.HOUR_OF_DAY);
        _date[5]= (byte)  _calendar.get(Calendar.MINUTE);
        _date[6]= (byte) _calendar.get(Calendar.SECOND);
        return _date;
    }

    public void sendPacket(GizWifiDevice mDevice) {
        packet[0] = type;
        packet[1] = eventNumber;
        packet[2] = deviceType;

        for (int i = 0; i < 8; i++) {
            Log.e("", "sendPacket: " + i);
            packet[3 + i] = mac[i];
        }
        packet[11] = command;
        packet[12] = dataLength;

        for (int i = 0; i < data.length; i++) {
            packet[13 + i] = data[i];
        }

        packet[31] = getCheckSum();
        dataMap.put("Packet", packet);
        mDevice.write(dataMap, 0);
        Log.e("发送数据", bytesToHex(packet));

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
        Log.e("发送数据", bytesToHex(packet));
    }

    //各种get set方法
    public byte[] getDataMac() {
        byte[] a = new byte[8];
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

    public PacketBean setDataState(byte dataState) {
        data[0] = dataState;
        return this;

    }

    public PacketBean setDataCountdown(int hour, int minute) {
        long time = System.currentTimeMillis();
        Calendar calendar = Calendar.getInstance();
        calendar.setTimeInMillis(time);
        int startHour = calendar.get(Calendar.HOUR_OF_DAY);
        int startMinute = calendar.get(Calendar.MINUTE);
        int endMinute = (startMinute + minute) % 60;
        int endHour = (startHour + hour + (startMinute + minute) / 60) % 24;
        data[0] = 1;
        data[2] = (byte) startHour;
        data[3] = (byte) startMinute;
        data[4] = (byte) endHour;
        data[5] = (byte) endMinute;
        data[7] = (byte) 0;
        //// TODO: 2016/10/30 7  灯状态记录
        return this;
    }

    public PacketBean setDataTimeState(byte state) {
        data[6] = state;
        return this;
    }

    public byte getType() {
        return type;
    }

    public PacketBean setType(byte type) {
        this.type = type;
        return this;
    }

    public byte getEventNumber() {
        return eventNumber;
    }

    public PacketBean setEventNumber(byte eventNumber) {
        this.eventNumber = eventNumber;
        return this;

    }

    public byte getCommand() {
        return command;
    }

    public PacketBean setCommand(byte command) {
        this.command = command;
        return this;

    }

    public byte[] getMac() {
        return mac;
    }

    public PacketBean setMac(byte[] mac) {
        if (mac != null)
            this.mac = mac;
        return this;

    }

    public byte getDataLength() {
        return dataLength;
    }

    public PacketBean setDataLength(byte dataLength) {
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

    public PacketBean setData(byte[] data) {
        this.data = data;
        return this;

    }

    public byte getDeviceType() {
        return deviceType;
    }

    public PacketBean setDeviceType(byte deviceType) {
        this.deviceType = deviceType;
        return this;
    }

    public PacketBean setDataTiming(byte[] data) {
        for (int i = 0; i < 8; i++) {
            if (i == 6) i++;
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

    public static void updateDeviceTime(byte[] mac, byte deviceType, GizWifiDevice device) {
        PacketBean _packetBean = new PacketBean();
        _packetBean.setType(TYPE.APP_RESPONSE)
                .setDeviceType(deviceType)
                .setMac(mac)
                .setCommand(COMMAND.TIME_WRITE)
                .setDataLength(DATA_LENGTH.TIME)
                .setData(getSystemTime())
                .sendPacket(device);

    }


    public interface TYPE {
        byte APP_REQUEST = 0x1;
        byte DEVICE_RESPONSE = 0x2;
        byte DEVICE_REQUEST = 0x3;
        byte APP_RESPONSE = 0x4;
    }

    public interface DEVICE_TYPE {
        byte GATEWAY = 0x0;
        byte LAMP = 0x1;
        byte SOCKET = 0x2;
        byte CURTAIN = 0x3;
        byte SENSOR_TEMPERATURE = 0x4;
    }

    public interface COMMAND {
        byte RESPONSE = 0;
        byte TIME_READ = 0x04;
        byte TIME_WRITE = 0x05;
        byte STATE_READ = 0x10;
        byte STATE_WRITE = 0x11;
        byte TIMING_READ = 0x14;
        byte TIMING_WRITE = 0x15;
        byte COUNTDOWN_READ = 0x12;
        byte COUNTDOWN_WRITE = 0x13;
        byte CURTAIN_STATE_READ = 0x7;
        byte CURTAIN_STATE_WRITE = 0x8;
        byte DEVICE_RESPONSE_APP_COUNT = 0x10;
        byte UPDATE_DEVICE_COUNT = 0x11;
        byte UPDATE_DEVICE_MESSAGE = 0x12;
    }


    public interface RECEIVE_DEVICE_TYPE {
        byte GATEWAY = 0x1;
        byte DEVICE = 0x2;
        byte UNKNOWN = 0x3;
        byte TIME = 0x4;

    }
    public interface DATA_LENGTH{
        byte TIME = 0x7;
    }
}


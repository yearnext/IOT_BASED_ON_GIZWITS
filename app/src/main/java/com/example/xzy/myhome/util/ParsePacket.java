package com.example.xzy.myhome.util;

import com.gizwits.gizwifisdk.api.GizWifiDevice;

import java.io.Serializable;
import java.util.concurrent.ConcurrentHashMap;

/**
 * Created by xzy on 16/9/18.
 */

public class ParsePacket implements Serializable {
    static ConcurrentHashMap<String, Object> dataMap = new ConcurrentHashMap<String, Object>();


    private byte[] parket = new byte[]{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


    public interface TYPE {
        byte END = 0;//结束此次通信流程
        byte APP_READ = 1;  //APP读取数据等待应答
        byte DEVICE_RESPONSE_APP_READ = 2;  //设备回应APP读取数据
        byte APP_WRITE = 3;  //设备请求与app通信
        byte DEVICE_RESPONSE_APP_WRITE = 4;  //app应答设备
        byte DEVICE_WRITE = 7;  //
        byte APP_RESPONSE_DEVICE_WRITE = 8;  //
        byte CHECK_ERROR = 9;
    }

    public interface DEVICE_TYPE {
        byte GATEWAY = 0;
        byte LAMP = 1;
        byte SOCKET = 2;
        byte CURTAIN= 3;
        byte SENSOR_TEMPERATURE= 4;

    }

    public interface MAC {
        byte[] LAMP = {0x20, 0x01};
        byte[] SOCKET = {0x20, 0x02,};
        byte[] CURTAIN = {0x20, 0x03};
    }

    public interface COMMAND {
        byte SWITCH = 1;
        byte TIMING = 2;
        byte COUNTDOWN = 3;
    }

    public interface DATALENGTH {
        byte SWITCH = 2;
        byte TIMING = 9;
        byte COUNTDOWN = 5;
    }

    public interface DATA {
        byte[] OFF = {0};
        byte[] ON = {1};
    }


    public ParsePacket() {

    }

    public ParsePacket(byte[] packet) {
        type = packet[0];
        eventNumber = packet[1];
        for (int i = 2; i < 8; i++) {
            mac[i - 2] = packet[i];
        }
        for (int i = 10; i < 31; i++) {
            data[i - 10] = packet[i];
        }
        dataLength = packet[8];
        command = packet[9];

        checkSum = parket[31];
    }

    byte type;
    byte eventNumber;
    byte[] mac = {0, 0, 0, 0, 0, 0};
    byte dataLength;
    byte command;
    byte[] data = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    byte checkSum;

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
        return checkSum;
    }

    public ParsePacket setCheckSum(byte checkSum) {
        this.checkSum = checkSum;
        return this;

    }

    public byte[] getData() {
        return data;
    }

    public ParsePacket setData(byte[] data) {
        this.data = data;
        return this;

    }

    public void sendPacket(GizWifiDevice mDevice) {
        parket[0] = type;
        parket[1] = eventNumber;
        for (int i = 0; i < mac.length; i++) {
            parket[2 + i] = mac[i];
        }

        parket[8] = dataLength;
        parket[9] = command;
        for (int i = 0; i < data.length; i++) {
            parket[10 + i] = data[i];
        }

        int sum = 0;
        for (int i = 0; i < 31; i++) {
            sum += (int) parket[i];
        }

        parket[31] = (byte) (sum % 256);
        dataMap.put("Packet", parket);
        mDevice.write(dataMap, 0);
    }

    public void sendPacket(GizWifiDevice mDevice, byte type, byte eventNumber, byte[] mac, byte dataLength, byte command, byte[] data) {
        parket[0] = type;
        parket[1] = eventNumber;
        for (int i = 0; i < mac.length; i++) {
            parket[2 + i] = mac[i];
        }
        parket[8] = dataLength;
        parket[9] = command;
        for (int i = 0; i < data.length; i++) {
            parket[10 + i] = data[i];
        }
        int sum = 0;
        for (int i = 0; i < 31; i++) {
            sum += (int) parket[i];
        }

        parket[31] = (byte) (sum % 256);
        dataMap.put("Packet", parket);
        mDevice.write(dataMap, 0);
    }




    /*public byte[] getParket() {
        parket[0] = type;
        parket[1] = eventNumber;
        for (int i = 0; i < mac.length; i++) {
            parket[2 + i] = mac[i];
        }
        for (int i = 0; i < data.length; i++) {
            parket[10 + i] = data[i];
        }
        parket[8] = dataLength;
        parket[9] = command;

        parket[31] = checkSum;
        return parket;
    }

    public void sendParket(GizWifiDevice mDevice) {
        dataMap.put("Packet", parket);
        mDevice.write(dataMap, 0);
    }*/

}

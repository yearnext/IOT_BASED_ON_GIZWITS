package com.example.xzy.myhome.util;

/**
 * Created by xzy on 16/9/18.
 */

public class ParsePacket {
    private byte[] parket = new byte[]{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    public interface TYPE {
        byte END = 0;//结束此次通信流程
        byte APP_READ = 1;  //APP读取数据等待应答
        byte DEVICE_RESPONSE_APP_READ = 2;  //设备回应APP读取数据
        byte APP_WRITE = 3;  //App写入数据等待设备应答
        byte DEVICE_RESPONSE_APP_WRITE = 4;  //
        byte DEVICE_WRITE = 7;  //
        byte APP_RESPONSE_DEVICE_WRITE = 8;  //
        byte CHECK_ERROR =9 ;
    }

    public ParsePacket(byte[] parket) {
        type = parket[0];
        eventNumber = parket[1];
        for (int i = 2; i < 8; i++) {
            mac[i - 2] = parket[i];
        }
        for (int i = 10; i < 31; i++) {
            data[i - 10] = parket[i];
        }
        dataLength = parket[8];
        command = parket[9];

        checkSum = parket[31];
    }

    byte type;
    byte eventNumber;
    byte[] mac;
    byte dataLength;
    byte command;
    byte[] data;
    byte checkSum;

    public byte getType() {
        return type;
    }

    public void setType(byte type) {
        this.type = type;
    }

    public byte getEventNumber() {
        return eventNumber;
    }

    public void setEventNumber(byte eventNumber) {
        this.eventNumber = eventNumber;
    }

    public byte getCommand() {
        return command;
    }

    public void setCommand(byte command) {
        this.command = command;
    }

    public byte[] getMac() {
        return mac;
    }

    public void setMac(byte[] mac) {
        this.mac = mac;
    }

    public byte getDataLength() {
        return dataLength;
    }

    public void setDataLength(byte dataLength) {
        this.dataLength = dataLength;
    }

    public byte getChecksum() {
        return checkSum;
    }

    public void setChecksum(byte checkSum) {
        this.checkSum = checkSum;
    }

    public byte[] getData() {
        return data;
    }

    public void setData(byte[] data) {
        this.data = data;
    }

    public byte[] getParket() {
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

}

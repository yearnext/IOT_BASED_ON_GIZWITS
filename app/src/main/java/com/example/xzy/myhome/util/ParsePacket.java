package com.example.xzy.myhome.util;

/**
 * Created by xzy on 16/9/18.
 */

public class ParsePacket {
    public byte type;
    public interface TYPE{
        char END = 0x00;//结束此次通信流程
        char APPREAD = 0x01;  //APP读取数据等待应答
        char DEVICERESPONSE = 0x02;  //设备回应APP读取数据
        char APPWRITE = 0x03;  //App写入数据等待设备应答
        char q = 0x04;  //
        char d1 = 0x05;  //
        char d = 0x06;  //
        char CHECKERROR = 0x09;

    }
    byte eventNumber;
    byte mac;
    byte dataLength;
    byte command;
    byte data;
    byte checksum;

    /*public ParsePacket(byte[] packet) {
        type = packet[0];
        eventNumber = packet[1];
        mac= packet[1];
        dataLength= packet[1];
        command= packet[1];
        data= packet[1];
        checksum= packet[1];

    }*/
}

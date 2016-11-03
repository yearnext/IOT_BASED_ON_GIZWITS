/**
************************************************************
* @file         gizwits_protocol.c
* @brief        Gizwits协议相关文件 (SDK API 接口函数定义)
* @author       Gizwits
* @date         2016-09-05
* @version      V03010101
* @copyright    Gizwits
* 
* @note         机智云.只为智能硬件而生
*               Gizwits Smart Cloud  for Smart Products
*               链接|增值ֵ|开放|中立|安全|自有|自由|生态
*               www.gizwits.com
*
***********************************************************/
#include "myprotocol.h"
#include <string.h>
#include "gizwits_product.h"
#include "gizwits_protocol.h"
#include "Onboard.h"

#if (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_COORD)
/** 协议全局变量 **/
gizwitsProtocol_t gizwitsProtocol;

/**@name 串口接收环形缓冲区实现
* @{
*/
static rb_t pRb;                                               ///< 环形缓冲区结构体变量
static uint8 rbBuf[RB_MAX_LEN];                       ///< 环形缓冲区数据缓存区

static void rbCreate(rb_t* rb)
{
    if(NULL == rb)
    {
        return;
    }

    rb->rbHead = rb->rbBuff;
    rb->rbTail = rb->rbBuff;
}

//static void rbDelete(rb_t* rb)
//{
//    if(NULL == rb)
//    {
//        return;
//    }
//
//    rb->rbBuff = NULL;
//    rb->rbHead = NULL;
//    rb->rbTail = NULL;
//    rb->rbCapacity = 0;
//}

static int32 rbCapacity(rb_t *rb)
{
    if(NULL == rb)
    {
        return -1;
    }

    return rb->rbCapacity;
}

static int32 rbCanRead(rb_t *rb)
{
    if(NULL == rb)
    {
        return -1;
    }

    if (rb->rbHead == rb->rbTail)
    {
        return 0;
    }

    if (rb->rbHead < rb->rbTail)
    {
        return rb->rbTail - rb->rbHead;
    }

    return rbCapacity(rb) - (rb->rbHead - rb->rbTail);
}

static int32 rbCanWrite(rb_t *rb)
{
    if(NULL == rb)
    {
        return -1;
    }

    return rbCapacity(rb) - rbCanRead(rb);
}

static int32 rbRead(rb_t *rb, void *data, size_t count)
{
    int copySz = 0;

    if(NULL == rb)
    {
        return -1;
    }

    if(NULL == data)
    {
        return -1;
    }

    if (rb->rbHead < rb->rbTail)
    {
        copySz = min(count, rbCanRead(rb));
        memcpy(data, rb->rbHead, copySz);
        rb->rbHead += copySz;
        return copySz;
    }
    else
    {
        if (count < rbCapacity(rb)-(rb->rbHead - rb->rbBuff))
        {
            copySz = count;
            memcpy(data, rb->rbHead, copySz);
            rb->rbHead += copySz;
            return copySz;
        }
        else
        {
            copySz = rbCapacity(rb) - (rb->rbHead - rb->rbBuff);
            memcpy(data, rb->rbHead, copySz);
            rb->rbHead = rb->rbBuff;
            copySz += rbRead(rb, (char*)data+copySz, count-copySz);
            return copySz;
        }
    }
}

static int32 rbWrite(rb_t *rb, const void *data, size_t count)
{
    int tailAvailSz = 0;

    if(NULL == rb)
    {
        return -1;
    }

    if(NULL == data)
    {
        return -1;
    }

    if (count >= rbCanWrite(rb))
    {
        return -1;
    }

    if (rb->rbHead <= rb->rbTail)
    {
        tailAvailSz = rbCapacity(rb) - (rb->rbTail - rb->rbBuff);
        if (count <= tailAvailSz)
        {
            memcpy(rb->rbTail, data, count);
            rb->rbTail += count;
            if (rb->rbTail == rb->rbBuff+rbCapacity(rb))
            {
                rb->rbTail = rb->rbBuff;
            }
            return count;
        }
        else
        {
            memcpy(rb->rbTail, data, tailAvailSz);
            rb->rbTail = rb->rbBuff;

            return tailAvailSz + rbWrite(rb, (char*)data+tailAvailSz, count-tailAvailSz);
        }
    }
    else
    {
        memcpy(rb->rbTail, data, count);
        rb->rbTail += count;
        return count;
    }
}
/**@} */

/**
* @brief 向环形缓冲区写入数据
* @param [in] buf        : buf地址
* @param [in] len        : 字节长度
* @return   正确 : 返回写入的数据长度
            失败 : -1
*/
int32 gizPutData(uint8 *buf, uint32 len)
{
    int32 count = 0;

    if(NULL == buf)
    {
        return -1;
    }

    count = rbWrite(&pRb, buf, len);
    if(count != len)
    {
        return -1;
    }

    return count;
}

/**
* @brief 系统毫秒计时维护函数,毫秒自增,溢出归零

* @param none
* @return none
*/
void gizTimerMs(void)
{
    gizwitsProtocol.timerMsCount++;
}

void gizTimer50Ms(void)
{
    gizwitsProtocol.timerMsCount++;
}

/**
* @brief 读取系统时间毫秒计时数

* @param none
* @return 系统时间毫秒数
*/
uint32 gizGetTimerCount(void)
{
    return gizwitsProtocol.timerMsCount;
}

/**
* @brief 报文数据校验和计算
*
* @param [in] buf   : buf地址
* @param [in] len   : 字节长度
*
* @return sum : 从缓冲区第3个字节后所有的字节累加求和
*/
static uint8 gizProtocolSum(uint8 *buf, uint32 len)
{
    uint8     sum = 0;
    uint32    i = 0;

    if(buf == NULL || len <= 0)
    {
        return 0;
    }

    for(i=2; i<len-1; i++)
    {
        sum += buf[i];
    }

    return sum;
}

/**
* @brief 16位数据字节序转换
*
* @param [in] value : 需要转换的数据
*
* @return  tmp_value: 转换后的数据
*/
static int16 gizProtocolExchangeBytes(int16 value)
{
    int16    tmp_value;
    uint8     *index_1, *index_2;

    index_1 = (uint8 *)&tmp_value;
    index_2 = (uint8 *)&value;

    *index_1 = *(index_2+1);
    *(index_1+1) = *index_2;

    return tmp_value;
}

///**
//* @brief 32位数据字节序转换
//*
//* @param [in] value : 需要转换的数据
//*
//* @return  tmp_value: 转换后的数据
//*/
//static uint32 gizExchangeWord(uint32  value)
//{
//    return ((value & 0x000000FF) << 24) |
//        ((value & 0x0000FF00) << 8) |
//        ((value & 0x00FF0000) >> 8) |
//        ((value & 0xFF000000) >> 24) ;
//}

/**
* @brief 数组缓冲区网络字节序转换
*
* @param [in] buf      : buf地址
* @param [in] dataLen  : 字节长度
*
* @return 正确 : 0 
          失败 : -1
*/
static int8 gizByteOrderExchange(uint8 *buf,uint32 dataLen)
{
    uint32 i = 0;
    uint8 preData = 0;
    uint8 aftData = 0;

    if(NULL == buf)
    {
//        GIZWITS_LOG("gizByteOrderExchange Error , Illegal Param\n");
        return -1;
    }

    for(i = 0;i<dataLen/2;i++)
    {
        preData = buf[i];
        aftData = buf[dataLen - i - 1];
        buf[i] = aftData;
        buf[dataLen - i - 1] = preData;
    }
    return 0;
}
//
///**
//* @brief 数据点跨字节判断
//*
//* @param [in] bitOffset     : 位偏移
//* @param [in] bitLen        : 占用位长度 
//*
//* @return 未跨字节 : 0 
//            跨字节 : 1
//*/
//static uint8 gizAcrossByteJudge(uint32 bitOffset,uint32 bitLen)
//{
//    if((0 == bitOffset)||(0 == bitOffset%8))
//    {
//        if(bitLen <= 8)
//        {
//            return 0;
//        }
//        else
//        {
//            return 1;
//        }
//    }
//    else
//    {
//        if(8 - bitOffset%8 >= bitLen)
//        {
//            return 0;
//        }
//        else
//        {
//            return 1; 
//        }
//    }
//}

///**
//* @brief bool和enum类型数据点数据压缩
//*
//* @param [in] byteOffset    : 字节偏移
//* @param [in] bitOffset     : 位偏移
//* @param [in] bitLen        : 占用位长度
//* @param [out] arrayAddr    : 数组地址
//* @param [in] srcData       : 原始数据
//*
//* @return : 0,正确返回;-1，错误返回
//*/
//static int32 gizCompressValue(uint32 byteOffset,uint32 bitOffset,uint32 bitLen,uint8 *bufAddr,uint32 srcData)
//{
//    uint8 highBit = 0;
//    uint8 lowBit = 0;
//    uint8 ret = 0;
//
//    if(NULL == bufAddr)
//    {
////        GIZWITS_LOG("gizCompressValue Error , Illegal Param\n");
//        return -1;
//    }
//
//    ret = gizAcrossByteJudge(bitOffset,bitLen);
//    if(0 == ret)
//    {
//        bufAddr[byteOffset] |= (((uint8)srcData)<<(bitOffset%8));
//    }
//    else if(1 == ret)
//    {
//        /* 暂时支持最多跨两字节的压缩 */ 
//        highBit = ((uint8)srcData)>>(8-bitOffset%8);
//        lowBit = (uint8)srcData & (0xFF >> (8-bitOffset%8));
//        bufAddr[byteOffset + 1] |=  highBit;
//        bufAddr[byteOffset] |= (lowBit<<(bitOffset%8));
//    }
//
//    return 0;
//////}

/**
* @brief 从环形缓冲区中抓取一包数据
*
* @param [in]  rb                  : 输入数据地址
* @param [out] data                : 输出数据地址
* @param [out] len                 : 输出数据长度
*
* @return : 0,正确返回;-1，错误返回;-2，数据校验失败
*/
static int8 gizProtocolGetOnePacket(rb_t *rb, uint8 *data, uint16 *len)
{
    uint8 ret = 0;
    uint8 sum = 0;
    uint8 i = 0;
    uint8 tmpData;
    uint8 tmpLen = 0;
    uint16 tmpCount = 0;
    static uint8 protocolFlag = 0;
    static uint16 protocolCount = 0;
    static uint8 lastData = 0;
    static uint8 debugCount = 0;
    uint8 *protocolBuff = data;
    protocolHead_t *head = NULL;

    if((NULL == rb) || (NULL == data) ||(NULL == len))
    {
        return -1;
    }

    tmpLen = rbCanRead(rb);
    if(0 == tmpLen)
    {
        return -1;
    }

    for(i=0; i<tmpLen; i++)
    {
        ret = rbRead(rb, &tmpData, 1);
        if(0 != ret)
        {
            if((0xFF == lastData) && (0xFF == tmpData))
            {
                if(0 == protocolFlag)
                {
                    protocolBuff[0] = 0xFF;
                    protocolBuff[1] = 0xFF;
                    protocolCount = 2;
                    protocolFlag = 1;
                }
                else
                {
                    if((protocolCount > 4) && (protocolCount != tmpCount))
                    {
                        protocolBuff[0] = 0xFF;
                        protocolBuff[1] = 0xFF;
                        protocolCount = 2;
                    }
                }
            }
            else if((0xFF == lastData) && (0x55 == tmpData))
            {
            }
            else
            {
                if(1 == protocolFlag)
                {
                    protocolBuff[protocolCount] = tmpData;
                    protocolCount++;

                    if(protocolCount > 4)
                    {
                        head = (protocolHead_t *)protocolBuff;
                        tmpCount = gizProtocolExchangeBytes(head->len)+4;
                        if(protocolCount == tmpCount)
                        {
                            break;
                        }
                    }
                }
            }

            lastData = tmpData;
            debugCount++;
        }
    }

    if((protocolCount > 4) && (protocolCount == tmpCount))
    {
        sum = gizProtocolSum(protocolBuff, protocolCount);

        if(protocolBuff[protocolCount-1] == sum)
        {
            memcpy(data, protocolBuff, tmpCount);
            *len = tmpCount;
            protocolFlag = 0;

            protocolCount = 0;
            debugCount = 0;
            lastData = 0;

            return 0;
        }
        else
        {
            return -2;
        }
    }

    return 1;
}

/**
* @brief 根据协议生成“控制型事件”

* @param [in] issuedData  ：控制型数据
* @param [out] info       ：事件队列
* @param [out] dataPoints ：数据点数据
* @return 0，执行成功， 非 0，失败
*/
static int8 gizDataPoint2Event(gizwitsIssued_t *issuedData, eventInfo_t *info, dataPoint_t *dataPoints)
{
    if((NULL == issuedData) || (NULL == info) ||(NULL == dataPoints))
    {
//        GIZWITS_LOG("gizDataPoint2Event Error , Illegal Param\n");
        return -1;
    }
    /** 大于1字节做位序转换 **/
    if(sizeof(issuedData->attrFlags) > 1)
    {
        if(-1 == gizByteOrderExchange((uint8 *)&issuedData->attrFlags,sizeof(attrFlags_t)))
        {
//            GIZWITS_LOG("gizByteOrderExchange Error\n");
            return -1;
        }
    }


    if(0x01 == issuedData->attrFlags.flagPacket)
    {
        info->event[info->num] = EVENT_PACKET;
        info->num++;
        memcpy((uint8 *)dataPoints->valuePacket,issuedData->attrVals.valuePacket,sizeof(issuedData->attrVals.valuePacket));
    }

    return 0;
}

///**
//* @brief 对比当前数据和上次数据
//*
//* @param [in] cur        : 当前数据点数据
//* @param [in] last       : 上次数据点数据
//*
//* @return : 0,数据无变化;1，数据有变化
//*/
//static int8 gizCheckReport(dataPoint_t *cur, dataPoint_t *last)
//{
//    int8 ret = 0;
////    static uint32 lastReportTime = 0;
//
//    if((NULL == cur) || (NULL == last))
//    {
//        return -1;
//    }
//    if(0 != memcmp((uint8 *)&last->valuePacket,(uint8 *)&cur->valuePacket,sizeof(last->valuePacket)))
//    {
//        ret = 1;
//    }
//
//
//    return ret;
//}

/**
* @brief 用户数据点数据转换为机智云上报数据点数据
*
* @param [in]  dataPoints           : 用户数据点数据地址
* @param [out] devStatusPtr         : 机智云上报数据点数据地址
*
* @return 0,正确返回;-1，错误返回
*/
static int8 gizDataPoints2ReportData(dataPoint_t *dataPoints , devStatus_t *devStatusPtr)
{
    if((NULL == dataPoints) || (NULL == devStatusPtr))
    {
//        GIZWITS_LOG("gizDataPoints2ReportData Error , Illegal Param\n");
        return -1;
    }

    memcpy((uint8 *)devStatusPtr->valuePacket,(uint8 *)&dataPoints->valuePacket,sizeof(dataPoints->valuePacket));
    return 0;
}

/**
* @brief 协议头初始化
*
* @param [out] head         : 协议头指针
*
* @return 0， 执行成功， 非 0， 失败    
*/
static int8 gizProtocolHeadInit(protocolHead_t *head)
{
    if(NULL == head)
    {
//        GIZWITS_LOG("ERROR: gizProtocolHeadInit head is empty \n");
        return -1;
    }

    memset((uint8 *)head, 0, sizeof(protocolHead_t));
    head->head[0] = 0xFF;
    head->head[1] = 0xFF;

    return 0;
}

/**
* @brief 协议ACK校验处理函数
*
* @param [in] data            : 数据地址
* @param [in] len             : 数据长度
*
* @return 0， 执行成功， 非 0， 失败
*/
static int8 gizProtocolWaitAck(uint8 *data, uint32 len)
{
    if(NULL == data)
    {
//        GIZWITS_LOG("ERROR: data is empty \n");
        return -1;
    }

    memset((uint8 *)&gizwitsProtocol.waitAck, 0, sizeof(protocolWaitAck_t));
    memcpy((uint8 *)gizwitsProtocol.waitAck.buf, data, len);
    gizwitsProtocol.waitAck.dataLen = (int16)len;
    
    gizwitsProtocol.waitAck.flag = 1;
    gizwitsProtocol.waitAck.sendTime = gizGetTimerCount();

    return 0;
}

/**
* @brief 协议数据重发

* 校验超时且满足重发次数限制的协议数据进行重发

* @param none    
*
* @return none
*/
static void gizProtocolResendData(void)
{
    if(0 == gizwitsProtocol.waitAck.flag)
    {
        return;
    }

    GIZWITS_UART_WRITE(gizwitsProtocol.waitAck.buf, gizwitsProtocol.waitAck.dataLen);

    gizwitsProtocol.waitAck.sendTime = gizGetTimerCount();
}

/**
* @brief 清除ACK协议报文
*
* @param [in] head : 协议头地址
*
* @return 0， 执行成功， 非 0， 失败
*/
static int8 gizProtocolWaitAckCheck(protocolHead_t *head)
{
    protocolHead_t *waitAckHead = (protocolHead_t *)gizwitsProtocol.waitAck.buf;

    if(NULL == head)
    {
//        GIZWITS_LOG("ERROR: data is empty \n");
        return -1;
    }

    if(waitAckHead->cmd+1 == head->cmd)
    {
        memset((uint8 *)&gizwitsProtocol.waitAck, 0, sizeof(protocolWaitAck_t));
    }

    return 0;
}

/**
* @brief 发送通用协议报文数据
* 
* @param [in] head              : 协议头指针
*
* @return : 有效数据长度,正确返回;-1，错误返回
*/
static int32 gizProtocolCommonAck(protocolHead_t *head)
{
    protocolCommon_t ack;

    if(NULL == head)
    {
//        GIZWITS_LOG("ERROR: gizProtocolCommonAck data is empty \n");
        return -1;
    }
    memcpy((uint8 *)&ack, (uint8 *)head, sizeof(protocolHead_t));
    ack.head.cmd = ack.head.cmd+1;
    ack.head.len = gizProtocolExchangeBytes(sizeof(protocolCommon_t)-4);
    ack.sum = gizProtocolSum((uint8 *)&ack, sizeof(protocolCommon_t));

    GIZWITS_UART_WRITE((uint8 *)&ack, sizeof(protocolCommon_t));

    return sizeof(protocolCommon_t);
}

/**
* @brief ACK逻辑处理函数

* 发送后的协议数据进行超时判断，超时200ms进行重发，重发上限为三次

* @param none 
*
* @return none
*/
static void gizProtocolAckHandle(void)
{
    if(1 == gizwitsProtocol.waitAck.flag)
    {
        if(SEND_MAX_NUM >= gizwitsProtocol.waitAck.num)
        {
            //300ms未收到ACK重发
            if(SEND_MAX_TIME < (gizGetTimerCount() - gizwitsProtocol.waitAck.sendTime))
            {
//                GIZWITS_LOG("Warning:gizProtocolResendData %d %d %d\n", gizGetTimerCount(), gizwitsProtocol.waitAck.sendTime, gizwitsProtocol.waitAck.num);
                gizProtocolResendData();
                gizwitsProtocol.waitAck.num++;
            }
        }
        else
        {
            memset((uint8 *)&gizwitsProtocol.waitAck, 0, sizeof(protocolWaitAck_t));
        }
    }
}

/**
* @brief 协议4.1 WiFi模组请求设备信息
*
* @param head : 待发送的协议报文数据
*
* @return 返回有效数据长度; -1，错误返回
*/
static int32 gizProtocolGetDeviceInfo(protocolHead_t * head)
{
    protocolDeviceInfo_t deviceInfo;

    if(NULL == head)
    {
//        GIZWITS_LOG("gizProtocolGetDeviceInfo Error , Illegal Param\n");
        return -1;
    }

    gizProtocolHeadInit((protocolHead_t *)&deviceInfo);
    deviceInfo.head.cmd = ACK_GET_DEVICE_INFO;
    deviceInfo.head.sn = head->sn;
    memcpy((uint8 *)deviceInfo.protocolVer, protocol_VERSION, 8);
    memcpy((uint8 *)deviceInfo.p0Ver, P0_VERSION, 8);
    memcpy((uint8 *)deviceInfo.softVer, SOFTWARE_VERSION, 8);
    memcpy((uint8 *)deviceInfo.hardVer, HARDWARE_VERSION, 8);
    memcpy((uint8 *)deviceInfo.productKey, PRODUCT_KEY, 32);
    memcpy((uint8 *)deviceInfo.devAttr, DEV_DEFAULT, 8);
    deviceInfo.ninableTime = 0;
    deviceInfo.head.len = gizProtocolExchangeBytes(sizeof(protocolDeviceInfo_t)-4);
    deviceInfo.sum = gizProtocolSum((uint8 *)&deviceInfo, sizeof(protocolDeviceInfo_t));

    GIZWITS_UART_WRITE((uint8 *)&deviceInfo, sizeof(protocolDeviceInfo_t));

    return sizeof(protocolDeviceInfo_t);
}

/**
* @brief 协议4.7 非法消息通知 的处理

* @param[in] head  : 协议头指针
* @param[in] errno : 非法消息通知类型
* @return 0， 执行成功， 非 0， 失败
*/
static int32 gizProtocolErrorCmd(protocolHead_t *head,errorPacketsType_t errno)
{
    protocolErrorType_t errorType;

    if(NULL == head)
    {
//        GIZWITS_LOG("gizProtocolErrorCmd Error , Illegal Param\n");
        return -1;
    }
    gizProtocolHeadInit((protocolHead_t *)&errorType);
    errorType.head.cmd = ACK_ERROR_PACKAGE;
    errorType.head.sn = head->sn;
    
    errorType.head.len = gizProtocolExchangeBytes(sizeof(protocolErrorType_t)-4);
    errorType.error = errno;
    errorType.sum = gizProtocolSum((uint8 *)&errorType, sizeof(protocolErrorType_t));
    
    GIZWITS_UART_WRITE((uint8 *)&errorType, sizeof(protocolErrorType_t));

    return sizeof(protocolErrorType_t);
}

/**
* @brief 协议4.4 设备MCU重置WiFi模组 的相关操作

* @param none
* @return none
*/
/**@} */

/**
* @brief MCU复位函数

* @param none
* @return none
*/
static void mcuRestart(void)
{
    Onboard_soft_reset();
}

/**@} */

static void gizProtocolReboot(void)
{
    uint32 timeDelay = gizGetTimerCount();
    /*Wait 600ms*/
    while((gizGetTimerCount() - timeDelay) <= 12);
    mcuRestart();
}

/**
* @brief 协议 4.5 WiFi模组向设备MCU通知WiFi模组工作状态的变化

* @param[in] status WiFi模组工作状态
* @return none
*/
static int8 gizProtocolModuleStatus(protocolWifiStatus_t *status)
{
    if(NULL == status)
    {
        GIZWITS_LOG("gizProtocolModuleStatus Error , Illegal Param\r\n");
        return -1;
    }

    status->ststus.value = gizProtocolExchangeBytes(status->ststus.value);
   
    //OnBoarding mode status
    if(1 == status->ststus.types.onboarding)
    {
        if(1 == status->ststus.types.softap)
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_SOFTAP;
            gizwitsProtocol.wifiStatusEvent.num++;
            GIZWITS_LOG("on softap \n");
        }

        if(1 == status->ststus.types.station)
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_AIRLINK;
            gizwitsProtocol.wifiStatusEvent.num++;
            GIZWITS_LOG("on AirLink \n");
        }
    }
    else
    {
        if(1 == status->ststus.types.softap)
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_SOFTAP;
            gizwitsProtocol.wifiStatusEvent.num++;
            GIZWITS_LOG("on softap \n");
        }

        if(1 == status->ststus.types.station)
        {
            gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_STATION;
            gizwitsProtocol.wifiStatusEvent.num++;
            GIZWITS_LOG("on AirLink \n");
        }
    }

    //binding mode status
    if(1 == status->ststus.types.binding)
    {
        gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_OPEN_BINDING;
        gizwitsProtocol.wifiStatusEvent.num++;
        GIZWITS_LOG("in binding \n");
    }
    else
    {
        gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_CLOSE_BINDING;
        gizwitsProtocol.wifiStatusEvent.num++;
        GIZWITS_LOG("out binding \n");
    }

    //router status
    if(1 == status->ststus.types.con_route)
    {
        gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_CON_ROUTER;
        gizwitsProtocol.wifiStatusEvent.num++;
        GIZWITS_LOG("connected router \n");
    }
    else
    {
        gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_DISCON_ROUTER;
        gizwitsProtocol.wifiStatusEvent.num++;
        GIZWITS_LOG("disconnected router \n");
        
//        gizwitsSetMode(0x02);
//        GIZWITS_LOG("set airlink mode! \n");
        
    }

    //M2M server status
    if(1 == status->ststus.types.con_m2m)
    {
        gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_CON_M2M;
        gizwitsProtocol.wifiStatusEvent.num++;
        GIZWITS_LOG("connected m2m \n");
    }
    else
    {
        gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_DISCON_M2M;
        gizwitsProtocol.wifiStatusEvent.num++;
        GIZWITS_LOG("disconnected m2m \n");
    }

    //APP status
    if(1 == status->ststus.types.app)
    {
        gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_CON_APP;
        gizwitsProtocol.wifiStatusEvent.num++;
        gizwitsProtocol.app_connt_status = true;
        GIZWITS_LOG("app connect \n");
    }
    else
    {
        gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_DISCON_APP;
        gizwitsProtocol.wifiStatusEvent.num++;
        gizwitsProtocol.app_connt_status = false;
        GIZWITS_LOG("no app connect \n");
    }

//    //test mode status
//    if(1 == status->ststus.types.test)
//    {
//        gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_OPEN_TESTMODE;
//        gizwitsProtocol.wifiStatusEvent.num++;
////        GIZWITS_LOG("WiFi status: in test mode\r\n");
//    }
//    else
//    {
//        gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_CLOSE_TESTMODE;
//        gizwitsProtocol.wifiStatusEvent.num++;
////        GIZWITS_LOG("WiFi status: out test mode\r\n");
//    }
        
    gizwitsProtocol.wifiStatusEvent.event[gizwitsProtocol.wifiStatusEvent.num] = WIFI_RSSI;
    gizwitsProtocol.wifiStatusEvent.num++;
    gizwitsProtocol.wifiStatusData.rssi = status->ststus.types.rssi;

    gizwitsProtocol.issuedFlag = 2;

    return 0;
}

/**
* @brief 协议下发数据返回ACK
*
* @param [in] head                  : 协议头指针
* @param [in] data                  : 数据地址
* @param [in] len                   : 数据长度
*
* @return : 有效数据长度,正确返回;-1，错误返回
*/
static int32 gizProtocolIssuedDataAck(protocolHead_t *head, uint8 *data, uint32 len)
{
    uint8 *ptrData = NULL;
    uint32 dataLen = 0;
    protocolReport_t protocolReport;
    protocolCommon_t protocolCommon;
    protocolP0Head_t *p0Head = (protocolP0Head_t *)head;

    if((NULL == head)||(NULL == data))
    {
//        GIZWITS_LOG("gizProtocolIssuedDataAck Error , Illegal Param\n");
        return -1;
    }
    if(0 == len)
    {
        gizProtocolHeadInit((protocolHead_t *)&protocolCommon);
        protocolCommon.head.cmd = head->cmd+1;
        protocolCommon.head.sn = head->sn;
        protocolCommon.head.len = gizProtocolExchangeBytes(sizeof(protocolCommon_t)-4);
        protocolCommon.sum = gizProtocolSum((uint8 *)&protocolCommon, sizeof(protocolCommon_t));
        
        ptrData = (uint8 *)&protocolCommon;
        dataLen = sizeof(protocolCommon_t);
    }
    else
    {
        gizProtocolHeadInit((protocolHead_t *)&protocolReport);
        protocolReport.head.cmd = p0Head->head.cmd+1;
        protocolReport.head.sn = p0Head->head.sn;
        protocolReport.head.len = gizProtocolExchangeBytes(sizeof(protocolP0Head_t)+len+1-4);
        protocolReport.action = p0Head->action+1;

        memcpy((uint8 *)&protocolReport.reportData, data, len);
        protocolReport.sum = gizProtocolSum((uint8 *)&protocolReport, sizeof(protocolReport_t));
        
        ptrData = (uint8 *)&protocolReport;
        dataLen = sizeof(protocolReport_t);
    }
    
    GIZWITS_UART_WRITE(ptrData, dataLen);

    return dataLen;
}

///**
//* @brief 上报数据
//*
//* @param [in] action            : PO cmd
//* @param [in] data              : 数据地址
//* @param [in] len               : 数据长度
//*
//* @return : 正确返回有效数据长度;-1，错误返回
//*/
//static int32 gizReportData(uint8 action, uint8 *data, uint32 len)
//{
//    protocolReport_t protocolReport;
//
//    if(NULL == data)
//    {
////        GIZWITS_LOG("gizReportData Error , Illegal Param\n");
//        return -1;
//    }
//    gizProtocolHeadInit((protocolHead_t *)&protocolReport);
//    protocolReport.head.cmd = CMD_REPORT_P0;
//    protocolReport.head.sn = gizwitsProtocol.sn++;
//    protocolReport.action = action;
//    protocolReport.head.len = gizProtocolExchangeBytes(sizeof(protocolReport_t)-4);
//    memcpy((gizwitsReport_t *)&protocolReport.reportData, (gizwitsReport_t *)data,len);
//    protocolReport.sum = gizProtocolSum((uint8 *)&protocolReport, sizeof(protocolReport_t));
//    
//    GIZWITS_UART_WRITE((uint8 *)&protocolReport, sizeof(protocolReport_t));
//
//    gizProtocolWaitAck((uint8 *)&protocolReport, sizeof(protocolReport_t));
////    gizwitsProtocol.lastReportTime = gizGetTimerCount();
//
//    return sizeof(protocolReport_t);
//}

/**
* @brief 转发
*
* @param [in] devstatus         : 数据地址
* @param [in] len               : 数据长度
*
* @return : 正确返回有效数据长度;-1，错误返回
*/
static int32 gizForwardData( uint8 *devstatus, uint8 len )
{
    uint8 packet[ sizeof(protocolReport_t)+sizeof(devStatus_t) ];
    protocolHead_t *head = (protocolHead_t *)packet;
    uint8 start = sizeof(protocolHead_t);
    uint8 i = 0;
    
    if( NULL == devstatus )
    {
        return -1;
    }

    memset(packet,0,sizeof(packet));
    
    gizProtocolHeadInit(head);
    
    head->cmd = CMD_REPORT_P0;
    head->sn = gizwitsProtocol.sn++;
    head->flags[0] = 0x00;
    head->flags[1] = 0x00;
    packet[start++] = ACTION_REPORT_DEV_STATUS;
    
    for( i=0; i<len; i++ )
    {
        packet[start++] = devstatus[i];
        if( devstatus[i] == 0xFF )
        {
           packet[start++] = 0x55; 
        }
    }
    start++;
    head->len = gizProtocolExchangeBytes(start-4);
    packet[start-1] = gizProtocolSum(packet, start);

    GIZWITS_UART_WRITE(packet, start);
    
    gizProtocolWaitAck(packet, start);

    return len;
}

/**
* @brief 接收协议报文并进行相应的协议处理

* Wifi模组接收来自云端或APP端下发的相关协议数据发送到MCU端，经过协议报文解析后将相关协议数据传入次函数，进行下一步的协议处理

* @param[in] inData   : 输入的协议数据
* @param[in] inLen    : 输入数据的长度
* @param[out] outData : 输出的协议数据
* @param[out] outLen  : 输出数据的长度
* @return 0， 执行成功， 非 0， 失败
*/
static int8 gizProtocolIssuedProcess(uint8 *inData, uint32 inLen, uint8 *outData, uint32 *outLen)
{
    protocolReport_t *protocolIssuedData = (protocolReport_t *)inData;
    uint8 issuedAction = 0;
    issuedAction = protocolIssuedData->action;

    if((NULL == inData)||(NULL == outData)||(NULL == outLen))
    {
//        GIZWITS_LOG("gizProtocolIssuedProcess Error , Illegal Param\n");
        return -1;
    }
    
    memset((uint8 *)&gizwitsProtocol.issuedProcessEvent, 0, sizeof(eventInfo_t));
    switch(issuedAction)
    {
        case ACTION_CONTROL_DEVICE:
            gizDataPoint2Event((gizwitsIssued_t *)(inData+sizeof(protocolP0Head_t)), &gizwitsProtocol.issuedProcessEvent,&gizwitsProtocol.gizCurrentDataPoint);
            gizwitsProtocol.issuedFlag = 1;
            outData = NULL;
            *outLen = 0;
            break;
        
        case ACTION_READ_DEV_STATUS:
            gizDataPoints2ReportData(&gizwitsProtocol.gizLastDataPoint,&gizwitsProtocol.reportData.devStatus);
            //memcpy((uint8 *)&gizwitsProtocol.gizLastDataPoint, (uint8 *)currentData, sizeof(dataPoint_t));

            memcpy(outData, (uint8 *)&gizwitsProtocol.reportData.devStatus, sizeof(gizwitsReport_t));
            *outLen = sizeof(gizwitsReport_t);
            break;
        
//        case ACTION_W2D_TRANSPARENT_DATA:
//            memcpy(gizwitsProtocol.transparentBuff, inData+sizeof(protocolP0Head_t), inLen-sizeof(protocolP0Head_t)-1);
//            gizwitsProtocol.transparentLen = inLen - sizeof(protocolP0Head_t) - 1;
//            gizwitsProtocol.issuedProcessEvent.event[gizwitsProtocol.issuedProcessEvent.num] = TRANSPARENT_DATA;
//            gizwitsProtocol.issuedProcessEvent.num++;
//            gizwitsProtocol.issuedFlag = 3;
//            break;
        
        default:
            break;
    }

    return 0;
}

/**@name Gizwits 用户API接口
* @{
*/

/**
* @brief gizwits协议初始化接口

* 用户调用该接口可以完成Gizwits协议相关初始化（包括协议相关定时器、串口的初始）

* 用户可在在此接口内完成数据点的初始化状态设置

* @param none
* @return none
*/
void gizwitsInit(void)
{
    pRb.rbCapacity = RB_MAX_LEN;
    pRb.rbBuff = rbBuf;
    rbCreate(&pRb);
        
//  gizwitsSetMode(0);
        
    memset((uint8 *)&gizwitsProtocol, 0, sizeof(gizwitsProtocol_t));
}

/**
* @brief WiFi配置接口

* 用户可以调用该接口使WiFi模组进入相应的配置模式或者复位模组

* @param[in] mode 配置模式选择：0x0， 模组复位 ;0x01， SoftAp模式 ;0x02， AirLink模式
* @return 错误命令码
*/
int32 gizwitsSetMode(uint8 mode)
{
    int32 ret = 0;
    protocolCfgMode_t cfgMode;
    protocolCommon_t setDefault;

    if(mode == WIFI_RESET_MODE)
    {
        gizProtocolHeadInit((protocolHead_t *)&setDefault);
        setDefault.head.cmd = CMD_SET_DEFAULT;
        setDefault.head.sn = gizwitsProtocol.sn++;
        setDefault.head.len = gizProtocolExchangeBytes(sizeof(protocolCommon_t)-4);
        setDefault.sum = gizProtocolSum((uint8 *)&setDefault, sizeof(protocolCommon_t));
        GIZWITS_UART_WRITE((uint8 *)&setDefault, sizeof(protocolCommon_t));

        gizProtocolWaitAck((uint8 *)&setDefault, sizeof(protocolCommon_t));
    }
    else if((mode == WIFI_SOFTAP_MODE)||(mode == WIFI_AIRLINK_MODE))
    {
        gizProtocolHeadInit((protocolHead_t *)&cfgMode);
        cfgMode.head.cmd = CMD_WIFI_CONFIG;
        cfgMode.head.sn = gizwitsProtocol.sn++;
        cfgMode.cfgMode = mode;
        cfgMode.head.len = gizProtocolExchangeBytes(sizeof(protocolCfgMode_t)-4);
        cfgMode.sum = gizProtocolSum((uint8 *)&cfgMode, sizeof(protocolCfgMode_t));
        GIZWITS_UART_WRITE((uint8 *)&cfgMode, sizeof(protocolCfgMode_t));

        gizProtocolWaitAck((uint8 *)&cfgMode, sizeof(protocolCfgMode_t));
    }

    return ret;
}

/**
* @brief 配置WIFI模块基进入绑定模式

* 用户可以调用该接口使WiFi模组进入绑定模式

* @param[in] mode void
* @return 错误命令码
*/
int32 gizwitsSetBind( void )
{
    int32 ret = 0;
    protocolCommon_t packet;
    
    gizProtocolHeadInit((protocolHead_t *)&packet);
    
    packet.head.cmd   = CMD_BIND_CONFIG;
    packet.head.len   = gizProtocolExchangeBytes(sizeof(protocolCommon_t)-4);
    packet.head.sn    = gizwitsProtocol.sn++;
    packet.sum        = gizProtocolSum((uint8 *)&packet, sizeof(protocolCommon_t));
    
    GIZWITS_UART_WRITE((uint8 *)&packet, sizeof(protocolCommon_t));

    gizProtocolWaitAck((uint8 *)&packet, sizeof(protocolCommon_t));
        
    return ret;
}

/**
* @brief 机智云上报函数

* 用户可以调用该接口上报数据

* @param[in] packet 数据包
* @return 错误命令码
*/
int32 gizwitsReport( uint8 *packet )
{
    int32 ret = 0;
    
//    if( packet == NULL )
//    {
//        return -1;
//    }
    
    gizDataPoints2ReportData((dataPoint_t *)packet,&gizwitsProtocol.reportData.devStatus);
    gizForwardData((uint8 *)&gizwitsProtocol.reportData.devStatus, sizeof(devStatus_t));
//    gizReportData(ACTION_REPORT_DEV_STATUS, (uint8 *)&gizwitsProtocol.reportData.devStatus, sizeof(devStatus_t));
//    gizReportData(ACTION_D2W_TRANSPARENT_DATA, (uint8 *)&gizwitsProtocol.reportData.devStatus, sizeof(devStatus_t));
    memcpy((uint8 *)&gizwitsProtocol.gizLastDataPoint, packet, sizeof(dataPoint_t));
        
    return ret;
}

/**
* @brief 获取APP连接状态

* 用户可以调用检测WIFI模块与APP之间的连接状态

* @param[in] void
* @return bool
*/
bool gizwitsGetAppConntStatus( void )
{
    return gizwitsProtocol.app_connt_status;
}

///**
//* @brief 机智云转发函数
//
//* 用户可以调用该接口上报数据
//
//* @param[in] packet 数据包
//* @return 错误命令码
//*/
//int32 gizwitsForward( uint8 *packet )
//{
//    int32 ret = 0;
//    
////    if( packet == NULL )
////    {
////        return -1;
////    }
//    
//    gizDataPoints2ReportData((dataPoint_t *)packet,&gizwitsProtocol.reportData.devStatus);
//    //gizReportData(ACTION_REPORT_DEV_STATUS, (uint8 *)&gizwitsProtocol.reportData.devStatus, sizeof(devStatus_t));
//    gizForwardData((uint8 *)&gizwitsProtocol.reportData.devStatus, sizeof(devStatus_t));
////    gizReportData(ACTION_D2W_TRANSPARENT_DATA, (uint8 *)&gizwitsProtocol.reportData.devStatus, sizeof(devStatus_t));
//    memcpy((uint8 *)&gizwitsProtocol.gizLastDataPoint, packet, sizeof(dataPoint_t));
//        
//    return ret;
//}

/**
* @brief 协议处理函数

* 该函数中完成了相应协议数据的处理及数据主动上报的相关操作

* @param [in] currentData 待上报的协议数据指针
* @return none
*/
int32 gizwitsHandle(dataPoint_t *currentData)
{
    int8 ret = 0;
//    uint16 i = 0;
    uint8 ackData[128];
    uint16 protocolLen = 0;
    uint32 ackLen = 0;
    protocolHead_t *recvHead = NULL;

    if(NULL == currentData)
    {
        GIZWITS_LOG("GizwitsHandle Error , Illegal Param\n");
        return -1;
    }

    /*重发机制*/
    gizProtocolAckHandle();
    ret = gizProtocolGetOnePacket(&pRb, gizwitsProtocol.protocolBuf, &protocolLen);

    if(0 == ret)
    {
        GIZWITS_LOG("Get Packet!\n");

        recvHead = (protocolHead_t *)gizwitsProtocol.protocolBuf;
        switch (recvHead->cmd)
        {
            case CMD_GET_DEVICE_INTO:
                gizProtocolGetDeviceInfo(recvHead);
                GIZWITS_LOG("report info!\n");
                break;
            case CMD_ISSUED_P0:
                ret = gizProtocolIssuedProcess(gizwitsProtocol.protocolBuf, protocolLen, ackData, &ackLen);
                if(0 == ret)
                {
                    gizProtocolIssuedDataAck(recvHead, ackData, ackLen);
                }
                break;
            case CMD_HEARTBEAT:
                gizProtocolCommonAck(recvHead);
                break;
            case CMD_WIFISTATUS:
                gizProtocolCommonAck(recvHead);
                gizProtocolModuleStatus((protocolWifiStatus_t *)recvHead);
                break;
            case ACK_REPORT_P0:
            case ACK_WIFI_CONFIG:
            case ACK_SET_DEFAULT:
                gizProtocolWaitAckCheck(recvHead);
                break;
            case CMD_MCU_REBOOT:
                gizProtocolCommonAck(recvHead);
                GIZWITS_LOG("MCU reboot!\n");
                
                gizProtocolReboot();
                break;
            case CMD_ERROR_PACKAGE:
                GIZWITS_LOG("I SEND ERROR PACKET TO GIZWITS!\n");
                break;
            default:
                gizProtocolErrorCmd(recvHead,ERROR_CMD);
                GIZWITS_LOG("ERROR: cmd code error!\n");
                break;
        }
    }
    else if(-2 == ret)
    {
        //校验失败，报告异常
        recvHead = (protocolHead_t *)gizwitsProtocol.protocolBuf;
        gizProtocolErrorCmd(recvHead,ERROR_ACK_SUM);
        GIZWITS_LOG("ERROR: check sum error!\n");
        return -2;
    }
    
    if(1 == gizwitsProtocol.issuedFlag)
    {
        gizwitsProtocol.issuedFlag = 0;
        gizwitsEventProcess(&gizwitsProtocol.issuedProcessEvent, (uint8 *)&gizwitsProtocol.gizCurrentDataPoint, sizeof(dataPoint_t));
        memset((uint8 *)&gizwitsProtocol.issuedProcessEvent,0x0,sizeof(gizwitsProtocol.issuedProcessEvent)); 
    }
    else if(2 == gizwitsProtocol.issuedFlag)
    {
        gizwitsProtocol.issuedFlag = 0;
        gizwitsEventProcess(&gizwitsProtocol.wifiStatusEvent, (uint8 *)&gizwitsProtocol.wifiStatusData, sizeof(moduleStatusInfo_t));
        memset((uint8 *)&gizwitsProtocol.wifiStatusEvent,0x0,sizeof(gizwitsProtocol.wifiStatusEvent));
    }
//    else if(3 == gizwitsProtocol.issuedFlag)
//    {
//        gizwitsProtocol.issuedFlag = 0;
//        gizwitsEventProcess(&gizwitsProtocol.issuedProcessEvent, (uint8 *)gizwitsProtocol.transparentBuff, gizwitsProtocol.transparentLen);
//    }
    
//    if((1 == gizCheckReport(currentData, (dataPoint_t *)&gizwitsProtocol.gizLastDataPoint)))
//    {
//        GIZWITS_LOG("changed, report data\n");
//        gizwitsReport((uint8 *)currentData);
//    }
    
//    if(600000 <= (gizGetTimerCount() - gizwitsProtocol.lastReportTime))
//    {
//        GIZWITS_LOG("Info: 600S report data\n");
//        gizwitsReport((uint8 *)currentData);
//    }

    return 0;
}

/**@} */

#endif

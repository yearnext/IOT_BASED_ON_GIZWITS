/**
************************************************************
* @file         gizwits_protocol.h
* @brief        对应gizwits_protocol.c的头文件 (包含产品相关定义)
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
#ifndef _GIZ_PROTOCOL_H
#define _GIZ_PROTOCOL_H

#include "ZComDef.h"
#include "hal_uart.h"
#include "MT_UART.h"

/**
* @name 日志打印宏定义
* @{
*/
#define USE_GIZWITS_DEBUG 1

#if USE_GIZWITS_DEBUG
    #define GIZWITS_LOG(n) HalUARTWrite(1,n,sizeof(n))  ///<运行日志打印
#else 
    #define GIZWITS_LOG(n)
#endif

/**@} */
#define GIZWITS_UART_PORT 0
#define GIZWITS_UART_WRITE(data,len)  HalUARTWrite(GIZWITS_UART_PORT,data,len)

/** WiFi模组配置参数*/
typedef enum
{
  WIFI_RESET_MODE = 0x00,                     ///< WIFI模组复位
  WIFI_SOFTAP_MODE,                           ///< WIFI模组softAP模式
  WIFI_AIRLINK_MODE,                          ///< WIFI模组AirLink模式
}WIFI_MODE_TYPE_T;

#define MAX_PACKAGE_LEN    256                ///< 数据缓冲区最大长度
#define RB_MAX_LEN         (MAX_PACKAGE_LEN*1)///< 环形缓冲区最大长度
#define min(a, b) (a)<(b)?(a):(b)             ///< 获取最小值

#define SEND_MAX_TIME   300                   ///< 3000ms重发
#define SEND_MAX_NUM    2                     ///< 重发次数

#define protocol_VERSION "00000004"           ///< protocol版本号
#define P0_VERSION       "00000002"                ///< P0协议版本号
#define DEV_DEFAULT      "00000000"               ///< 设备属性

/**@name Product Key (产品标识码)
* @{
*/
#define PRODUCT_KEY "c46a192ef791475e9d6c991a6b11106d"
/**@} */

/**@name 数据点相关定义
* @{
*/

/**@} */

/** 事件结构体*/
typedef enum
{
  WIFI_SOFTAP = 0x00,                         ///< WiFi SOFTAP 配置事件
  WIFI_AIRLINK,                               ///< WiFi模块 AIRLINK 配置事件
  WIFI_STATION,                               ///< WiFi模块 STATION 配置事件
  WIFI_OPEN_BINDING,                          ///< WiFi模块开启绑定事件
  WIFI_CLOSE_BINDING,                         ///< WiFi模块关闭绑定事件
  WIFI_CON_ROUTER,                            ///< WiFi模块已连接路由事件
  WIFI_DISCON_ROUTER,                         ///< WiFi模块已断开连接路由事件
  WIFI_CON_M2M,                               ///< WiFi模块已连服务器M2M事件
  WIFI_DISCON_M2M,                            ///< WiFi模块已断开连服务器M2M事件
  WIFI_OPEN_TESTMODE,                         ///< WiFi模块开启测试模式事件
  WIFI_CLOSE_TESTMODE,                        ///< WiFi模块关闭测试模式事件
  WIFI_CON_APP,                               ///< WiFi模块连接APP事件
  WIFI_DISCON_APP,                            ///< WiFi模块断开APP事件
  WIFI_RSSI,                                  ///< WiFi模块信号事件
  TRANSPARENT_DATA,                           ///< 透传事件
  EVENT_PACKET,
  EVENT_TYPE_MAX
} EVENT_TYPE_T;

/** 用户区设备状态结构体*/
typedef struct {
  uint8 valuePacket[32];
} dataPoint_t;

/** 对应协议“4.10 WiFi模组控制设备”中的标志位"attr_flags" */ 
typedef struct {
  uint8 flagPacket:1;
} attrFlags_t;

/** 对应协议“4.10 WiFi模组控制设备”中的数据值"attr_vals" */
typedef struct {
  uint8 valuePacket[32];
} attrVals_t;

/** 对应协议“4.10 WiFi模组控制设备”中“P0协议区”的标志位"attr_flags" + 数据值"attr_vals" */ 
typedef struct {
    attrFlags_t attrFlags;
    attrVals_t  attrVals;
}gizwitsIssued_t;

/** 对应协议“4.9 设备MCU向WiFi模组主动上报当前状态”中的设备状态"dev_status" */
typedef struct {

  uint8 valuePacket[32];
} devStatus_t;

/** 对应协议“4.9 设备MCU向WiFi模组主动上报当前状态”中的 设备状态位"dev_status"  */ 
typedef struct {
    devStatus_t devStatus;                          ///< 作用：用来存放待上报的设备状态数据
}gizwitsReport_t;

/** 事件队列结构体 */
typedef struct {
    uint8 num;                                    ///< 队列成员个数
    uint8 event[EVENT_TYPE_MAX];                  ///< 队列成员事件内容
}eventInfo_t;

/** wifi信号强度数值结构体 */
typedef struct {
    uint8 rssi;                                   ///< WIFI信号强度
}moduleStatusInfo_t;

/** 协议命令码 */
typedef enum
{
    CMD_GET_DEVICE_INTO             = 0x01,         ///< 命令字，对应协议：4.1 WiFi模组请求设备信息 中 WiFi模组发送
    ACK_GET_DEVICE_INFO             = 0x02,         ///< 命令字，对应协议：4.1 WiFi模组请求设备信息 中 设备MCU回复
            
    CMD_ISSUED_P0                   = 0x03,         ///< 命令字，对应协议：4.8 WiFi模组读取设备的当前状态、4.10 WiFi模组控制设备 中 WiFi模组发送
    ACK_ISSUED_P0                   = 0x04,         ///< 命令字，对应协议：4.8 WiFi模组读取设备的当前状态、4.10 WiFi模组控制设备 中 设备MCU回复
            
    CMD_REPORT_P0                   = 0x05,         ///< 命令字，对应协议：4.9 设备MCU向WiFi模组主动上报当前状态 中 设备MCU发送
    ACK_REPORT_P0                   = 0x06,         ///< 命令字，对应协议：4.9 设备MCU向WiFi模组主动上报当前状态 中 WiFi模组回复
            
    CMD_HEARTBEAT                   = 0x07,         ///< 命令字，对应协议：4.2 WiFi模组与设备MCU的心跳 中 WiFi模组发送
    ACK_HEARTBEAT                   = 0x08,         ///< 命令字，对应协议：4.2 WiFi模组与设备MCU的心跳 中 设备MCU回复
            
    CMD_WIFI_CONFIG                 = 0x09,         ///< 命令字，对应协议：4.3 设备MCU通知WiFi模组进入配置模式 中 设备MCU发送
    ACK_WIFI_CONFIG                 = 0x0A,         ///< 命令字，对应协议：4.3 设备MCU通知WiFi模组进入配置模式 中 WiFi模组回复
            
    CMD_SET_DEFAULT                 = 0x0B,         ///< 命令字，对应协议：4.4 设备MCU重置WiFi模组 中 设备MCU发送
    ACK_SET_DEFAULT                 = 0x0C,         ///< 命令字，对应协议：4.4 设备MCU重置WiFi模组 中 WiFi模组回复
            
    CMD_WIFISTATUS                  = 0x0D,         ///< 命令字，对应协议：4.5 WiFi模组向设备MCU通知WiFi模组工作状态的变化 中 WiFi模组发送
    ACK_WIFISTATUS                  = 0x0E,         ///< 命令字，对应协议：4.5 WiFi模组向设备MCU通知WiFi模组工作状态的变化 中 设备MCU回复
        
    CMD_MCU_REBOOT                  = 0x0F,         ///< 命令字，对应协议：4.6 WiFi模组请求重启MCU 中 WiFi模组发送
    ACK_MCU_REBOOT                  = 0x10,         ///< 命令字，对应协议：4.6 WiFi模组请求重启MCU 中 设备MCU回复
            
    CMD_ERROR_PACKAGE               = 0x11,         ///< 命令字，对应协议：4.7 非法消息通知 中 WiFi模组回应MCU对应包序号的数据包非法
    ACK_ERROR_PACKAGE               = 0x12,         ///< 命令字，对应协议：4.7 非法消息通知 中 MCU回应WiFi模组对应包序号的数据包非法

    CMD_BIND_CONFIG                 = 0x15,        ///< 命令字，对应协议：4.7 MCU通知wifi模组进入可绑定模式 中 设备MCU发送
    ACK_BIND_CONFIG                 = 0x16,        ///< 命令字，对应协议：4.7 MCU通知wifi模组进入可绑定模式 中 WiFi模组回复
} PROTOCOL_CMDTYPE;

/** 重发机制结构体 */
typedef struct {
    uint8                 num;                    ///< 重发次数
    uint8                 flag;                   ///< 1,表示有需要等待的ACK;0,表示无需要等待的ACK
    uint8                 buf[MAX_PACKAGE_LEN];   ///< 重发数据缓冲区
    uint16                dataLen;                ///< 重发数据长度
    uint32                sendTime;               ///< 重发的系统时间
} protocolWaitAck_t;

/** 协议标准头结构体 */
typedef struct
{
    uint8                 head[2];                ///< 包头(header)固定为0xFFFF
    uint16                len;                    ///< 长度(len)是指从cmd 开始到整个数据包结束所占用的字节数
    uint8                 cmd;                    ///< 命令字（cmd）表示具体的命令含义，详见协议举例
    uint8                 sn;                     ///< 消息序号(sn)由发送方给出,接收方响应命令时需把消息序号返回给发送方
    uint8                 flags[2];               ///< 标志位（flag），产品填写默认0
} protocolHead_t;

/** 4.1 WiFi模组请求设备信息 协议结构体 */
typedef struct
{
    protocolHead_t        head;                   ///< 协议标准头结构体
    uint8                 protocolVer[8];         ///< 协议版本号
    uint8                 p0Ver[8];               ///< p0协议版本号
    uint8                 hardVer[8];             ///< 硬件版本号
    uint8                 softVer[8];             ///< 软件版本号
    uint8                 productKey[32];         ///< 产品标识码
    uint16                ninableTime;            ///< 绑定超时(秒)
    uint8                 devAttr[8];             ///< 设备属性
    uint8                 sum;                    ///< 检验和
} protocolDeviceInfo_t;

/** 协议通用数据帧(4.2、4.4、4.6、4.9、4.10) 协议结构体 */
typedef struct
{
    protocolHead_t        head;                   ///< 协议标准头结构体
    uint8                 sum;                    ///< 检验和
} protocolCommon_t;

/** 4.3 设备MCU通知WiFi模组进入配置模式 协议结构体 */
typedef struct
{
    protocolHead_t        head;                   ///< 协议标准头结构体
    uint8                 cfgMode;                ///< 配置参数
    uint8                 sum;                    ///< 检验和
} protocolCfgMode_t;

/** WiFi模组工作状态*/
typedef union
{
    uint16                value;
     struct
    {
        uint16            softap:1;               ///< 表示WiFi模组所处的SOFTAP模式状态，类型为bool
        uint16            station:1;              ///< 表示WiFi模组所处的STATION模式状态，类型为bool
        uint16            onboarding:1;           ///< 表示WiFi模组所处的配置状态，类型为bool
        uint16            binding:1;              ///< 表示WiFi模组所处的绑定状态，类型为bool
        uint16            con_route:1;            ///< 表示WiFi模组与路由器的连接状态，类型为bool
        uint16            con_m2m:1;              ///< 表示WiFi模组与云端m2m的状态，类型为bool
        uint16            reserve1:2;             ///< 数据位补齐
        uint16            rssi:3;                 ///< 表示路由的信号强度，类型为数值
        uint16            app:1;                  ///< 表示WiFi模组与APP端的连接状态，类型为bool
        uint16            test:1;                 ///< 表示WiFi模组所处的场测模式状态，类型为bool
        uint16            reserve2:3;             ///< 数据位补齐
    }types; 
    
} wifiStatus_t;

/** WiFi状态类型 协议结构体 */
typedef struct
{
    protocolHead_t          head;                   ///< 协议标准头结构体
    wifiStatus_t            ststus;                 ///< WIFI状态
    uint8                   sum;                    ///< 检验和
} protocolWifiStatus_t;

/** 非法报文类型*/  
typedef enum
{
    ERROR_ACK_SUM = 0x01,                           ///< 校验错误
    ERROR_CMD     = 0x02,                           ///< 命令码错误
    ERROR_OTHER   = 0x03,                           ///< 其他
} errorPacketsType_t;

/** 4.7 非法消息通知 协议结构体*/
typedef struct
{
    protocolHead_t          head;                   ///< 协议标准头结构体
    uint8                 error;                  ///< 错误值
    uint8                 sum;                    ///< 检验和
} protocolErrorType_t;

/** P0 command 命令码*/
typedef enum
{
    ACTION_CONTROL_DEVICE       = 0x01,             ///< 协议4.10 WiFi模组控制设备 WiFi模组发送
    ACTION_READ_DEV_STATUS      = 0x02,             ///< 协议4.8 WiFi模组读取设备的当前状态 WiFi模组发送
    ACTION_READ_DEV_STATUS_ACK  = 0x03,             ///< 协议4.8 WiFi模组读取设备的当前状态 设备MCU回复
    ACTION_REPORT_DEV_STATUS    = 0X04,             ///< 协议4.9 设备MCU向WiFi模组主动上报当前状态 设备MCU发送
    ACTION_W2D_TRANSPARENT_DATA = 0x05,             ///< WiFi到设备MCU透传
    ACTION_D2W_TRANSPARENT_DATA = 0x06,             ///< 设备MCU到WiFi透传
} actionType_t;

/** 4.8 WiFi模组读取设备的当前状态 设备MCU回复结构体 */
typedef struct
{
    protocolHead_t          head;                   ///< 协议标准头结构体
    uint8                 action;                 ///< p0 命令位
    gizwitsReport_t         reportData;             ///< 上报数据区
    uint8                 sum;                    ///< 检验和
} protocolReport_t;

/** P0报文标准头 */
typedef struct
{
    protocolHead_t          head;                   ///< 协议标准头结构体
    uint8                 action;                 ///< p0 命令位
} protocolP0Head_t;

/** 环形缓冲区数据结构 */
typedef struct {
    uint16  rbCapacity;
    uint8   *rbHead;
    uint8   *rbTail;
    uint8   *rbBuff;
}rb_t;

/** 协议处理总结构体 */
typedef struct
{
    uint8 issuedFlag;                             ///< 保存对应的 p0命令类型
    uint8 protocolBuf[MAX_PACKAGE_LEN];           ///< 协议报文存储区
//    uint8 transparentBuff[MAX_PACKAGE_LEN];       ///< 透传数据存储区
//    uint32 transparentLen;                        ///< 透传数据长度
    
    uint32 sn;                                    ///< 消息序号
    uint32 timerMsCount;                          ///< 时间计数(Ms)
    uint32 lastReportTime;                        ///< 上次上报时间
    protocolWaitAck_t waitAck;                      ///< 协议ACK数据结构
    
    eventInfo_t issuedProcessEvent;                 ///< 控制事件
    eventInfo_t wifiStatusEvent;                    ///< WIFI状态 事件

    dataPoint_t gizCurrentDataPoint;                ///< 当前设备状态数据
    dataPoint_t gizLastDataPoint;                   ///< 上次上报的设备状态数据
    gizwitsReport_t reportData;                     ///< 协议上报实际数据
    moduleStatusInfo_t wifiStatusData;              ///< WIFI 状态信息(信号强度)
}gizwitsProtocol_t;

#if defined (USE_GIZWITS_MOD)

/**@name Gizwits 用户API接口
* @{
*/
void gizwitsInit(void);
int32 gizwitsHandle(dataPoint_t *currentData);
int32 gizwitsSetMode(uint8 mode);
int32 gizwitsSetBind( void );
/**@} */
void gizTimerMs(void);
int32 gizPutData(uint8 *buf, uint32 len);
uint32 gizGetTimerCount(void);

int32 gizwitsReport( uint8 *packet );

#endif

#endif
/**
 ******************************************************************************
  * @file       myprotocol.h
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      myprotocol 头文件
  * @par        工作平台                                  
  *                  CC2530
  * @par        工作频率                                  
  *                  32MHz
  * @par        编译平台									                          
  * 				 IAR
 ******************************************************************************
  * @note
  * 1.XXXXX                  						                      
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MY_IOT_PROTOCOL_H__
#define __MY_IOT_PROTOCOL_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "comdef.h"
#include <stddef.h>
    
/* Exported macro ------------------------------------------------------------*/
/** 程序调试宏 */
#define USE_MYPROTOCOL_DEBUG 1

/** 定义支持设备清单 */
#define MYPROTOCOL_DEVICE_PHONE     (0x00)
#define MYPROTOCOL_DEVICE_COORD     (0x00)
#define MYPROTOCOL_DEVICE_LIGHT     (0x01)
#define MYPROTOCOL_DEVICE_SOCKET    (0x02)
#define MYPROTOCOL_DEVICE_CURTAIN   (0x03)
#define MYPROTOCOL_DEVICE_HT_SENSOR (0x04)

/**
 * @name 定义MYPROTOCOL设备信息
 * @{
 */
/** 定义当前设备类型 */
#if defined (MYIOT_DEVICE_COORD)
#define MYPROTOCOL_DEVICE MYPROTOCOL_DEVICE_COORD
#elif defined (MYIOT_DEVICE_LIGHT)
#define MYPROTOCOL_DEVICE MYPROTOCOL_DEVICE_LIGHT
#elif defined (MYIOT_DEVICE_SOCKET)
#define MYPROTOCOL_DEVICE MYPROTOCOL_DEVICE_SOCKET
#elif defined (MYIOT_DEVICE_CURTAIN)
#define MYPROTOCOL_DEVICE MYPROTOCOL_DEVICE_CURTAIN
#elif defined (MYIOT_DEVICE_HT_SENSOR)
#define MYPROTOCOL_DEVICE MYPROTOCOL_DEVICE_HT_SENSOR
#else
#define MYPROTOCOL_DEVICE
#endif
  
/** 判断当前设备功能宏 */
#define MYPROTOCOL_DEVICE_IS_COORD     ( MYPROTOCOL_DEVICE == MYPROTOCOL_DEVICE_COORD     )
#define MYPROTOCOL_DEVICE_IS_LIGHT     ( MYPROTOCOL_DEVICE == MYPROTOCOL_DEVICE_LIGHT     )
#define MYPROTOCOL_DEVICE_IS_SOCKET    ( MYPROTOCOL_DEVICE == MYPROTOCOL_DEVICE_SOCKET    )
#define MYPROTOCOL_DEVICE_IS_CURTAIN   ( MYPROTOCOL_DEVICE == MYPROTOCOL_DEVICE_CURTAIN   )
#define MYPROTOCOL_DEVICE_IS_HT_SENSOR ( MYPROTOCOL_DEVICE == MYPROTOCOL_DEVICE_HT_SENSOR )
   
/** 判断是否存在遵守MYPROTOCOL协议设备的宏 */
#if MYPROTOCOL_DEVICE_IS_COORD \
    || MYPROTOCOL_DEVICE_IS_LIGHT \
    || MYPROTOCOL_DEVICE_IS_SOCKET \
    || MYPROTOCOL_DEVICE_IS_CURTAIN \
    || MYPROTOCOL_DEVICE_IS_HT_SENSOR
        
#define USE_MYPROTOCOL_DEVICE 1
        
#else
#define USE_MYPROTOCOL_DEVICE 0
        
#endif
        
/** 判断是否存在遵守MYPROTOCOL协议设备的宏 */
#if MYPROTOCOL_DEVICE_IS_LIGHT \
    || MYPROTOCOL_DEVICE_IS_SOCKET \
    || MYPROTOCOL_DEVICE_IS_CURTAIN \
        
#define USE_MYPROTOCOL_TIME_DEVICE 1
        
#else
#define USE_MYPROTOCOL_TIME_DEVICE 0
        
#endif
        
/**
 * @}
 */  
        
/**
 * @name 定义通信相关参数
 * @{
 */
/** 计算偏移量 */
//#define MYPROTOCOL_OFFSETOF(t,m)                 ((uint16)(&((t *)0)->m)) 
#define MYPROTOCOL_OFFSETOF(t,m)                 offsetof(t,m)
/** 数据包所占内存空间 */
#define MYPROTOCOL_PACKET_SIZE                   (sizeof(MYPROTOCOL_FORMAT_t)/sizeof(uint8))
/** 设备信息所占内存空间 */
#define MYPROTOCOL_DEVICE_INFO_SIZE              (sizeof(MYPROTOCOL_DEVICE_INFO_t)/sizeof(uint8))
/** 计算user_data在MYPROTOCOL_FORMAT_t中的偏移量 */
#define MYPROTOCOL_USER_DATA_OFFSET              MYPROTOCOL_OFFSETOF(MYPROTOCOL_FORMAT_t,user_data)
/** 计算user_data各个成员的的偏移量 */
#define MYPROTOCOL_USER_DATA_M_OFFSET(m)         MYPROTOCOL_OFFSETOF(MYPROTOCOL_USER_DATA_t,m)
/** 计算cmd在MYPROTOCOL_USER_DATA_t中的偏移量 */
#define MTPROTOCOL_USER_DATA_CMD_OFFSET          MYPROTOCOL_USER_DATA_M_OFFSET(cmd)
/** 计算len在MYPROTOCOL_USER_DATA_t中的偏移量 */
#define MYPROTOCOL_USER_DATA_LEN_OFFSET          MYPROTOCOL_USER_DATA_M_OFFSET(len) 
/** 计算data在MYPROTOCOL_USER_DATA_t中的偏移量 */
#define MYPROTOCOL_USER_DATA_DATA_OFFSET         MYPROTOCOL_USER_DATA_M_OFFSET(data) 
///** 计算user_data所占内存空间大小 user_data为MYPROTOCOL_USER_DATA_t类型的结构体指针*/
//#define MYPROTOCOL_CAL_USER_DATA_SIZE(user_data) ( MYPROTOCOL_USER_DATA_LEN_OFFSET \
//                                                  + (((MYPROTOCOL_USER_DATA_t *)user_data)->len))  
/** 通讯格式 用户数据大小 */
#define MYPROTOCOL_USER_DATA_SIZE   (18)
/** 通讯格式 MAC地址大小 */
#define MYPROTOCOL_MAC_ADDR_SIZE    (8)

/** 定义通讯方向 */
// #define MYPROTOCOL_COMM_END                  (0x00)
// #define MYPROTOCOL_W2D_WAIT			(0x01)
// #define MYPROTOCOL_W2D_ACK			(0x02)
// #define MYPROTOCOL_D2W_WAIT			(0x03)
// #define MYPROTOCOL_D2W_ACK			(0x04)
// #define MYPROTOCOL_H2S_WAIT			(0x05)
// #define MYPROTOCOL_H2S_ACK			(0x06)
// #define MYPROTOCOL_S2H_WAIT			(0x07)
// #define MYPROTOCOL_S2H_ACK			(0x08)
// #define MYPROTOCOL_COMM_ERROR		(0x09)

/** 定义设备通用命令 */
#define MYPROTOCOL_TICK_CMD         (0x00)
#define MYPROTOCOL_RESET_CMD        (0x01)
#define MYPROTOCOL_REBOOT_CMD       (0x02)
#define MYPROTOCOL_UPDATE_TIME_CMD  (0x03)
#define MYPROTOCOL_RD_TIME_CMD      (0x04)
#define MYPROTOCOL_WR_TIME_CMD      (0x05)
/**@} */

/* Exported types ------------------------------------------------------------*/
/**
 * @name MYPROTOCOL数据包格式定义
 * @{
 */
/** 通讯类型 */
typedef enum
{
    MYPROTOCOL_COMM_END = 0x00,
    MYPROTOCOL_W2D_WAIT,
    MYPROTOCOL_W2D_ACK,
    MYPROTOCOL_D2W_WAIT,
    MYPROTOCOL_D2W_ACK,
    MYPROTOCOL_H2S_WAIT,
    MYPROTOCOL_H2S_ACK,
    MYPROTOCOL_S2H_WAIT,
    MYPROTOCOL_S2H_ACK,
    MYPROTOCOL_COMM_ERROR,
}MYPROTOCOL_COMMTYPE_t;

/** 设备信息 */
typedef struct
{
    uint8 device;
    uint8 mac[MYPROTOCOL_MAC_ADDR_SIZE];
}MYPROTOCOL_DEVICE_INFO_t;

/** 用户数据 */
typedef struct
{
    uint8 cmd;
    uint8 len;
    uint8 data[MYPROTOCOL_USER_DATA_SIZE];
}MYPROTOCOL_USER_DATA_t;
    
/** 设备应答 */
typedef struct
{
    uint8 id;
    MYPROTOCOL_DEVICE_INFO_t *info;
}MYPROTOCOL_DEVCICE_ACK_t;

/** 用户信息 */
typedef struct
{
    MYPROTOCOL_DEVICE_INFO_t deviceInfo;
    MYPROTOCOL_USER_DATA_t   userData;
}MYPROTOCOL_USER_MESSAGE_t;

/** MYPROTOCOL 格式 */
typedef struct
{
    MYPROTOCOL_COMMTYPE_t    commtype;
    uint8                    sn;
    MYPROTOCOL_DEVICE_INFO_t device;
    MYPROTOCOL_USER_DATA_t   user_data;
    uint8                    sum;
}MYPROTOCOL_FORMAT_t;

/**@} */

/**
 * @name MYPROTOCOL相关功能接口定义
 * @{
 */
typedef bool (*packet_type)(void *ctx, void *packet);
typedef bool (*send_type)(void *ctx, void *packet);
typedef bool (*receive_type)(MYPROTOCOL_FORMAT_t *ctx);
/**@} */

/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
 * @name 硬件初始化函数 宏定义
 * @{
 */
#if USE_MYPROTOCOL_DEBUG
extern bool MyprotocolInit( uint8* );
#else
extern void MyprotocolInit( uint8* );
#endif
/**@} */

/**
 * @name 输出调试信息函数 宏定义
 * @note 请使用宏 MYPROTOCOL_LOG 输出调试信息
 * @{
 */
#if USE_MYPROTOCOL_DEBUG
extern void MyprotocolPutLog( uint8*, uint16 );
#define MYPROTOCOL_LOG(n) MyprotocolPutLog(n,sizeof(n))
#else
#define MYPROTOCOL_LOG(n)
#endif
/**@} */

/**
 * @name 数据包校验宏 宏定义
 * @{
 */
extern bool MyprotocolPacketCheck( void* );
extern bool MyprotocolW2DRecDeviceCheck( MYPROTOCOL_FORMAT_t* );
extern bool MyprotocolD2DRecDeviceCheck( MYPROTOCOL_FORMAT_t* );
/**@} */

/**
 * @name 设备与网关的连接状态API
 * @{
 */
extern bool MyprotocolDeviceConStatusUpdate( uint8 );
/**@} */

/**
 * @name 发送数据API
 * @note 建议使用函数MyprotocolSendData发送数据
 * @{
 */
// 通用发送数据包函数
extern bool MyprotocolD2DSendData( void*, void* );
extern bool MyprotocolD2DBroadcastData( void*, void* );
extern bool MyprotocolForwardData( void*, void*, send_type );
extern bool MyprotocolSendData( void*, void*, packet_type, send_type );

// 专用发送数据包函数
extern bool MyprotocolReplyErrPacket( MYPROTOCOL_FORMAT_t* );

#if MYPROTOCOL_DEVICE_IS_COORD
extern bool MyprotocolD2WSendData( void*, void* );
#else
extern bool MyprotocolD2DSendData( void*, void* );
#define MyprotocolD2WSendData MyprotocolD2DSendData
#endif

/**@} */

/**
 * @name                  接收数据API
 * @param ctx             接收到的数据包
 * @param receive_func    数据包处理函数
 * @note                  建议使用函数MyprotocolReceiveData接收解析数据
 * @{
 */
#define MyprotocolW2DReceiveData( ctx, receive_func ) receive_func(ctx)
#define MyprotocolD2DReceiveData( ctx, receive_func ) receive_func(ctx)

extern bool MyprotocolReceiveData( void*, receive_type );
/**@} */

/**
 * @name    数据包函数
 * @{
 */
extern bool createCommErrorPacket( void*, void* );
extern bool createCommEndPacket( void*, void* );
extern bool createDeviceTickPacket( void*, void* );
extern bool createDeviceTickAckPacket( void*, void* );
extern bool createS2HWaitPacket( void*, void* );
extern bool createS2HAckPacket( void*, void* );
extern bool createH2SWaitPacket( void*, void* );
extern bool createH2SAckPacket( void*, void* );
extern bool createD2WWaitPacket( void*, void* );
extern bool createD2WAckPacket( void*, void* );
extern bool createW2DWaitPacket( void*, void* );
extern bool createW2DAckPacket( void*, void* );
extern bool createDeviceGetNTPPacket( void*, void* );
extern bool createDeviceGetNTPAckPacket( void*, void* );
extern bool createDeviceWrNTPPacket( void*, void* );
extern bool createDeviceWrNTPAckPacket( void*, void* );
/**@} */

#ifdef __cplusplus
}
#endif

#endif      /** __MY_IOT_PROTOCOL_H__ */

/**********************************END OF FILE*********************************/

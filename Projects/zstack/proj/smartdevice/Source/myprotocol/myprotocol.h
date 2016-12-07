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

/* Exported macro ------------------------------------------------------------*/
/** 程序调试宏 */
#define USE_MYPROTOCOL_DEBUG 1

/** 定义支持设备清单 */
#define MYPROTOCOL_DEVICE_COORD     (0x00)
#define MYPROTOCOL_DEVICE_LIGHT     (0x01)
#define MYPROTOCOL_DEVICE_SOCKET    (0x02)
#define MYPROTOCOL_DEVICE_CURTAIN   (0x03)
#define MYPROTOCOL_DEVICE_HT_SENSOR (0x04)

/** 定义当前设备类型 */
#define SMART_DEVICE_TYPE MYPROTOCOL_DEVICE_COORD

/**
 * @name 定义通信相关参数
 * @{
 */
/** 数据包所占内存空间 */
#define MYPROTOCOL_PACKET_SIZE                   (sizeof(MYPROTOCOL_FORMAT_t)/sizeof(uint8))

/** 设备信息所占内存空间 */
#define MYPROTOCOL_DEVICE_INFO_SIZE              (sizeof(MYPROTOCOL_DEVICE_INFO_t)/sizeof(uint8))

/** 计算user_data在MYPROTOCOL_FORMAT_t中的偏移量 */
#define MYPROTOCOL_USER_DATA_OFFSET              ((uint8)(&((MYPROTOCOL_FORMAT_t *)(0))->user_data))

/** 计算cmd在MYPROTOCOL_USER_DATA_t中的偏移量 */
#define MTPROTOCOL_USER_CMD_OFFSET               ((uint8)(&((MYPROTOCOL_USER_DATA_t *)(0))->user_data.cmd))

/** 计算len在MYPROTOCOL_USER_DATA_t中的偏移量 */
#define MYPROTOCOL_USER_DATA_LEN_OFFSET          ((uint8)(&((MYPROTOCOL_USER_DATA_t *)(0))->user_data.len))

/** 计算data在MYPROTOCOL_USER_DATA_t中的偏移量 */
#define MYPROTOCOL_USER_DATA_DATA_OFFSET         ((uint8)(&((MYPROTOCOL_USER_DATA_t *)(0))->user_data.data))

/** 计算user_data所占内存空间大小 user_data为MYPROTOCOL_USER_DATA_t类型的结构体指针*/
#define MYPROTOCOL_CAL_USER_DATA_SIZE(user_data) ((uint8)(MYPROTOCOL_USER_DATA_LEN_OFFSET \
                                                          + ((MYPROTOCOL_USER_DATA_t *)(user_data))->len))

/** 计算user_message所占内存空间大小 user_data为MYPROTOCOL_USER_DATA_t类型的结构体指针*/                                                          
#define MYPROTOCOL_CAL_USER_MSG_SIZE(user_data)  ((uint8)((uint8)MYPROTOCOL_DEVICE_INFO_SIZE \
                                                          + MYPROTOCOL_USER_DATA_LEN_OFFSET \
                                                          + ((MYPROTOCOL_USER_DATA_t *)(user_data))->len))

/** 通讯格式 用户数据大小 */
#define MYPROTOCOL_USER_DATA_SIZE   (18)

/** 通讯格式 MAC地址大小 */
#define MYPROTOCOL_MAC_ADDR_SIZE    (8)

/** 定义通讯方向 */
// #define MYPROTOCOL_COMM_END          (0x00)
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
 * @name MYPROTOCOL相关功能接口定义
 * @{
 */
typedef bool (*packet_type)(void *ctx, void *packet);
typedef bool (*send_type)(void *ctx, void *packet);
typedef bool (*receive_type)(void *ctx, void *packet);
/**@} */

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
    MYPROTOCOL_DEVICE_INFO *info;
}MYPROTOCOL_DEVCICE_ACK_t;

/** 用户信息 */
typedef struct
{
    MYPROTOCOL_DEVICE_INFO deviceInfo;
    MYPROTOCOL_USER_DATA   userData;
}MYPROTOCOL_USER_MESSAGE_t;

/** MYPROTOCOL 格式 */
typedef struct
{
    MYPROTOCOL_COMMTYPE    commtype;
    uint8                  sn;
    MYPROTOCOL_DEVICE_INFO device;
    MYPROTOCOL_USER_DATA   user_data;
    uint8                  sum;
}MYPROTOCOL_FORMAT_t;

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
extern bool HalMyprotocolInit( uint8* );
#else
extern void HalMyprotocolInit( uint8* );
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
 * @name 发送数据API
 * @note 建议使用函数MyprotocolSendData发送数据
 * @{
 */
extern bool MyprotocolD2DSendData( void*, void* );
extern bool MyprotocolD2WSendData( void*, void* );
extern bool MyprotocolForwardData( void*, void*, send_type );
extern bool MyprotocolSendData( void*, void*, packet_type, send_type );
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

extern bool MyprotocolReceiveData( void*, void*, receive_type, receive_type );
/**@} */

/**
 * @name    数据包函数
 * @{
 */
extern bool CommErrorPacket( void*, void* );
extern bool DeviceTickPacket( void*, void* );
extern bool DeviceTickAckPacket( void*, void* );
extern bool S2HWaitPacket( void*, void* );
extern bool S2HAckPacket( void*, void* );
extern bool H2SWaitPacket( void*, void* );
extern bool H2SAckPacket( void*, void* );
extern bool D2WWaitPacket( void*, void* );
extern bool D2WAckPacket( void*, void* );
extern bool W2DWaitPacket( void*, void* );
extern bool W2DAckPacket( void*, void* );
/**@} */

#ifdef __cplusplus
}
#endif

#endif      /** __MY_IOT_PROTOCOL_H__ */

/**********************************END OF FILE*********************************/

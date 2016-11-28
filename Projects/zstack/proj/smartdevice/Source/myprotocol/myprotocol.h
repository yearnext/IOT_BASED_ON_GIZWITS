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
#ifndef __MY_PROTOCOL_H__
#define __MY_PROTOCOL_H__

/* Includes ------------------------------------------------------------------*/
#include "comdef.h"
#include "AF.h"

/* Exported macro ------------------------------------------------------------*/
/** 通讯格式 用户数据大小 */
#define MYPROTOCOL_USER_DATA_SIZE  (18)
/** 通讯格式 MAC地址大小 */
#define MYPROTOCOL_MAC_ADDR_SIZE   (8)
// 设备心跳命令
#define MYPROTOCOL_TICK_CMD        (0x00)
// 获取设备信息命令
#define W2D_DEVICE_LIST_UPDATE_CMD (0x10)
#define W2D_GET_DEVICE_NUM_CMD     (0x11)
#define W2D_GET_DEVICE_INFO_CMD    (0x12)

//设备类型
#define MYPROTOCOL_DEVICE_COORD     (0x00)
#define MYPROTOCOL_DEVICE_LIGHT     (0x01)
#define MYPROTOCOL_DEVICE_SOCKET    (0x02)
#define MYPROTOCOL_DEVICE_CURTAIN   (0x03)
#define MYPROTOCOL_DEVICE_HT_SENSOR (0x04)

/* Exported types ------------------------------------------------------------*/
// 通讯类型
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
}myprotocol_commtype;

// 设备信息
typedef struct
{
    uint8 device;
    uint8 mac[MYPROTOCOL_MAC_ADDR_SIZE];
}MYPROTOCOL_DEVICE_INFO;

// 用户数据
typedef struct
{
    uint8 cmd;
    uint8 len;
    uint8 data[MYPROTOCOL_USER_DATA_SIZE];
}MYPROTOCOL_USER_DATA;
    
// 设备应答
typedef struct
{
    uint8 id;
    MYPROTOCOL_DEVICE_INFO *info;
}MYPROTOCOL_DEVCICE_ACK;

// MYPROTOCOL 格式
typedef struct
{
    myprotocol_commtype    commtype;
    uint8                  sn;
    MYPROTOCOL_DEVICE_INFO device;
    MYPROTOCOL_USER_DATA   user_data;
    uint8                  check_sum;
}MYPROTOCOL_FORMAT;

// 数据发送方向
typedef enum
{
    MYPROTOCOL_DIR_W2D,
    MYPROTOCOL_DIR_D2W,
    MYPROTOCOL_DIR_S2H,
    MYPROTOCOL_DIR_H2S,
    MYPROTOCOL_FORWORD_D2W,
    MYPROTOCOL_FORWORD_W2D,
}MYPROTOCOL_DATA_DIR;

// 创建数据包函数
typedef bool (*packet_func)(void *ctx, MYPROTOCOL_FORMAT *packet);

// 设备基本命令
typedef enum
{
    DEVICE_TICK   = 0x00,
    DEVICE_RESET  = 0x01,
    DEVICE_REBOOT = 0x02,
}DEVICE_BASE_CMD;

/* Exported variables --------------------------------------------------------*/
// 设备类型
#define SMART_DEVICE_TYPE MYPROTOCOL_DEVICE_COORD

/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
// MYPROTOCOL初始化函数
extern void myprotocol_init( uint8 *task_id );

// 数据包解析函数
extern void SmartDevice_Message_Handler( afIncomingMSGPacket_t *pkt );
extern void Gizwits_Message_Handler( uint8 *report_data, uint8 *packet_data );

// 数据包发送函数
extern bool MYPROTOCO_S2H_MSG_SEND( packet_func, void * );
extern bool MYPROTOCO_H2S_MSG_SEND( MYPROTOCOL_DEVICE_INFO, packet_func, void* );
extern bool MYPROTOCOL_FORWARD_PACKET( MYPROTOCOL_DATA_DIR, MYPROTOCOL_FORMAT* );
extern bool MYPROTOCOL_D2W_SEND_MSG( MYPROTOCOL_FORMAT*, packet_func, void* );
extern bool MYPROTOCOL_SEND_MSG( MYPROTOCOL_DATA_DIR, MYPROTOCOL_FORMAT*, packet_func, void* );

// 创建数据包函数
extern bool create_tick_packet( void *ctx, MYPROTOCOL_FORMAT *packet );
extern bool create_acktick_packet( void *ctx, MYPROTOCOL_FORMAT *packet );
extern bool create_errcode_packet( void *ctx, MYPROTOCOL_FORMAT *packet );
extern bool create_commend_packet( void *ctx, MYPROTOCOL_FORMAT *packet );
extern bool create_w2d_ack_packet( void *ctx, MYPROTOCOL_FORMAT *packet );
extern bool create_w2d_wait_packet( void *ctx, MYPROTOCOL_FORMAT *packet );
extern bool create_d2w_ack_packet( void *ctx, MYPROTOCOL_FORMAT *packet );
extern bool create_d2w_wait_packet( void *ctx, MYPROTOCOL_FORMAT *packet );
extern bool create_devicenum_packet( void *ctx, MYPROTOCOL_FORMAT *packet );
extern bool create_deviceinfo_packet( void *ctx, MYPROTOCOL_FORMAT *packet );
extern bool create_devicelist_update_packet( void *ctx, MYPROTOCOL_FORMAT *packet );
extern bool create_report_packet( void *ctx, MYPROTOCOL_FORMAT *packet );

#endif      /* __myprotocol_H__ */

/**********************************END OF FILE*********************************/

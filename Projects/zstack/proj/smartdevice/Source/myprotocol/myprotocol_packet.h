/**
 ******************************************************************************
  * @file       myprotocol_packet.h
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      myprotocol_packet 头文件
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
#ifndef __MY_PROTOCOL_PACKET_H__
#define __MY_PROTOCOL_PACKET_H__

/* Includes ------------------------------------------------------------------*/
#include "comdef.h"
#include "myprotocol.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
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

#endif      /* __myprotocol_packet_H__ */

/**********************************END OF FILE*********************************/

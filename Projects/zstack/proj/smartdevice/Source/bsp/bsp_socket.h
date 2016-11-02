/**
 ******************************************************************************
  * @file       bsp_socket.h
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      插座配置 头文件
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
#ifndef __SMART_DEVICE_SOCKET_H__
#define __SMART_DEVICE_SOCKET_H__

/* Includes ------------------------------------------------------------------*/
#include "comdef.h"
#include "myprotocol.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
// 灯的开启关闭亮度
#define SOCKET_ON_CMD  ( 255 )
#define SOCKET_OFF_CMD ( 0 )

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
extern void bsp_socket_init( void );
extern void set_socket_value( uint8 );
extern uint8 get_socket_value( void );
extern void socket_control_handler( uint8 );
extern void socket_switch_headler( void );
extern void socket_working_headler( void );
extern bool socket_cmd_resolve( MYPROTOCOL_USER_DATA *data );

#endif      /* __SMART_DEVICE_socket_H__ */

/**********************************END OF FILE*********************************/

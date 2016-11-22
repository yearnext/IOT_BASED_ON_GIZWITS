/**
 ******************************************************************************
  * @file       bsp_curtain.h
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      窗帘配置 头文件
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
#ifndef __SMART_DEVICE_CURTAIN_H__
#define __SMART_DEVICE_CURTAIN_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "comdef.h"
#include "myprotocol.h"
#include "bsp_key.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
extern void bsp_curtain_init( void );

#if (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_CURTAIN)
#define curtain_switch_handler key1_handler
#define curtain_rain_handler   key2_handler

extern void curtain_switch_handler( key_message_t );
extern void curtain_rain_handler( key_message_t );

#endif

extern bool curtain_cmd_resolve( MYPROTOCOL_USER_DATA *data );

#ifdef __cplusplus
}
#endif

#endif      /* __SMART_DEVICE_CURTAIN_H__ */

/**********************************END OF FILE*********************************/

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
#if MYPROTOCOL_DEVICE_IS_CURTAIN
#define bspDeviceInit        bspCurtainInit
#define deviceMessageHandler curtainMessageHandler
#endif
  
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
extern void bspCurtainInit( void );
extern bool curtainControlCmd( uint8 );
#if MYPROTOCOL_DEVICE_IS_CURTAIN
#define curtainSwitchKeyHandler key1_handler
#define curtainRainKeyHandler   key2_handler
#endif
extern void curtainSwitchKeyHandler( key_message_t );
extern void curtainRainKeyHandler( key_message_t );

extern void curtainWorkingHandler( void );
extern bool curtainMessageHandler( MYPROTOCOL_FORMAT_t *recPacket );

#ifdef __cplusplus
}
#endif

#endif      /* __SMART_DEVICE_CURTAIN_H__ */

/**********************************END OF FILE*********************************/

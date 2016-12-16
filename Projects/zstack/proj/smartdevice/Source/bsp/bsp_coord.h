/**
 ******************************************************************************
  * @file       bsp_coord.h
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      bsp_coord 头文件
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
#ifndef __SMART_DEVICE_COORD_H__
#define __SMART_DEVICE_COORD_H__

#ifdef __cplusplus
extern "C" 
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "comdef.h"
#include "bsp_key.h"

/* Exported macro ------------------------------------------------------------*/
#if MYPROTOCOL_DEVICE_IS_COORD
#define bspDeviceInit bspCoordInit
#define deviceMessageHandler coordMessageHandler
#endif

/* Exported types ------------------------------------------------------------*/ 
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
extern void bspCoordInit(void);

#if MYPROTOCOL_DEVICE_IS_COORD
#define coordRstWIFIKeyHandler    key1_handler
#define coordUpdateTimeKeyHandler key2_handler
#endif

extern void coordRstWIFIKeyHandler( key_message_t );
extern void coordUpdateTimeKeyHandler( key_message_t );

extern bool coordMessageHandler( MYPROTOCOL_FORMAT_t* );

extern bool createDeviceListChangePacket( void*, void* );

#ifdef __cplusplus
extern "C" 
}
#endif

#endif      /* __SMART_DEVICE_COORD_H__ */

/**********************************END OF FILE*********************************/

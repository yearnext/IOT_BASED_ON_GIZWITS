/**
 ******************************************************************************
  * @file       bsp_light.h
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      智能电灯配置头文件
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
#ifndef __SMART_DEVICE_LIGHT_H__
#define __SMART_DEVICE_LIGHT_H__

/* Includes ------------------------------------------------------------------*/
#include "comdef.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
// 灯的开启关闭亮度
#define Light_ON_Brightness            ( 255 )
#define Light_OFF_Brightness           ( 0 )

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
extern void bsp_light_init( void );
extern void light_brightness_set( uint8 brightness );
extern uint8 light_brightness_get( void );

#endif      /* __CC2530_TIMER_CONFIG_H__ */

/**********************************END OF FILE*********************************/

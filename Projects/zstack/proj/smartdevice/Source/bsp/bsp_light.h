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
#if (SMART_DEVICE_TYPE) == (MYPROTOCOL_DEVICE_LIGHT)
#include "comdef.h"
#include "myprotocol.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
// 灯的最大亮度/最小亮度
#define LIGHT_MAX_BRIGHTNESS           (100)
#define LIGHT_MIN_BRIGHTNESS           (0)
// 灯的开启亮度/关闭亮度
#define LIGHT_ON_BRIGHTNESS            (100)  
#define LIGHT_OFF_BRIGHTNESS           (0)

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
extern void bsp_light_init( void );
extern void set_light_brightness( uint8 );
extern uint8 get_light_brightness( void );
extern void light_control_handler( uint8 );
extern void light_switch_handler( void );
extern void light_working_handler( void );
extern bool light_cmd_resolve( MYPROTOCOL_USER_DATA *data );

#endif
#endif      /* __SMART_DEVICE_LIGHT_H__ */

/**********************************END OF FILE*********************************/

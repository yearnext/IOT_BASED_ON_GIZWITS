/**
 ******************************************************************************
  * @file       gizwits.h
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      gizwits 头文件
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

#if defined ( USE_GIZWITS_MOD )

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_GIZWITS_H__
#define __APP_GIZWITS_H__

/* Includes ------------------------------------------------------------------*/
#include "ZComDef.h"
#include "gizwits_product.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
extern void app_gizwits_init( void );

extern uint16 Gizwits_ProcessEven( uint8 task_id, uint16 events );
#endif      /* __APP_GIZWITS_H__ */

#endif

/**********************************END OF FILE*********************************/

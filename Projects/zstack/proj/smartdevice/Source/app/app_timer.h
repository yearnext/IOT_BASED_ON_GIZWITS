/**
 ******************************************************************************
  * @file       app_timer.h
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      定时器应用头文件
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
#ifndef __APP_USER_TIMER_H__
#define __APP_USER_TIMER_H__

/* Includes ------------------------------------------------------------------*/
#include "comdef.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
// 定时器自定义工作类型
typedef struct
{
    uint8 status    : 1;
    uint8 monday    : 1;
    uint8 tuesday   : 1;
    uint8 wednesday : 1;
    uint8 thursday  : 1;
    uint8 friday    : 1;
    uint8 saturday  : 1;
    uint8 sunday    : 1;
}TIMER_CUSTOM_MODE;

// 设备定时器工作模式
typedef struct
{
    uint8 status;
    uint8 single_mode;
    TIMER_CUSTOM_MODE custom_mode;
}DEVICE_TIMER_MODE;
// 设备定时器类型
typedef struct
{
    // res
    uint8 timer_type;
    // use
    DEVICE_TIMER_MODE mode;
    uint8 device_status;
    uint8 start_hour;
    uint8 start_minute;
    uint8 stop_hour;
    uint8 stop_minute;
}DEVICE_TIMER;

// 设备状态信号
typedef enum
{
    DEVICE_START_SIGNAL,
    DEVICE_STOP_SIGNAL,
    DEVICE_KEEP_SIGNAL,
}DEVICE_STATUS_SIGNAL;

/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
extern DEVICE_STATUS_SIGNAL device_timer_check( DEVICE_TIMER timer );

#endif      /* __APP_USER_TIMER_H__ */

/**********************************END OF FILE*********************************/

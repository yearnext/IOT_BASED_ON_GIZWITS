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
//// 定时器工作类型
//#define TIMER_COUNTDOWN_TYPE (0xA1)
//#define TIMER_GENERAL_TYPE   (0xA2)
//// 定时器工作模式
//#define TIMER_SIGNAL_MODE    (0xF1)
//#define TIMER_CUSTOM_MODE    (0xF2)

/* Exported types ------------------------------------------------------------*/
// 定时器类型
//typedef enum
//{
//    TIMER_COUNTDOWN_TYPE = 0xA1,
//    TIMER_GENERAL_TYPE   = 0xA2,
//}TIMER_TYPE;

// 定时器工作模式
typedef enum
{
	TIMER_SLEEP_MODE = 0x00,
	TIMER_DOWNCNT_MODE_WAIT = 0xB1,
	TIMER_PERIOD_MODE_WAIT = 0xB2,
	TIMER_CUSTOM_MODE_WAIT = 0xB3,
	TIMER_DOWNCNT_MODE,
	TIMER_PERIOD_MODE ,
	TIMER_CUSTOM_MODE ,
}TIMER_MODE;

// 定时器自定义工作类型
// 定时器自定义工作类型
typedef struct
{
	const uint8 res : 1;
	uint8 monday     : 1;
	uint8 tuesday    : 1;
	uint8 wednesday  : 1;
	uint8 thursday   : 1;
	uint8 friday     : 1;
	uint8 saturday   : 1;
	uint8 sunday     : 1;
}TIMER_CUSTOM_PARAM;

// 定时器工作时间
typedef struct
{
	uint16 start;
	uint16 end;
}TIMER_WOKRING_TIME;

// 定时器工作时间
typedef struct
{
	uint8 start;
	uint8 end;
}TIMER_WOKRING_STATUS;

// 设备定时器类型
typedef struct
{
	TIMER_MODE mode;
	TIMER_CUSTOM_PARAM custom;
	TIMER_WOKRING_TIME time;
	TIMER_WOKRING_STATUS status;
}DEVICE_TIMER;

// 设备状态信号
typedef enum
{
    DEVICE_START_SIGNAL,
    DEVICE_STOP_SIGNAL,
    DEVICE_KEEP_SIGNAL,
}DEVICE_STATUS_SIGNAL;

// 设备控制函数
typedef void (*device_timer_func)(uint8);

/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
extern bool device_timer_handler(DEVICE_TIMER*, device_timer_func);

#endif      /* __APP_USER_TIMER_H__ */

/**********************************END OF FILE*********************************/

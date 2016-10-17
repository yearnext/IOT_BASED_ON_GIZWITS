 /**
 ******************************************************************************
  * @file       app_timer.c
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      定时器应用源文件
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

/**
 * @defgroup 定时器应用模块
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "app_time.h"
#include "app_timer.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ONE_DAY_MINUTES ( 24*60 )
#define Time_Hour2Minute(hour) ((hour)*60)
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
DEVICE_STATUS_SIGNAL device_timer_check( DEVICE_TIMER timer )
{
    user_time time = app_get_time();
    uint16 start_time = Time_Hour2Minute(timer.start_hour)+timer.start_minute;
    uint16 stop_time  = Time_Hour2Minute(timer.stop_hour)+timer.stop_minute;
    uint16 now_time = Time_Hour2Minute(time.hour) + time.minute;
    DEVICE_STATUS_SIGNAL status = DEVICE_KEEP_SIGNAL;
    
    if( start_time < stop_time )
    {
        if( now_time >= stop_time )
        {
            status = DEVICE_STOP_SIGNAL;
        }
        else if( now_time >= start_time )
        {
            status = DEVICE_START_SIGNAL;
        }
        else
        {
            /** do nothing */
        }
    }
    else
    {
        
    }
    
    
    
    return status;
}

/** @}*/     /* 定时器应用模块 */

/**********************************END OF FILE*********************************/

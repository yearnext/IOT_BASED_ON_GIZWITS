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
#include <string.h>

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ONE_DAY_MINUTES ( 24*60 )
#define Time_Hour2Minute(hour) ((hour)*60)
#define TIMER_CUSTOM_BV(n)     ( 0x01 << (n) )

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief       设备运行状态判定函数
 * @param       [in/out]  timer    当前定时器的状态
 * @return      [in/out]  status   定时器动作信号
 * @note        None
 *******************************************************************************
 */
DEVICE_STATUS_SIGNAL device_timer_check( DEVICE_TIMER timer )
{
    user_time time = app_get_time();
    
    uint16 start_time = Time_Hour2Minute(timer.time.start_hour)+timer.time.start_minute;
    uint16 stop_time  = Time_Hour2Minute(timer.time.stop_hour)+timer.time.stop_minute;
    uint16 now_time = Time_Hour2Minute(time.hour) + time.minute;
    
    uint8 mode = *((uint8 *)&timer.custom);
    DEVICE_STATUS_SIGNAL status = DEVICE_KEEP_SIGNAL;
    
    
    if( timer.mode != TIMER_SLEEP_MODE )
    {
        if( start_time < stop_time )
        {
            if( now_time >= stop_time )
            {
                status = DEVICE_STOP_SIGNAL;
            }
            else if( now_time >= start_time )
            {
                if( (timer.mode == TIMER_SIGNAL_WAIT_MODE) \
                    || ((timer.mode == TIMER_CUSTOM_WAIT_MODE) \
                        && (mode & TIMER_CUSTOM_BV(time.week)) \
                        && (timer.custom.status)) )
                {
                    status = DEVICE_START_SIGNAL;
                }
            }
            else
            {
                status = DEVICE_KEEP_SIGNAL;
            }
        }
        else
        {
            if( now_time >= start_time )
            {
                if( (timer.mode == TIMER_SIGNAL_WAIT_MODE) \
                    || ((timer.mode == TIMER_CUSTOM_WAIT_MODE) \
                        && (mode & TIMER_CUSTOM_BV(time.week)) \
                        && (timer.custom.status)) )
                {
                    status = DEVICE_START_SIGNAL;
                }
            }
            else if( now_time >= stop_time )
            {
                status = DEVICE_STOP_SIGNAL;
            }
            else
            {
                status = DEVICE_KEEP_SIGNAL;
            }
        }
    }
    
    return status;
}
         
/**
 *******************************************************************************
 * @brief       定时器处理函数
 * @param       [in/out]  *timer    当前定时器的配置
 *              [in/out]  func      定时器功能回调函数
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void device_timer_headler( DEVICE_TIMER *timer, device_timer_func func )
{
    DEVICE_TIMER timer_bkp;
    DEVICE_STATUS_SIGNAL status;
    
    memcpy(&timer_bkp,timer,sizeof(DEVICE_TIMER));
    status = device_timer_check(timer_bkp);
    
    switch( timer->mode )
    {
        case TIMER_SIGNAL_WAIT_MODE:
            if( status == DEVICE_START_SIGNAL )
            {
                timer->mode = TIMER_SIGNAL_MODE;
                func(timer->device_start_status);
            }
            break;
        case TIMER_CUSTOM_WAIT_MODE:
            if( status == DEVICE_START_SIGNAL )
            {
                timer->mode = TIMER_CUSTOM_MODE;
                func(timer->device_start_status);
            }
            break;
        case TIMER_SIGNAL_MODE:
            if( status == DEVICE_STOP_SIGNAL )
            {
                timer->mode = TIMER_SLEEP_MODE;
                func(timer->device_end_status);
            }
            break;
        case TIMER_CUSTOM_MODE:
            if( status == DEVICE_STOP_SIGNAL )
            {
                timer->mode = TIMER_CUSTOM_WAIT_MODE;
                func(timer->device_end_status);
            }
            break;
        default:
            break;
    }
}

/** @}*/     /* 定时器应用模块 */

/**********************************END OF FILE*********************************/

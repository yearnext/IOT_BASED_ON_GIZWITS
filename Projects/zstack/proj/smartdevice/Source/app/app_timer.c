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
// 时间相关宏
#define ONE_DAY_MINUTES        ( 24*60 )
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
 * @return      [in/out]  time     当前时间信息
 * @note        None
 *******************************************************************************
 */
static DEVICE_STATUS_SIGNAL devicePeriodTimerInquire(TIMER_WOKRING_TIME *timer, uint16 time)
{
    uint16 start_time = Time_Hour2Minute(timer->start_hour)+timer->start_minute;
    uint16 end_time = Time_Hour2Minute(timer->end_hour)+timer->end_minute;
    
	if (start_time < end_time)
	{
		if (time >= end_time)
		{
			return DEVICE_STOP_SIGNAL;
		}
		else if (time >= start_time)
		{
			return DEVICE_START_SIGNAL;
		}
		else
		{
			return DEVICE_KEEP_SIGNAL;
		}
	}

	return DEVICE_KEEP_SIGNAL;
}

/**
 *******************************************************************************
 * @brief       设备运行状态判定函数
 * @param       [in/out]  timer    当前定时器的状态
 * @return      [in/out]  time     当前时间信息
 * @note        None
 *******************************************************************************
 */
static DEVICE_STATUS_SIGNAL deviceDowncntTimerInquire(TIMER_WOKRING_TIME *timer, uint16 time)
{
    uint16 start_time = Time_Hour2Minute(timer->start_hour)+timer->start_minute;
    uint16 end_time = Time_Hour2Minute(timer->end_hour)+timer->end_minute;
    
	if (end_time)
	{
// 		if (timer->start == time)
// 		{
// 			return DEVICE_START_SIGNAL;
// 		} 

		uint16 past_time = time - start_time;
		timer->start_hour = time/60;
        timer->start_minute = time&60;

		if (end_time > past_time)
		{
			end_time -= past_time;
            timer->end_hour = end_time/60;
            timer->end_minute = end_time%60;
			return DEVICE_KEEP_SIGNAL;
		} 
		else
		{
			timer->end_hour = 0;
            timer->end_minute = 0;
			timer->start_hour = 0;
            timer->start_minute = 0;
			return DEVICE_STOP_SIGNAL;
		}
	}

	return DEVICE_KEEP_SIGNAL;
}

/**
 *******************************************************************************
 * @brief       设备运行状态判定函数
 * @param       [in/out]  timer    当前定时器的状态
 * @return      [in/out]  func     定时器执行功能
 * @note        None
 *******************************************************************************
 */
bool deviceTimerHandler(DEVICE_TIMER *timer, device_timer_func func)
{
	user_time time = app_get_time();
	uint16 now_time = Time_Hour2Minute(time.hour) + time.minute;

	switch (timer->mode)
	{
		case TIMER_SLEEP_MODE:
			return true;
			break;
		case TIMER_PERIOD_MODE_WAIT:
			if (devicePeriodTimerInquire(&timer->time,now_time) == DEVICE_START_SIGNAL)
			{
				func(timer->status.start);
				timer->mode = TIMER_PERIOD_MODE;
			}
			break;
		case TIMER_PERIOD_MODE:
			if (devicePeriodTimerInquire(&timer->time, now_time) == DEVICE_STOP_SIGNAL)
			{
				func(timer->status.end);
				timer->mode = TIMER_SLEEP_MODE;
			}
			break;
		case TIMER_DOWNCNT_MODE_WAIT:
			func(timer->status.start);
			timer->mode = TIMER_DOWNCNT_MODE;
		case TIMER_DOWNCNT_MODE:
			if (deviceDowncntTimerInquire(&timer->time, now_time) == DEVICE_STOP_SIGNAL)
			{
				func(timer->status.end);
				timer->mode = TIMER_SLEEP_MODE;
			}
			break;		
		case TIMER_CUSTOM_MODE_WAIT:
		{
			uint8 week = *(uint8 *)(&timer->custom);
			if (( week & TIMER_CUSTOM_BV(time.week)) \
				&&(devicePeriodTimerInquire(&timer->time, now_time) == DEVICE_START_SIGNAL))
			{
				func(timer->status.start);
				timer->mode = TIMER_CUSTOM_MODE;
			}
			break;
		}
		case TIMER_CUSTOM_MODE:
		{
			uint8 week = *(uint8 *)(&timer->custom);
			if (week & TIMER_CUSTOM_BV(time.week))
			{
				if (devicePeriodTimerInquire(&timer->time, now_time) == DEVICE_STOP_SIGNAL)
				{
					func(timer->status.end);
					timer->mode = TIMER_CUSTOM_MODE_WAIT;
				} 
			}
			else
			{
				func(timer->status.end);
				timer->mode = TIMER_CUSTOM_MODE_WAIT;
			}
			break;
		}	
		default:
			break;
	}

	return false;
}

/** @}*/     /* 定时器应用模块 */

/**********************************END OF FILE*********************************/

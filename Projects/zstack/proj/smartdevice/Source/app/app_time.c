 /**
 ******************************************************************************
  * @file       app_time.c
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      时间 应用源文件
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
 * @defgroup 时间应用模块
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "app_time.h"
#include <string.h>
#include "hal_ds1302.h"
#include "gizwits_protocol.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define hal_timechip_init() hal_ds1302_init()  
#define hal_time_set(time)  ds1302_wr_time(time)
#define hal_time_read(time) ds1302_rd_time(time)
     
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static user_time now_time;

/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief       时间初始化程函数
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void app_time_init( void )
{
    hal_timechip_init();
    
    app_time_update();
}

/**
 *******************************************************************************
 * @brief       时间刷新函数
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void app_time_update( void )
{
    hal_time_read(&now_time);
    
    if( getGizwitsM2MStatus() == true )
    {
        hal_time_set(gizwitsGetTime()); 
    }
}

/**
 *******************************************************************************
 * @brief       读取时间
 * @param       [in/out]  void
 * @return      [in/out]  time   读取到的时间数据
 * @note        None
 *******************************************************************************
 */
user_time app_get_time( void )
{
    return now_time;
}

/**
 *******************************************************************************
 * @brief       根据年月日数据计算星期
 * @param       [in/out]  time   时间数据
 * @return      [in/out]  uint8  计算出的星期数据
 * @note        None
 *******************************************************************************
 */
uint8 app_cal_week( user_time time )
{
    uint8  temp_month = 0;
	uint16 temp_year = 0;
	uint16 temp_week = 0;

    if (time.month <= 2)
	{
		temp_month = 12 + time.month;
		temp_year = time.year - 1;
	}
	else
	{
		temp_month = time.month;
		temp_year = time.year;
	}

	temp_week = temp_year%100;
	temp_week += temp_week / 4;
	temp_week += temp_year/100 / 4;
	temp_week -= temp_year/100 * 2;
	temp_week += time.day - 1;
	temp_week += 26 * (temp_month + 1) / 10;

	time.week = temp_week % 7;
    
    return time.week;
}

/** @}*/     /* 时间应用模块 */

/**********************************END OF FILE*********************************/

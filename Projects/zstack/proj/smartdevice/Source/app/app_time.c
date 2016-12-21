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
#define USE_HAL_TIME (0)

#if USE_HAL_TIME
#define hal_timechip_init() hal_ds1302_init()  
#define hal_time_set(time)  ds1302_wr_time(time)
#define hal_time_read(time) ds1302_rd_time(time)

#else
#define hal_timechip_init()
#define hal_time_set(time)
#define hal_time_read(time)

#endif

/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/  
#if !USE_HAL_TIME
    static user_time deviceTime;
#endif
 
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
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
    
    app_get_time();
    
#if !USE_HAL_TIME 
    memset(&deviceTime, 0, sizeof(deviceTime));
#endif
}

/**
 *******************************************************************************
 * @brief       时间刷新函数
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void app_time_update( user_time *time )
{
#if USE_HAL_TIME 
    time->year -= 2000;
    hal_time_set(time); 
#else
    memcpy(&deviceTime, time, sizeof(deviceTime));
#endif
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
#if !MYPROTOCOL_DEVICE_IS_COORD
#if USE_HAL_TIME 
    user_time time;
    
    memset(&time, 0, sizeof(time));
    
    hal_time_read(&time);
    
    time.year += 2000;

    return time;
#else
    return deviceTime;
#endif

#else  
    return gizwitsNTPConverUserTime();
#endif
}

/**
 *******************************************************************************
 * @brief       闰年信息判断
 * @param       [in/out]  year   闰年年号
 * @return      [in/out]  bool   判断结果
 * @note        None
 *******************************************************************************
 */
bool IsLeapYear( uint16 year )
{
    if( (year % 4) == 0 )
    {
        if( (year % 100) == 0 )
        {
            if( (year % 400) == 0 )
            {
                return true;
            }
        }
        else
        {
            return true;
        }
    }
    
    return false;
}

/**
 *******************************************************************************
 * @brief       时间转换为时间戳数据
 * @param       [in/out]  time       需要转换的时间数据
 * @return      [in/out]  timeStamp  时间戳数据
 * @note        None
 *******************************************************************************
 */
uint32 TimeConverTimeStamp( user_time *time )
{
    uint32 timeStamp = 0;
    uint16 i = 0;
    
    const uint8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};
    
    for(i=1970; i<time->year; i++)	//把所有年份的秒钟相加
	{
		if(IsLeapYear(time->year))
        {
            timeStamp += 31622400;//闰年的秒钟数
        }
		else
        {
            timeStamp += 31536000;			  //平年的秒钟数
        }
	}
    
	time->month -= 1;
    
	for( i=0; i<time->month; i++ )	   //把前面月份的秒钟数相加
	{
		timeStamp += (uint32)mon_table[i] * 86400; //月份秒钟数相加
        
		if( IsLeapYear(time->year) && i==1 )
        {
            timeStamp += 86400;//闰年2月份增加一天的秒钟数	  
        }
	}
    
	timeStamp += (uint32)(time->day-1)*86400;//把前面日期的秒钟数相加 
	timeStamp += (uint32)time->hour * 3600;//小时秒钟数
    timeStamp += (uint32)time->minute * 60;	 //分钟秒钟数
	timeStamp += time->second;//最后的秒钟加上去
    
    return timeStamp;
}


/**
 *******************************************************************************
 * @brief       时间戳数据转换为时间数据
 * @param       [in/out]  timeStamp    需要转换的时间戳数据
 * @return      [in/out]  nowTime      时间数据
 * @note        None
 *******************************************************************************
 */
user_time TimeStampConverTime( uint32 timeStamp )
{
    user_time nowTime;
    
    uint16 dayCount = 0;
    uint32 temp = timeStamp/86400;
    uint32 temp1 = 0;

    const uint8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};
    
    if( temp != dayCount )
    {
        dayCount = temp;
        temp1 = 1970;
        
        while( temp >= 365 )
		{				 
			if( IsLeapYear(temp1) )//是闰年
			{
				if(temp >= 366)
                {
                    temp -= 366;//闰年的天数
                }
				else
                {
                    temp1++;
                    break;
                }  
			}
			else
            {
                temp-=365;	  //平年 
            }
			temp1++;  
		}
        
        nowTime.year = temp1;//得到年份
		temp1 = 0;
        
		while( temp >= 28 )//超过了一个月
		{
			if( IsLeapYear(nowTime.year) && temp1==1 )//当年是不是闰年/2月份
			{
				if( temp >= 29)
                {
                    temp -= 29;//闰年的秒钟数
                }
				else 
                {
                    break;
                }
			}
			else 
			{
				if( temp>=mon_table[temp1] )
                {
                    temp -= mon_table[temp1];//平年
                }
				else
                {
                    break;
                }
			}
            
			temp1++;  
		}
		nowTime.month = temp1+1;	//得到月份
		nowTime.day = temp+1;  	//得到日期 
	}
    
	temp = timeStamp % 86400;     	//得到秒钟数  
	nowTime.hour = temp/3600;     	//小时
	nowTime.minute = (temp%3600)/60; 	//分钟	
	nowTime.second = (temp%3600)%60; 	//秒钟
	nowTime.week = app_cal_week(&nowTime);//获取星期 
    
    return nowTime;
}
 
/**
 *******************************************************************************
 * @brief       根据年月日数据计算星期
 * @param       [in/out]  time   时间数据
 * @return      [in/out]  uint8  计算出的星期数据
 * @note        None
 *******************************************************************************
 */
uint8 app_cal_week( user_time *time )
{
    uint8  temp_month = 0;
	uint16 temp_year = 0;
	uint16 temp_week = 0;

    if (time->month <= 2)
	{
		temp_month = 12 + time->month;
		temp_year = time->year - 1;
	}
	else
	{
		temp_month = time->month;
		temp_year = time->year;
	}

	temp_week = temp_year%100;
	temp_week += temp_week / 4;
	temp_week += temp_year/100 / 4;
	temp_week -= temp_year/100 * 2;
	temp_week += time->day - 1;
	temp_week += 26 * (temp_month + 1) / 10;

	time->week = temp_week % 7;
    
    return time->week;
}

/** @}*/     /* 时间应用模块 */

/**********************************END OF FILE*********************************/

/**
 ******************************************************************************
  * @file       app_time.h
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      时间应用头文件
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
#ifndef __APP_USER_TIME_H__
#define __APP_USER_TIME_H__

/* Includes ------------------------------------------------------------------*/
#include "comdef.h"
#include "app_save.h"
#include "myprotocol.h"

/* Exported macro ------------------------------------------------------------*/
#define deviceUpdateNTPTime() MyprotocolSendData(NULL,NULL,createDeviceGetNTPPacket,MyprotocolD2WSendData)

/* Exported types ------------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief       用户时间结构体
 * @note        可以根据硬件的数据传输顺序而更改
 *******************************************************************************
 */
typedef struct
{
    uint8 second;
    uint8 minute;
    uint8 hour;
    uint8 day;
    uint8 month;
    uint8 week;
    uint16 year;
}user_time;

/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
extern void app_time_init( void );
extern void app_time_update( user_time* );
extern user_time app_get_time( void );
extern bool clrTimeUpdateFlag( void );
extern bool IsLeapYear( uint16 );
extern uint32 TimeConverTimeStamp( user_time* );
extern user_time TimeStampConverTime( uint32 );
extern uint8 app_cal_week( user_time* );

#endif      /* __APP_USER_TIME_H__ */

/**********************************END OF FILE*********************************/

/**
 ******************************************************************************
  * @file       hal_ds1302.h
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      ds1302 驱动头文件
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
#ifndef __HAL_DS1302_H__
#define __HAL_DS1302_H__

/* Includes ------------------------------------------------------------------*/
#include "comdef.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef struct
{
    uint8 second;
    uint8 minute;
    uint8 hour;
    uint8 day;
    uint8 month;
    uint8 week;
    uint8 year;
}DS1302_TIME;

/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DS1302_RD_TIME_ADDR     (0x00)
#define DS1302_SEC_REG_ADDR     (0x00)
#define DS1302_MIN_REG_ADDR     (0x01)
#define DS1302_HR_REG_ADDR      (0x02)
#define DS1302_DATE_REG_ADDR    (0x03)
#define DS1302_MONTH_REG_ADDR   (0x04)
#define DS1302_DAY_REG_ADDR     (0x05)
#define DS1302_YEAR_REG_ADDR    (0x06)
#define DS1302_CONTROL_REG_ADDR (0x07)
#define DS1302_TRICK_REG_ADDR   (0x08)
#define DS1302_CLOCK_REG_ADDR   (0x1F)
#define DS1302_RAM_ADDR(n)      (0x20+n)

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
extern void hal_ds1302_init( void );
extern void ds1302_wr_data( uint8 wr_addr, uint8 wr_data );
extern uint8 ds1302_rd_data( uint8 rd_addr );
extern DS1302_TIME ds1302_rd_time( void );
extern void ds1302_wr_time( DS1302_TIME newtime );

#endif      /* __HAL_DS1302_H__ */

/**********************************END OF FILE*********************************/

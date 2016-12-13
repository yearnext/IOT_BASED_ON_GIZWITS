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
#include "myprotocol.h"
#include "app_time.h"

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
#if MYPROTOCOL_DEVICE_IS_LIGHT || MYPROTOCOL_DEVICE_IS_SOCKET
// SCK端口寄存器定义
#define DS1302_SCK_PORT     P0_6
#define DS1302_SCK_BV       BV(6)
#define DS1302_SCK_SEL      P0SEL
#define DS1302_SCK_DIR      P0DIR
#define DS1302_SCK_POLARITY ACTIVE_HIGH
// SDA端口寄存器定义
#define DS1302_SDA_PORT     P0_7
#define DS1302_SDA_BV       BV(7)
#define DS1302_SDA_SEL      P0SEL
#define DS1302_SDA_DIR      P0DIR
#define DS1302_SDA_POLARITY ACTIVE_HIGH
// RST端口寄存器定义
#define DS1302_RST_PORT     P1_0
#define DS1302_RST_BV       BV(0)
#define DS1302_RST_SEL      P1SEL
#define DS1302_RST_DIR      P1DIR
#define DS1302_RST_POLARITY ACTIVE_HIGH
// 启用DS1302
#define USE_HAL_DS1302      (1)

#elif MYPROTOCOL_DEVICE_IS_CURTAIN
// SCK端口寄存器定义
#define DS1302_SCK_PORT     P2_0
#define DS1302_SCK_BV       BV(0)
#define DS1302_SCK_SEL      P2SEL
#define DS1302_SCK_DIR      P2DIR
#define DS1302_SCK_POLARITY ACTIVE_HIGH
// SDA端口寄存器定义
#define DS1302_SDA_PORT     P1_7
#define DS1302_SDA_BV       BV(7)
#define DS1302_SDA_SEL      P1SEL
#define DS1302_SDA_DIR      P1DIR
#define DS1302_SDA_POLARITY ACTIVE_HIGH
// RST端口寄存器定义
#define DS1302_RST_PORT     P1_6
#define DS1302_RST_BV       BV(6)
#define DS1302_RST_SEL      P1SEL
#define DS1302_RST_DIR      P1DIR
#define DS1302_RST_POLARITY ACTIVE_HIGH

// 启用DS1302
#define USE_HAL_DS1302      (1)

#else
// 禁用DS1302
#define USE_HAL_DS1302      (0)

#endif

// 寄存器地址
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
#if USE_HAL_DS1302
extern void hal_ds1302_init( void );
extern void ds1302_wr_data( uint8 wr_addr, uint8 wr_data );
extern uint8 ds1302_rd_data( uint8 rd_addr );
extern void ds1302_rd_time( user_time *time );
extern void ds1302_wr_time( user_time *time );

#else
#define hal_ds1302_init() 
#define ds1302_wr_data( a, b ) 
#define ds1302_rd_data( a ) 
#define ds1302_rd_time( a ) 
#define ds1302_wr_time( a ) 

#endif

#endif      /* __HAL_DS1302_H__ */

/**********************************END OF FILE*********************************/

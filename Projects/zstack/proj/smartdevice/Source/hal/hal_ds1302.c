 /**
 ******************************************************************************
  * @file       hal_ds1302.c
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      ds1302 驱动源文件
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
 * @defgroup ds1302驱动模块
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "hal_ds1302.h"
#include <ioCC2530.h>
#include "hal_board_cfg.h"
#include "OnBoard.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
// SCK端口寄存器定义
#define DS1302_SCK_PORT     P0_5
#define DS1302_SCK_BV       BV(5)
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
#define DS1302_RST_PORT     P0_6
#define DS1302_RST_BV       BV(6)
#define DS1302_RST_SEL      P0SEL
#define DS1302_RST_DIR      P0DIR
#define DS1302_RST_POLARITY ACTIVE_HIGH
// SCK端口功能配置
#define DS1302_SCK_WrMode() ( DS1302_SCK_DIR |=  DS1302_SCK_BV )
#define DS1302_SCK_RdMode() ( DS1302_SCK_DIR &= ~DS1302_SCK_BV )
#define SET_DS1302_SCK()    ( DS1302_SCK_PORT = DS1302_SCK_POLARITY(1) )
#define RST_DS1302_SCK()    ( DS1302_SCK_PORT = DS1302_SCK_POLARITY(0) )
#define Read_DS1302_SCK()   ( DS1302_SCK_POLARITY(DS1302_SCK_PORT) )
// SDA端口功能配置
#define DS1302_SDA_WrMode() ( DS1302_SDA_DIR |=  DS1302_SDA_BV )
#define DS1302_SDA_RdMode() ( DS1302_SDA_DIR &= ~DS1302_SDA_BV )
#define SET_DS1302_SDA()    ( DS1302_SDA_PORT = DS1302_SDA_POLARITY(1) )
#define RST_DS1302_SDA()    ( DS1302_SDA_PORT = DS1302_SDA_POLARITY(0) )
#define Read_DS1302_SDA()   ( DS1302_SDA_POLARITY(DS1302_SDA_PORT) )
// RST端口功能配置
#define DS1302_RST_WrMode() ( DS1302_RST_DIR |=  DS1302_RST_BV )
#define DS1302_RST_RdMode() ( DS1302_RST_DIR &= ~DS1302_RST_BV )
#define SET_DS1302_RST()    ( DS1302_RST_PORT = DS1302_RST_POLARITY(1) )
#define RST_DS1302_RST()    ( DS1302_RST_PORT = DS1302_RST_POLARITY(0) )
#define Read_DS1302_RST()   ( DS1302_RST_POLARITY(DS1302_RST_PORT) )
// 延时功能定义
#define DS1302_DELAY()     Onboard_wait(1)

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief       DS1302初始化程函数
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void hal_ds1302_init( void )
{
    DS1302_RST_WrMode();
    RST_DS1302_RST();
    
    DS1302_SCK_WrMode();
    SET_DS1302_SCK();
    
    DS1302_SDA_WrMode();
    SET_DS1302_SDA();
    DS1302_SDA_RdMode();
}

/**
 *******************************************************************************
 * @brief       DS1302读取一字节
 * @param       [in/out]  void
 * @return      [in/out]  rd_data    读取到的数据
 * @note        None
 *******************************************************************************
 */
static uint8 ds1302_rd_byte( void )
{
    uint8 rd_data = 0;
    uint8 i = 0;
    
    DS1302_SDA_RdMode();
    
    for( i=0; i<8; i++ )
    {
        SET_DS1302_SCK();
        
        rd_data <<= 1;
        rd_data |= (uint8)Read_DS1302_SDA();
        DS1302_DELAY();
        
        RST_DS1302_SCK();
        DS1302_DELAY();
    }
    
    return rd_data;
}

/**
 *******************************************************************************
 * @brief       DS1302写入一字节
 * @param       [in/out]  wr_data    写入的数据
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
static void ds1302_wr_byte( uint8 wr_data )
{
    uint8 i = 0;
    
    DS1302_SDA_WrMode();
    
    for( i=0; i<8; i++ )
    {
        RST_DS1302_SCK();
        
        (wr_data & BV(i)) ? (SET_DS1302_SDA()) : (RST_DS1302_SDA());
        DS1302_DELAY();
        
        SET_DS1302_SCK();
        DS1302_DELAY();
    }

    DS1302_SDA_RdMode();
}

/**
 *******************************************************************************
 * @brief       DS1302写入数据
 * @param       [in/out]  wr_addr    写入的地址
 * @param       [in/out]  wr_data    写入的数据
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void ds1302_wr_data( uint8 wr_addr, uint8 wr_data )
{
    uint8 cmd_data = 0xC0 | ( wr_addr << 1 );
    
    SET_DS1302_RST();
    
    ds1302_wr_byte(cmd_data);
    ds1302_wr_byte(wr_data);
    
    RST_DS1302_RST();
}

/**
 *******************************************************************************
 * @brief       DS1302读取数据
 * @param       [in/out]  wr_addr    写入的地址
 * @param       [in/out]  wr_data    写入的数据
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
uint8 ds1302_rd_data( uint8 rd_addr )
{
    uint8 cmd_data = 0x81 | ( rd_addr << 1 );
    uint8 rd_data = 0;
    
    SET_DS1302_RST();
    
    ds1302_wr_byte(cmd_data);
    rd_data = ds1302_rd_byte();
    
    RST_DS1302_RST();
    
    return rd_data;
}

/** @}*/     /* ds1302驱动模块 */

/**********************************END OF FILE*********************************/

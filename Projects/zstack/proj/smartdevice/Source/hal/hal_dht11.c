 /**
 ******************************************************************************
  * @file       hal_dht11.c
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      DHT11 驱动源文件
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
 * @defgroup DHT11 驱动模块
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "hal_dht11.h"
#include <ioCC2530.h>
#include "hal_board_cfg.h"
#include "OnBoard.h"

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
// 硬件寄存定义
#define DHT11_PORT     P2_1
#define DHT11_BV       BV(1)
#define DHT11_SEL      P2SEL
#define DHT11_DIR      P2DIR
#define DHT11_POLARITY ACTIVE_HIGH
// 硬件功能定义
#define DHT11_PORT_WrMode() ( DHT11_DIR |=  DHT11_BV )
#define DHT11_PORT_RdMode() ( DHT11_DIR &= ~DHT11_BV )
#define SET_DHT11_PORT()    ( DHT11_PORT = 1 )
#define RST_DHT11_PORT()    ( DHT11_PORT = 0 )
#define Read_DHT11_PORT()   ( DHT11_POLARITY(DHT11_PORT) )
// 延时功能定义
#define DHT11_START_DELAY()     Onboard_wait(20000)
#define DHT11_END_START_DELAY() Onboard_wait(40)
#define DHT11_WORKING_DELAY()   Onboard_wait(30)

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief       DHT11初始化函数
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void hal_dht11_init( void )
{
    DHT11_PORT_RdMode();
}

/**
 *******************************************************************************
 * @brief       从DHT11读取1字节函数
 * @param       [in/out]  void
 * @return      [in/out]  uint8    读取到的数据
 * @note        None
 *******************************************************************************
 */
static uint8 Read_Byte_FromDHT11( void )
{
    uint8 read_data = 0;
    uint8 i = 0;
    uint8 timeout = 0;
    uint8 get_bit = 0;
    
    for( i=0; i<8; i++ )
    {
        // 等待端口被拉高
        timeout = 1;
        while( !Read_DHT11_PORT() && ++timeout );
        // 检测是否存在超时现象
        if( timeout == 0 )
        {
            return 0xFF;
        }
        // 等待30us
        DHT11_WORKING_DELAY();
        // 检测此次接收的位信号
        ( Read_DHT11_PORT() ) ? (get_bit = 1) : (get_bit = 0); 
        // 等待端口被拉低
        timeout = 1;
        while( Read_DHT11_PORT() && ++timeout );
        // 检测是否存在超时现象
        if( timeout == 0 )
        {
            return 0xFF;
        }
        // 写入当前读取到的位数据
        read_data <<= 1;
        read_data |= get_bit;
    }
    
    return read_data;
}

/**
 *******************************************************************************
 * @brief       温湿度数据刷新函数
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
DHT11_DATA_t dht11_rd_data( void )
{
    uint8 timeout = 0;
    uint8 i = 0;
    uint8 data_cache[5];
    uint8 check_sum = 0;
    DHT11_DATA_t dht11_data = {0xFF, 0xFF};
    
    DHT11_PORT_WrMode();
    
    RST_DHT11_PORT();
    DHT11_START_DELAY();
    SET_DHT11_PORT();
    DHT11_PORT_RdMode();
    DHT11_END_START_DELAY();
    
    if( !Read_DHT11_PORT() )
    {
        // 等待端口被拉低
        timeout = 1;
        while( !Read_DHT11_PORT() && ++timeout );
        // 检测是否存在超时现象
        if( timeout == 0 )
        {
            return dht11_data;
        }
        // 等待端口被拉低
        timeout = 1;
        while( Read_DHT11_PORT() && ++timeout );
        // 检测是否存在超时现象
        if( timeout == 0 )
        {
            return dht11_data;
        }
        // 接收数据
        for( i=0; i<5; i++ )
        {
            data_cache[i] = Read_Byte_FromDHT11();
            
            if( i == 4 )
            {
                if( check_sum == data_cache[i] )
                {
                    dht11_data.temp = data_cache[0];
                    dht11_data.hum  = data_cache[2];
                    break;
                }
            }
            else
            {
                check_sum |= data_cache[i];
            }
        }
    }
    
    return dht11_data;
}

/** @}*/     /* DHT11 驱动模块 */

/**********************************END OF FILE*********************************/

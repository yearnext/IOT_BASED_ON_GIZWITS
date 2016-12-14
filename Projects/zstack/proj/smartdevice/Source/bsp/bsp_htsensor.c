 /**
 ******************************************************************************
  * @file       bsp_htsensor.c
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      温湿度传感器 源文件
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
 * @defgroup 温湿度传感器模块
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "hal_dht11.h"
#include <string.h>
#include "bsp_htsensor.h"
#include "Onboard.h"

/* Exported macro ------------------------------------------------------------*/
/** 定义读取温湿度数据超时计数值 */
#define RD_HT_DATA_TIMEOUT_COUNT (3)

#define HT_SENSOR_SCAN_DELAY()   {Onboard_wait(50000);Onboard_wait(50000); \
                                  Onboard_wait(50000);Onboard_wait(50000); \
                                  Onboard_wait(50000);Onboard_wait(50000);}

/* Exported types ------------------------------------------------------------*/
typedef DHT11_DATA_t ht_sensor_t;

/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
// 温湿度传感器控制命令
#define REPOER_HT_SENSOR_DATA (0x10)
#define READ_HT_SENSOR_DATA   (0x11)

/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief       初始化温湿度传感器
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void bspHTSensorInit(void)
{
	hal_dht11_init();
}

/**
 *******************************************************************************
 * @brief       上报温湿度数据
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void reportHTSensorData( void )
{
	MYPROTOCOL_USER_DATA_t user_data;
    uint8 timeout = 0;
    
    memset(&user_data,0,sizeof(user_data));
    
    while(1)
    {
        if( dht11_rd_data((DHT11_DATA_t *)&user_data.data) == true )
        {
            break;
        }
        else
        {
            if( timeout++ >= RD_HT_DATA_TIMEOUT_COUNT )
            {
                ((DHT11_DATA_t *)&user_data.data)->hum = 0xFE;
                ((DHT11_DATA_t *)&user_data.data)->temp = 0xFE;
                break;
            }
            
            HT_SENSOR_SCAN_DELAY();
        }
    }

	user_data.len = sizeof(DHT11_DATA_t);
	user_data.cmd = REPOER_HT_SENSOR_DATA;
    
    MyprotocolSendData(&user_data, NULL, createD2WWaitPacket, MyprotocolD2WSendData );
}

/**
 *******************************************************************************
 * @brief       上报温湿度数据
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void readHTSensorData( void )
{
	MYPROTOCOL_USER_DATA_t user_data;
    uint8 timeout = 0;
    
    memset(&user_data,0,sizeof(user_data));
    
    while(1)
    {
        if( dht11_rd_data((DHT11_DATA_t *)&user_data.data) == true )
        {
            break;
        }
        else
        {
            if( timeout++ >= RD_HT_DATA_TIMEOUT_COUNT )
            {
                ((DHT11_DATA_t *)&user_data.data)->hum = 0xFE;
                ((DHT11_DATA_t *)&user_data.data)->temp = 0xFE;
                break;
            }
            
            HT_SENSOR_SCAN_DELAY();
        }
    }

	user_data.len = sizeof(DHT11_DATA_t);
	user_data.cmd = READ_HT_SENSOR_DATA;
    
	MyprotocolSendData(&user_data, NULL, createD2WWaitPacket, MyprotocolD2WSendData );
}

/**
 *******************************************************************************
 * @brief       温湿度传感器模块命令解析
 * @param       [in/out]  *data    命令数据
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
bool htsensorMessageHandler(MYPROTOCOL_FORMAT_t *recPacket)
{
	switch(recPacket->user_data.cmd)
	{
        case MYPROTOCOL_TICK_CMD:
            break;
        case MYPROTOCOL_RESET_CMD:
        case MYPROTOCOL_REBOOT_CMD:
            Onboard_soft_reset();
            break;
        case READ_HT_SENSOR_DATA:
#if USE_MYPROTOCOL_DEBUG
        MYPROTOCOL_LOG("htsensor need send data! \r\n");
#endif
            readHTSensorData();
			break;
		default:
            return false;
			break;
	}
	
	return true;
}

/** @}*/     /* 温湿度传感器模块 */

/**********************************END OF FILE*********************************/

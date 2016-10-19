 /**
 ******************************************************************************
  * @file       app_save.c
  * @author     yearnext
  * @version    1.0.0
  * @date       2016年9月17日
  * @brief      Flash 存储功能源文件
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
 * @defgroup Flash 存储功能模块
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "app_save.h"
#include <string.h>

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
 *******************************************************************************
 * @brief       恢复设备数据至出厂数据
 * @param       [in/out]  device_id    设备ID
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void Rst_DeviceSaveData( uint8 device_id )
{
    const uint16 device_save_id = 0x0201;
    
    switch( device_id )
    {
        case DEVICE_COORD_ID:
        {
            uint8 init_data[DEVICE_COORD_DATA_SIZE];
            
            memset(init_data,0,sizeof(init_data));

            osal_nv_write(device_save_id, 0, DEVICE_COORD_DATA_SIZE, init_data );
            break;
        }
        case DEVICE_LIGHT_ID:
        {
            uint8 init_data[DEVICE_LIGHT_DATA_SIZE];
            
            memset(init_data,0,sizeof(init_data));
            
            osal_nv_write(device_save_id, 0, DEVICE_LIGHT_DATA_SIZE, init_data );
            break;
        }
        case DEVICE_SOCKET_ID:
        {
            uint8 init_data[DEVICE_SOCKET_DATA_SIZE];
            
            memset(init_data,0,sizeof(init_data));
            
            osal_nv_write(device_save_id, 0, DEVICE_SOCKET_DATA_SIZE, init_data );
            break;
        }
        case  DEVICE_CURTAIN_ID:
        {
            uint8 init_data[DEVICE_CURTAIN_DATA_SIZE];
            
            memset(init_data,0,sizeof(init_data));
            
            osal_nv_write(device_save_id, 0, DEVICE_CURTAIN_DATA_SIZE, init_data );
            break;
        }
        case DEVICE_DEVICE_TEMP_HUM_SENSOR_ID:
        {
            uint8 init_data[DEVICE_DEVICE_TEMP_HUM_SENSOR_DATA_SIZE];
            
            memset(init_data,0,sizeof(init_data));

            osal_nv_write(device_save_id, 0, DEVICE_DEVICE_TEMP_HUM_SENSOR_DATA_SIZE, init_data );
            
            break;
        }
        default:
            break;
    }
}

/**
 *******************************************************************************
 * @brief       初始化出厂密钥
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
void Device_FirstWriteKey_Init( void )
{
    uint8 key = 0;
    
    key = DEVICE_FIRST_WRIYE_KEY;
    osal_nv_write(DEVICE_LIGHT_SAVE_ID,0,1,(void *)&key);
}

/**
 *******************************************************************************
 * @brief       检测出厂密钥
 * @param       [in/out]  void
 * @return      [in/out]  void
 * @note        None
 *******************************************************************************
 */
bool Device_FirstWrite_Check( void )
{
    uint8 key = 0;
    
    if( osal_nv_read(DEVICE_FIRSTWRITEKEY_ID,0,1,(void *)&key) != SUCCESS )
    {
        osal_nv_item_init(DEVICE_LIGHT_SAVE_ID,DEVICE_LIGHT_DATA_SIZE,NULL);
        
        Device_FirstWriteKey_Init();
        
        return false;
    }
    else
    {
        if( key != DEVICE_FIRST_WRIYE_KEY )
        {
            Device_FirstWriteKey_Init();
            return false;
        }
        else
        {
            return true;
        }
    }
}

/** @}*/     /* Flash 存储功能模块 */

/**********************************END OF FILE*********************************/

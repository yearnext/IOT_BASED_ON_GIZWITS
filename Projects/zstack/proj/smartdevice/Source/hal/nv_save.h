/**
 ******************************************************************************
  * @file       nv_save.c
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
  * 1.保存的Flash地址范围为0x0201-0x0FFF                						                      
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CC2530_FLASH_SAVE_H__
#define __CC2530_FLASH_SAVE_H__

/* Includes ------------------------------------------------------------------*/
#include "comdef.h"
#include "OSAL_Nv.h"

/* Exported macro ------------------------------------------------------------*/
// 设备存储地址
#define DEVICE_COORD_SAVE_ID                    (0x0201)
#define DEVICE_LIGHT_SAVE_ID                    (0x0201)
#define DEVICE_SOCKET_SAVE_ID                   (0x0201)
#define DEVICE_CURTAIN_SAVE_ID                  (0x0201)
#define DEVICE_DEVICE_TEMP_HUM_SENSOR_SAVE_ID   (0x0201)
//设备占用存储空间大小
#define Cal_DataSize(data)                      ( sizeof(data) / sizeof(unsigned char) )
#define DEVICE_COORD_DATA_SIZE                  (Cal_DataSize(DEVICE_COORD_SAVE_DATA))
#define DEVICE_LIGHT_DATA_SIZE                  (Cal_DataSize(DEVICE_LIGHT_SAVE_DATA))
#define DEVICE_SOCKET_DATA_SIZE                 (Cal_DataSize(DEVICE_SOCKET_SAVE_DATA))
#define DEVICE_CURTAIN_DATA_SIZE                (Cal_DataSize(DEVICE_CURTAIN_SAVE_DATA))
#define DEVICE_DEVICE_TEMP_HUM_SENSOR_DATA_SIZE (Cal_DataSize(DEVICE_TEMP_HUM_SENSOR_SAVE_DATA))
//设备ID
#define DEVICE_COORD_ID                         (0x00)
#define DEVICE_LIGHT_ID                         (0x01)
#define DEVICE_SOCKET_ID                        (0x02)
#define DEVICE_CURTAIN_ID                       (0x03)
#define DEVICE_DEVICE_TEMP_HUM_SENSOR_ID        (0x04)
// 初次写入数据密钥
#define DEVICE_FIRST_WRIYE_KEY (0x5A)

/* Exported types ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
// COORD 设备存储数据
typedef struct
{
    uint8 firstwritekey;
}DEVICE_BASE_SAVE_DATA;

// 设备定时器工作模式
typedef struct
{
    uint16 TIMER_Working_Once      : 1;
    uint16 TIMER_Working_EveryDay  : 1;
    uint16 TIMER_Working_Custom    : 1;
    uint16 TIMER_Working_Monday    : 1;
    uint16 TIMER_Working_Tuesday   : 1;
    uint16 TIMER_Working_Wednesday : 1;
    uint16 TIMER_Working_Thursday  : 1;
    uint16 TIMER_Working_Friday    : 1;
    uint16 TIMER_Working_Saturday  : 1;
    uint16 TIMER_Working_Sunday    : 1;
}DEVICE_TIMER_MODE;
// 设备定时器类型
typedef struct
{
    uint8 timer_type;
    DEVICE_TIMER_MODE mode;
    uint8 device_status;
    uint8 statr_hour;
    uint8 start_minute;
    uint8 stop_hour;
    uint8 stop_minute;
}DEVICE_TIMER;
// 简单设备存储数据
typedef struct
{
    DEVICE_BASE_SAVE_DATA key;
    
    struct
    {
        uint8 status;
        DEVICE_TIMER normal_timer;
        DEVICE_TIMER cntdown_timer;
    }data;  
}SIMPLE_DEVICE_SAVE_DATA;

// 简单设备存储数据
typedef struct
{
    DEVICE_BASE_SAVE_DATA Key;
    
    struct
    {
        uint8 status;
        
        struct
        {
            uint8 mode;
            uint8 active_data;
        }smart;
        
        DEVICE_TIMER normal_timer;
        DEVICE_TIMER cntdown_timer;
    }data;  
}DEVICE_CURTAIN_SAVE_DATA;

typedef DEVICE_BASE_SAVE_DATA   DEVICE_COORD_SAVE_DATA;
typedef DEVICE_BASE_SAVE_DATA   DEVICE_TEMP_HUM_SENSOR_SAVE_DATA;
typedef SIMPLE_DEVICE_SAVE_DATA DEVICE_LIGHT_SAVE_DATA;
typedef SIMPLE_DEVICE_SAVE_DATA DEVICE_SOCKET_SAVE_DATA;

/* Private variables ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
extern void Rst_DeviceSaveData( uint8 );

#endif      /* __CC2530_FLASH_SAVE_H__ */

/**********************************END OF FILE*********************************/

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
  * 1.保存的Flash地址范围为0x0201-0x0FFF                						                      
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CC2530_FLASH_SAVE_H__
#define __CC2530_FLASH_SAVE_H__

/* Includes ------------------------------------------------------------------*/
#include "comdef.h"
#include "osal_nv.h"

/* Exported macro ------------------------------------------------------------*/
// 设备存储地址
#define DEVICE_FIRSTWRITEKEY_ID                 (0x0202)
#define DEVICE_COORD_SAVE_ID                    (0x0210)
#define DEVICE_LIGHT_SAVE_ID                    (0x0210)
#define DEVICE_SOCKET_SAVE_ID                   (0x0210)
#define DEVICE_CURTAIN_SAVE_ID                  (0x0210)
#define DEVICE_DEVICE_TEMP_HUM_SENSOR_SAVE_ID   (0x0210)

//设备占用存储空间大小
#define calSaveDataSize(data)                   ( sizeof(data) / sizeof(unsigned char) )

//数据保存使能标志
#define USE_DATA_SAVE                           (0)

// 初次写入数据密钥
#define DEVICE_FIRST_WRIYE_KEY                  (0x5A)

// 配置智能设备的定时器数量
#define SIMPLE_DEVICE_TIMER_NUM                 (2)

/* Exported types ------------------------------------------------------------*/
//加载数据错误功能
typedef void (*load_flase_handler)(void);

/* Exported variables --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
#if USE_DATA_SAVE
extern bool deviceSaveData( uint16 id, uint16 size, void *ctx );
extern bool deviceLoadData( uint16, uint16, void*, load_flase_handler );

#else
#define deviceSaveData(a,b,c) 
#define deviceLoadData(a,b,c,d)

#endif

#endif      /* __CC2530_FLASH_SAVE_H__ */

/**********************************END OF FILE*********************************/
